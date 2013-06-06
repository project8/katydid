/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTAnalyticAssociateData.hh"
#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumFFTW.hh"
//#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#include <algorithm>

using std::string;
using std::vector;

using boost::shared_ptr;

namespace Katydid
{

    static KTDerivedNORegistrar< KTProcessor, KTWignerVille > sWVRegistrar("wigner-ville");

    KTWignerVille::KTWignerVille(const std::string& name) :
            KTProcessor(name),
            fPairs(),
            fBuffer(),
            fWindowSize(1),
            fWindowStride(1),
            fNWindowsToAverage(1),
            fInputArray(new KTTimeSeriesFFTW(1,0.,1.)),
            fFFT(new KTComplexFFTW()),
            fOutputArrays(),
            fOutputData(new KTData()),
            fOutputSHData(NULL),
            fOutputWVData(NULL),
            fWindowAverageCounter(0),
            fWindowCounter(0),
            fDataOutCounter(0),
            //fLeftStartPointer(0),
            fWVSignal("wigner-ville", this),
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

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        const KTPStoreNode* childNode = node->GetChild("complex-fftw");
        if (childNode != NULL)
        {
            fFFT->Configure(childNode);
        }

        KTPStoreNode::csi_pair itPair = node->EqualRange("wv-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            UIntPair pair = ParsePairUInt(citer->second.get_value< string >());
            KTINFO(wvlog, "Adding WV pair " << pair.first << ", " << pair.second);
            this->AddPair(pair);
        }

        SetWindowSize(node->GetData< UInt_t >("window-size", fWindowSize));
        SetWindowStride(node->GetData< UInt_t >("window-stride", fWindowStride));
        SetNWindowsToAverage(node->GetData< UInt_t >("n-windows-to-average", fNWindowsToAverage));

        return true;
    }

    void KTWignerVille::Initialize(Double_t acqRate, UInt_t nComponents, UInt_t inputSliceSize)
    {
        if (fPairs.empty())
        {
            KTWARN(wvlog, "No Wigner-Ville pairs specified; no transforms performed.");
            return;
        }
        UInt_t nPairs = fPairs.size();

        if (fNWindowsToAverage == 0) fNWindowsToAverage = 1;

        // initialize the FFT
        fFFT->SetSize(fWindowSize);
        fFFT->InitializeFFT();

        Double_t timeBW = 1. / acqRate;

        // initialize the input array
        delete fInputArray;
        fInputArray = new KTTimeSeriesFFTW(fWindowSize, 0., Double_t(fWindowSize) * timeBW);

        // initialize the output arrays
        if (fNWindowsToAverage > 1)
        {
            while (! fOutputArrays.empty())
            {
                delete fOutputArrays.back();
                fOutputArrays.pop_back();
            }
            fOutputArrays.resize(nPairs);
            for (UInt_t iPair = 0; iPair < nPairs; iPair++)
            {
                fOutputArrays[iPair] = new KTFrequencySpectrumFFTW(fWindowSize, fFFT->GetMinFrequency(timeBW), fFFT->GetMaxFrequency(timeBW));
            }
        }

        // initialize the circular buffer
        fBuffer.resize(nComponents);
        for (UInt_t iChannel = 0; iChannel < fBuffer.size(); iChannel++)
        {
            fBuffer[iChannel].clear();
            fBuffer[iChannel].set_capacity(inputSliceSize + fWindowSize);
        }

        // initialize the output data
        fOutputData.reset(new KTData());

        // slice header
        fOutputSHData = &(fOutputData->Of< KTSliceHeader >().SetNComponents(nPairs));
        fOutputSHData->SetSampleRate(acqRate);
        fOutputSHData->SetSliceSize(fWindowSize);
        fOutputSHData->CalculateBinWidthAndSliceLength();

        //fOutputWVData = &(fOutputData->Of< KTWV2DData >().SetNComponents(fPairs.size()));
        fOutputWVData = &(fOutputData->Of< KTWignerVilleData >().SetNComponents(nPairs));
        //UInt_t nWindows = 897;
        UInt_t iPair = 0;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;
            fOutputWVData->SetInputPair(firstChannel, secondChannel, iPair);
            /*
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* newSpectra = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >(nWindows, -0.5 * timeBW, timeBW * (Double_t(nWindows) - 0.5));
            for (UInt_t iSpectrum = 0; iSpectrum < nWindows; iSpectrum++)
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

        return;
    }

    void KTWignerVille::InitializeWithHeader(const KTEggHeader* header)
    {
        return Initialize(header->GetAcquisitionRate(), header->GetNChannels(), header->GetSliceSize());
    }

    Bool_t KTWignerVille::TransformData(KTTimeSeriesData& data, Bool_t clearBuffers)
    {
        return TransformFFTWBasedData(data, clearBuffers);
    }

    Bool_t KTWignerVille::TransformData(KTAnalyticAssociateData& data, Bool_t clearBuffers)
    {
        return TransformFFTWBasedData(data, clearBuffers);
    }

    void KTWignerVille::CalculateACF(Buffer::iterator data1It, const Buffer::iterator& data2End, UInt_t iWindow)
    {
        // data2It will be decremented before it's used
        Buffer::iterator data2It = data2End + fWindowSize;

        register Double_t t1_real;
        register Double_t t1_imag;
        register Double_t t2_real;
        register Double_t t2_imag;

        KTERROR(wvlog, "iWindow = " << iWindow);

        for (UInt_t fftBin = 0; fftBin < fWindowSize; fftBin++)
        {
            // decrement data2It first so that it doesn't run past begin() on the first window
            data2It--;
            t1_real = data1It->real();
            t1_imag = data1It->imag();
            t2_real = data2It->real();
            t2_imag = data2It->imag();
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            KTWARN(wvlog, "  " << fftBin << " -- " << t1_real << "  " << t1_imag << " -- " << t2_real << "  " << t2_imag << " -- " << (*fInputArray)(fftBin)[0] << "  " << (*fInputArray)(fftBin)[1]);
            data1It++;
        }

        return;
    }


    void KTWignerVille::CalculateLaggedACF(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2, UInt_t offset)
    {
        UInt_t sliceSize = data1->size();
        UInt_t fftSize = GetWindowSize();

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
        fInputArray->SetRange(0., (Double_t)fftSize * data1->GetBinWidth());

        KTERROR(wvlog, "offset = " << offset << "  inArr Size = " << fInputArray->size() << "  data1 Size = " << data1->size() << "  data2 Size = " << data2->size());

        // Now calculate the lagged ACF at all possible lags.
        register Double_t t1_real;
        register Double_t t1_imag;
        register Double_t t2_real;
        register Double_t t2_imag;
/*
        UInt_t binsToFill = std::min(fftSize, sliceSize - fLeftStartPointer);
        register UInt_t rightStartPointer = fLeftStartPointer + binsToFill - 1;

        for (UInt_t fftBin = 0; fftBin < binsToFill; fftBin++)
        {
            t1_real = (*data1)(fLeftStartPointer + fftBin)[0];
            t1_imag = (*data1)(fLeftStartPointer + fftBin)[1];
            t2_real = (*data1)(rightStartPointer - fftBin)[0];
            t2_imag = (*data1)(rightStartPointer - fftBin)[1];
            (*fInputArray)(fftBin)[0] = t1_real * t2_real + t1_imag * t2_imag;
            (*fInputArray)(fftBin)[1] = t1_imag * t2_real - t1_real * t2_imag;
            KTWARN(wvlog, "  " << binsToFill << "  " << fLeftStartPointer << "  " << rightStartPointer << "  " << fftBin << " -- " << fLeftStartPointer + fftBin << "  " << (*data1)(fLeftStartPointer + fftBin)[0] << "  " << (*data1)(fLeftStartPointer + fftBin)[1] << " -- " << rightStartPointer - fftBin << "  " << (*data2)(rightStartPointer - fftBin)[0] << "  " << (*data2)(rightStartPointer - fftBin)[1] << " -- " << (*fInputArray)(fftBin)[0] << "  " << (*fInputArray)(fftBin)[1]);
        }
        for (UInt_t fftBin = binsToFill; fftBin < fftSize; fftBin++)
        {
            (*fInputArray)(fftBin)[0] = 0.;
            (*fInputArray)(fftBin)[1] = 0.;
        }
*/
/*
        register UInt_t time = offset;
        register Int_t taumax = std::min(std::min((Int_t)time, (Int_t)sliceSize - (Int_t)time -1), (Int_t)fftSize/2-1);
        KTERROR(wvlog, "time = " << time << "  taumax = " << taumax);

        UInt_t fftBin = 0;
        for (Int_t tau = -taumax; tau <= taumax; tau++)
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
        //register UInt_t tau_plus = size - 1;
        //register UInt_t tau_minus = 0;
        ///register UInt_t start = (UInt_t)std::max(0, (Int_t)offset - ((Int_t)size - 1));
        ///register UInt_t end = (UInt_t)std::min((Int_t)offset, (Int_t)size - 1);
        ///for (UInt_t inArrBin = 0; inArrBin < start; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
        ///for (UInt_t inArrBin = start; inArrBin <= end; inArrBin++)
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
        ///for (UInt_t inArrBin = end + 1; inArrBin < size; inArrBin++)
        ///{
        ///    (*fInputArray)(inArrBin)[0] = 0.;
        ///    (*fInputArray)(inArrBin)[1] = 0.;
        ///    KTINFO(wvlog, "  " << inArrBin << " -- 0 -- 0");
        ///}
    }

    void KTWignerVille::ProcessTimeSeries(shared_ptr< KTData > data)
    {
        // Standard data slot pattern, except the signal is called asynchronously

        // Check to ensure that the required data type is present
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(slotlog, "Data not found with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        Bool_t clearBuffers = false;
        if (data->Has< KTSliceHeader >())
        {
            clearBuffers = data->Of< KTSliceHeader >().GetIsNewAcquisition();
        }

        // Call the function
        if (! TransformData(data->Of< KTTimeSeriesData >(), clearBuffers))
        {
            KTERROR(slotlog, "Something went wrong while analyzing data with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        // the signal is not called here because the WV data is asynchronous with the input slices

        return;
    }

    void KTWignerVille::ProcessAnalyticAssociate(shared_ptr< KTData > data)
    {
        // Standard data slot pattern, except the signal is called asynchronously

        // Check to ensure that the required data type is present
        if (! data->Has< KTAnalyticAssociateData >())
        {
            KTERROR(slotlog, "Data not found with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        Bool_t clearBuffers = false;
        if (data->Has< KTSliceHeader >())
        {
            clearBuffers = data->Of< KTSliceHeader >().GetIsNewAcquisition();
        }

        // Call the function
        if (! TransformData(data->Of< KTAnalyticAssociateData >(), clearBuffers))
        {
            KTERROR(slotlog, "Something went wrong while analyzing data with type <" << typeid(KTTimeSeriesData).name() << ">");
            return;
        }

        // the signal is not called here because the WV data is asynchronous with the input slices

        return;
    }

} /* namespace Katydid */
