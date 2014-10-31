/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumFFTW.hh"
//#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTWindower.hh"

#include <algorithm>

using std::string;
using std::vector;



namespace Katydid
{

    KT_REGISTER_PROCESSOR(KTWignerVille, "wigner-ville");

    KTWignerVille::KTWignerVille(const std::string& name) :
            KTProcessor(name),
            fPairs(),
            fWindowSize(1),
            fWindowStride(1),
            fNWindowsToAverage(1),
            fFirstHeader(),
            fSecondHeader(),
            fReceivedLastData(false),
            fBuffer(),
            fSliceSampleOffset(0),
            fAdvanceStartIteratorOnNewSlice(false),
            fSliceBreak(),
            fInputArray(new KTTimeSeriesFFTW(1,0.,1.)),
            fUseWindowFunction(false),
            fWindower(new KTWindower()),
            fFFT(new KTComplexFFTW()),
            fOutputArrays(),
            fOutputData(new KTData()),
            fOutputSHData(NULL),
            fOutputWVData(NULL),
            fWindowAverageCounter(0),
            fWindowCounter(0),
            fDataOutCounter(0),
            fWVSignal("wv", this),
            fHeaderSlot("header", this, &KTWignerVille::InitializeWithHeader)
    {
        RegisterSlot("ts", this, &KTWignerVille::ProcessTimeSeries);
        RegisterSlot("aa", this, &KTWignerVille::ProcessAnalyticAssociate);
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fInputArray;
        delete fFFT;
        while (! fOutputArrays.empty())
        {
            delete fOutputArrays.back();
            fOutputArrays.pop_back();
        }
    }

    bool KTWignerVille::Configure(const KTParamNode* node)
    {
        fFFT->Configure(node->NodeAt("complex-fftw"));

        const KTParamNode* windowerNode = node->NodeAt("windower");
        if (windowerNode != NULL)
        {
            fUseWindowFunction = true;
            fWindower->Configure(windowerNode);
        }

        const KTParamArray* wvPairs = node->ArrayAt("wv-pairs");
        if (wvPairs != NULL)
        {
            for (KTParamArray::const_iterator pairIt = wvPairs->Begin(); pairIt != wvPairs->End(); ++pairIt)
            {
                if (! ((*pairIt)->IsArray() && (*pairIt)->AsArray().Size() == 2))
                {
                    KTERROR(wvlog, "Invalid pair: " << (*pairIt)->ToString());
                    return false;
                }
                UIntPair pair((*pairIt)->AsArray().GetValue< unsigned >(0), (*pairIt)->AsArray().GetValue< unsigned >(1));
                KTINFO(wvlog, "Adding WV pair " << pair.first << ", " << pair.second);
                this->AddPair(pair);
            }
        }

        SetWindowSize(node->GetValue< unsigned >("window-size", fWindowSize));
        SetWindowStride(node->GetValue< unsigned >("window-stride", fWindowStride));
        SetNWindowsToAverage(node->GetValue< unsigned >("n-windows-to-average", fNWindowsToAverage));

        return true;
    }

    bool KTWignerVille::Initialize(double acqRate, unsigned nComponents, unsigned inputSliceSize)
    {
        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return false;
        }
        unsigned nPairs = fPairs.size();

        if (fNWindowsToAverage == 0) fNWindowsToAverage = 1;

        // initialize the Windower
        if (fUseWindowFunction)
        {
            fWindower->InitializeWindow(1. / acqRate, fWindowSize);
        }

        // initialize the FFT
        fFFT->SetSize(fWindowSize);
        fFFT->InitializeFFT();

        double timeBW = 1. / acqRate;

        // initialize the input array
        delete fInputArray;
        fInputArray = new KTTimeSeriesFFTW(fWindowSize, 0., double(fWindowSize) * timeBW);

        // initialize the output arrays
        if (fNWindowsToAverage > 1)
        {
            while (! fOutputArrays.empty())
            {
                delete fOutputArrays.back();
                fOutputArrays.pop_back();
            }
            fOutputArrays.resize(nPairs);
            for (unsigned iPair = 0; iPair < nPairs; iPair++)
            {
                fOutputArrays[iPair] = new KTFrequencySpectrumFFTW(fWindowSize, fFFT->GetMinFrequency(timeBW), fFFT->GetMaxFrequency(timeBW));
            }
        }

        // initialize the circular buffer
        fBuffer.resize(nComponents);
        fSliceBreak.resize(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            fBuffer[iComponent].clear();
            fBuffer[iComponent].set_capacity(inputSliceSize + fWindowSize);
            fSliceBreak[iComponent] = fBuffer[iComponent].end();
        }

        fSliceSampleOffset = 0;
        fAdvanceStartIteratorOnNewSlice = false;

        // initialize the output data
        fOutputData.reset(new KTData());

        // slice header
        fFirstHeader.SetNComponents(nPairs);
        fSecondHeader.SetNComponents(nPairs);

        // The order for creating the data types is important: slice header _then_ WV data because this class
        // uses the same data object for all of the slices it generates.
        // This way KTExtensibleStruct::Clear can be called on the WV data to remove any downstream data
        // that was added in a previous slice
        fOutputSHData = &(fOutputData->Of< KTSliceHeader >().SetNComponents(nPairs));
        fOutputSHData->SetSampleRate(acqRate);
        fOutputSHData->SetSliceSize(fWindowSize + (fNWindowsToAverage - 1) * fWindowStride); // this includes the fact that windows might be averaged together
        fOutputSHData->SetRawSliceSize(fOutputSHData->GetSliceSize());
        fOutputSHData->CalculateBinWidthAndSliceLength();
        fOutputSHData->SetNonOverlapFrac((double)(fNWindowsToAverage * fWindowStride) / (double)fOutputSHData->GetSliceSize()); // this includes the fact that windows might be averaged together

        //fOutputWVData = &(fOutputData->Of< KTWV2DData >().SetNComponents(fPairs.size()));
        fOutputWVData = &(fOutputData->Of< KTWignerVilleData >().SetNComponents(nPairs));
        //unsigned nWindows = 897;
        unsigned iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            unsigned firstChannel = (*pairIt).first;
            unsigned secondChannel = (*pairIt).second;
            fOutputWVData->SetInputPair(firstChannel, secondChannel, iPair);
            /*
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nWindows, -0.5 * timeBW, timeBW * (double(nWindows) - 0.5));
            for (unsigned iSpectrum = 0; iSpectrum < nWindows; iSpectrum++)
            {
                (*newSpectra)(iSpectrum) = NULL;
            }
            fOutputWVData->SetSpectra(newSpectra, iPair);
            */
            iPair++;
        }

        fDataOutCounter = 0;
        fWindowCounter = 0;
        fWindowAverageCounter = 0;

        return true;
    }

    bool KTWignerVille::InitializeWithHeader(KTEggHeader& header)
    {
        return Initialize(header.GetAcquisitionRate(), header.GetNChannels(), header.GetSliceSize());
    }

    bool KTWignerVille::TransformData(KTTimeSeriesData& data, KTSliceHeader& header)
    {
        return TransformFFTWBasedData(data, header);
    }

    bool KTWignerVille::TransformData(KTAnalyticAssociateData& data, KTSliceHeader& header)
    {
        return TransformFFTWBasedData(data, header);
    }

    void KTWignerVille::CalculateACF(Buffer::iterator data1It, const Buffer::iterator& data2End, unsigned /*iWindow*/)
    {
        // data2It will be decremented before it's used
        Buffer::iterator data2It = data2End + fWindowSize;

        register double t1_real;
        register double t1_imag;
        register double t2_real;
        register double t2_imag;

        //KTERROR(wvlog, "iWindow = " << iWindow);

        for (unsigned fftBin = 0; fftBin < fWindowSize; fftBin++)
        {
            // decrement data2It first so that it doesn't run past begin() on the first window
            --data2It;
            t1_real = data1It->real();
            t1_imag = data1It->imag();
            t2_real = data2It->real();
            t2_imag = data2It->imag();
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            //KTWARN(wvlog, "  " << fftBin << " -- " << t1_real << "  " << t1_imag << " -- " << t2_real << "  " << t2_imag << " -- " << (*fInputArray)(fftBin)[0] << "  " << (*fInputArray)(fftBin)[1]);
            ++data1It;
        }

        if (fUseWindowFunction)
        {
            fWindower->ApplyWindow(fInputArray);
        }

        return;
    }


    void KTWignerVille::CalculateLaggedACF(const KTTimeSeriesFFTW* /*data1*/, const KTTimeSeriesFFTW* /*data2*/, unsigned /*offset*/)
    {
        //unsigned sliceSize = data1->size();
        //unsigned fftSize = GetWindowSize();

        /*
        if (fInputArray->size() != size)
        {
            delete fInputArray;
            fInputArray = new KTTimeSeriesFFTW(size,
                    data1->GetRangeMin(),
                    data1->GetRangeMax() + 0.5 * data1->GetBinWidth());
            KTWARN(wvlog, "Setting the input array size to " << size);
        }
        else
        {
            fInputArray->SetRange(data1->GetRangeMin(), data1->GetRangeMax());
        }
        */
        //fInputArray->SetRange(0., (double)fftSize * data1->GetBinWidth());

        //KTERROR(wvlog, "offset = " << offset << "  inArr Size = " << fInputArray->size() << "  data1 Size = " << data1->size() << "  data2 Size = " << data2->size());

        // Now calculate the lagged ACF at all possible lags.
        //register double t1_real;
        //register double t1_imag;
        //register double t2_real;
        //register double t2_imag;
/*
        unsigned binsToFill = std::min(fftSize, sliceSize - fLeftStartPointer);
        register unsigned rightStartPointer = fLeftStartPointer + binsToFill - 1;

        for (unsigned fftBin = 0; fftBin < binsToFill; fftBin++)
        {
            t1_real = (*data1)(fLeftStartPointer + fftBin)[0];
            t1_imag = (*data1)(fLeftStartPointer + fftBin)[1];
            t2_real = (*data1)(rightStartPointer - fftBin)[0];
            t2_imag = (*data1)(rightStartPointer - fftBin)[1];
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            KTWARN(wvlog, "  " << binsToFill << "  " << fLeftStartPointer << "  " << rightStartPointer << "  " << fftBin << " -- " << fLeftStartPointer + fftBin << "  " << (*data1)(fLeftStartPointer + fftBin)[0] << "  " << (*data1)(fLeftStartPointer + fftBin)[1] << " -- " << rightStartPointer - fftBin << "  " << (*data2)(rightStartPointer - fftBin)[0] << "  " << (*data2)(rightStartPointer - fftBin)[1] << " -- " << (*fInputArray)(fftBin)[0] << "  " << (*fInputArray)(fftBin)[1]);
        }
        for (unsigned fftBin = binsToFill; fftBin < fftSize; fftBin++)
        {
            (*fInputArray)(fftBin)[0] = 0.;
            (*fInputArray)(fftBin)[1] = 0.;
        }
*/
/*
        register unsigned time = offset;
        register int taumax = std::min(std::min((int)time, (int)sliceSize - (int)time -1), (int)fftSize/2-1);
        KTERROR(wvlog, "time = " << time << "  taumax = " << taumax);

        unsigned fftBin = 0;
        for (int tau = -taumax; tau <= taumax; tau++)
        {
            t1_real = (*data1)(time + tau)[0];
            t1_imag = (*data1)(time + tau)[1];
            t2_real = (*data2)(time - tau)[0];
            t2_imag = (*data2)(time - tau)[1];
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            KTWARN(wvlog, "  " << time << "  " << taumax << "  " << tau << "  " << fftBin << " -- " << time + tau << "  " << (*data1)(time+tau)[0] << "  " << (*data1)(time+tau)[1] << " -- " << time - tau << "  " << (*data2)(time-tau)[0] << "  " << (*data2)(time-tau)[1]);
            fftBin++;
        }
        for (; fftBin < fftSize; fftBin++)
        {
            (*fInputArray)(fftBin)[0] = 0.;
            (*fInputArray)(fftBin)[1] = 0.;
        }
*/
        //register unsigned tau_plus = size - 1;
        //register unsigned tau_minus = 0;
        ///register unsigned start = (unsigned)std::max(0, (int)offset - ((int)size - 1));
        ///register unsigned end = (unsigned)std::min((int)offset, (int)size - 1);
        ///for (unsigned inArrBin = 0; inArrBin < start; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
        ///for (unsigned inArrBin = start; inArrBin <= end; inArrBin++)
        ///{
            //t1_real = (*data1)(offset + tau_minus)[0];
            //t1_imag = (*data1)(offset + tau_minus)[1];
            //t2_real = (*data2)(offset + tau_plus)[0];
            //t2_imag = (*data2)(offset + tau_plus)[1];
            ///t1_real = (*data1)(inArrBin)[0];
            ///t1_imag = (*data1)(inArrBin)[1];
            ///t2_real = (*data2)(offset - inArrBin)[0];
            ///t2_imag = (*data2)(offset - inArrBin)[1];

            ///(*fInputArray)(inArrBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            ///(*fInputArray)(inArrBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            ///KTWARN(wvlog, "  " << inArrBin << " -- " << inArrBin << "  " << (*data1)(inArrBin)[0] << "  " << (*data1)(inArrBin)[1] << " -- " << offset - inArrBin << "  " << (*data2)(inArrBin)[0] << "  " << (*data2)(inArrBin)[1]);
            //KTWARN(wvlog, "  " << inArrBin << " -- " << tau_minus << "  " << offset + tau_minus << " = " << t1_real << "  " << t1_imag << " -- " << tau_plus << "  " << offset + tau_plus << " = " << t2_real << "  " << t2_imag << " -- " << (*fInputArray)(inArrBin)[0] << "  " << (*fInputArray)(inArrBin)[1]);

            //tau_minus++;
            //tau_plus--;
        ///}
        ///for (unsigned inArrBin = end + 1; inArrBin < size; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
    }

    void KTWignerVille::ProcessTimeSeries(KTDataPtr data)
    {
        // Standard data slot pattern, except the signal is called asynchronously

        // Check to ensure that the required data type is present
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(wvlog, "Data not found with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(wvlog, "Data not found with type <" << typeid(KTSliceHeader).name() << ">");
            return;
        }

        fReceivedLastData = data->GetLastData();

        // Call the function
        if (! TransformData(data->Of< KTTimeSeriesData >(), data->Of< KTSliceHeader >()))
        {
            KTERROR(wvlog, "Something went wrong while analyzing data with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        // the signal is not called here because the WV data is asynchronous with the input slices

        return;
    }

    void KTWignerVille::ProcessAnalyticAssociate(KTDataPtr data)
    {
        // Standard data slot pattern, except the signal is called asynchronously

        // Check to ensure that the required data type is present
        if (! data->Has< KTAnalyticAssociateData >())
        {
            KTERROR(wvlog, "Data not found with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(wvlog, "Data not found with type <" << typeid(KTSliceHeader).name() << ">");
            return;
        }

        fReceivedLastData = data->GetLastData();

        // Call the function
        if (! TransformData(data->Of< KTAnalyticAssociateData >(), data->Of< KTSliceHeader >()))
        {
            KTERROR(wvlog, "Something went wrong while analyzing data with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        // the signal is not called here because the WV data is asynchronous with the input slices

        return;
    }

} /* namespace Katydid */
