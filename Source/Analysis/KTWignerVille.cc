/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTComplexFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesPairedData.hh"
#include "KTPStoreNode.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <algorithm>
#include <set>

using std::copy;
using std::map;
using std::set;
using std::string;

// I can't just use boost::spirit::qi because of naming conflicts with std
using boost::spirit::qi::int_;
// I had to take this out because of a naming conflict with boost::bind
//using boost::spirit::qi::_1;
using boost::spirit::qi::phrase_parse;
using boost::spirit::ascii::space;
using boost::phoenix::ref;


namespace Katydid
{
    KTLOGGER(wvlog, "katydid.analysis");

    KTWignerVille::KTWignerVille() :
            KTProcessor(),
            fFullFFT(NULL),
            fSaveFrequencySpectrum(false),
            fWVSignal()
    {
        fConfigName = "wigner-ville";

        RegisterSignal("wigner-ville", &fWVSignal, "void (const KTWriteableData*)");

        //RegisterSlot("header", this, &KTWignerVille::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("ts-data", this, &KTWignerVille::ProcessTimeSeriesData, "void (const KTTimeSeriesData*)");
        RegisterSlot("fs-data", this, &KTWignerVille::ProcessFrequencySpectrumData, "void (const KTFrequencySpectrumDataFFTW*)");
        RegisterSlot("event", this, &KTWignerVille::ProcessEvent, "void (KTEvent*)");
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFullFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        SetSaveFrequencySpectrum(node->GetData< Bool_t >("save-frequency-spectrum", fSaveFrequencySpectrum));

        KTPStoreNode::csi_pair itPair = node->EqualRange("wv-pair");
        for (KTPStoreNode::const_sorted_iterator citer = itPair.first; citer != itPair.second; citer++)
        {
            string pairString(citer->second.get_value< string >());
            UInt_t first = 0, second = 0;
            Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                    (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> int_[ref(second) = boost::spirit::qi::_1]),
                    space);
            if (! parsed)
            {
                KTWARN(wvlog, "Unable to parse WV pair: " << pairString);
                continue;
            }
            KTINFO(wvlog, "Adding WV pair " << first << ", " << second);
            this->AddPair(KTWVPair(first, second));
        }

        const KTPStoreNode* fftNode = node->GetChild("complex-fft");
        if (fftNode != NULL)
        {
            delete fFullFFT;
            fFullFFT = new KTComplexFFTW();
            if (! fFullFFT->Configure(fftNode)) return false;
        }

        return true;
    }

    KTTimeSeriesData* KTWignerVille::TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData)
    {
        if (fFullFFT == NULL)
        {
            KTERROR(wvlog, "FFT is not initialized; cannot perform a transform on time series data.");
            return NULL;
        }

        if (fSaveFrequencySpectrum && outputFSData == NULL)
        {
            KTWARN(wvlog, "The flag for saving the frequency spectrum is set, but no KTFrequencySpectrumDataFFTW** was provide;\n"
                    << "\tThe frequency spectrum will not be saved."
                    << "\tfSaveFrequencySpectrum is being set to false");
            fSaveFrequencySpectrum = false;
        }
        else if (! fSaveFrequencySpectrum && outputFSData != NULL)
        {
            KTWARN(wvlog, "A KTFrequencySpectrumDataFFTW** was supplied to store the intermediate frequency spectrum, but fSaveFrequencySpectrum is false."
                    << "\tTo avoid a potential memory leak, the frequency spectra will not be saved.");
        }

        // make a set of the channel numbers in use so that we can make the analytic associates of only the channels we need
        set< UInt_t > channelsInUse;
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            channelsInUse.insert((*pairIt).first);
            channelsInUse.insert((*pairIt).second);
        }

        // New data to hold the time series of the analytic associate
        KTBasicTimeSeriesData* aaTSData = new KTBasicTimeSeriesData(data->GetNTimeSeries());
        KTFrequencySpectrumDataFFTW* aaFSData = NULL;
        if (fSaveFrequencySpectrum)
        {
            aaFSData = new KTFrequencySpectrumDataFFTW(data->GetNTimeSeries());
            outputFSData = &aaFSData;
        }

        // Calculate the analytic associates
        // Use this map to hold pointers to them so we don't have to dynamic cast later.
        map< UInt_t, const KTTimeSeriesFFTW* > channelAAs;
        for (set< UInt_t >::const_iterator channelIt = channelsInUse.begin(); channelIt != channelsInUse.end(); channelIt++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(data->GetTimeSeries(*channelIt));
            if (nextInput == NULL)
            {
                KTERROR(wvlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                delete aaTSData;
                return NULL;
            }

            KTFrequencySpectrumFFTW* newFS = NULL;
            KTTimeSeriesFFTW* newTS = NULL;
            if (fSaveFrequencySpectrum)
            {
                newTS = CalculateAnalyticAssociate(nextInput, &newFS);
            }
            else
            {
                newTS = CalculateAnalyticAssociate(nextInput);
            }

            if (newFS != NULL)
                aaFSData->SetSpectrum(newFS, *channelIt);

            if (newTS == NULL)
            {
                KTERROR(wvlog, "Channel <" << *channelIt << "> did not transform correctly.");
                delete aaTSData;
                return NULL;
            }

            aaTSData->SetTimeSeries(newTS, *channelIt);
            channelAAs[*channelIt] = newTS;
        }

        // new KTTimeSeriesPairedData to hold the results of the cross multiplication
        KTTimeSeriesPairedData* crossMultipliedData = new KTTimeSeriesPairedData(fPairs.size());

        // Cross-multiply pairs of channels
        for (PairVector::const_iterator pairIt = fPairs.begin(); pairIt != fPairs.end(); pairIt++)
        {
            UInt_t firstChannel = (*pairIt).first;
            UInt_t secondChannel = (*pairIt).second;

            KTTimeSeriesFFTW* newTS = CrossMultiply(channelAAs[firstChannel], channelAAs[secondChannel]);
            if (newTS == NULL)
            {
                KTERROR(wvlog, "Something went wrong in the cross multiplication of channels <" << firstChannel << "> and < " << secondChannel << ">.");
                delete aaTSData;
                delete crossMultipliedData;
                return NULL;
            }

            crossMultipliedData->SetTimeSeries(newTS, firstChannel, secondChannel);
        }




        KTDEBUG(fftlog_comp, "W-V transform complete; " << newTSData->GetNTimeSeries() << " channel(s) transformed");

        return newTSData;
    }
/*
    KTTimeSeriesFFTW* KTWignerVille::Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        if (fFullFFT == NULL)
        {
            KTERROR(wvlog, "FFT is not initialized; cannot perform a transform on time series data.");
            return NULL;
        }

        if (fSaveFrequencySpectrum && outputFS == NULL)
        {
            KTWARN(wvlog, "The flag for saving the frequency spectrum is set, but no KTFrequencySpectrumFFTW** was provide;\n"
                    << "\tThe frequency spectrum will not be saved."
                    << "\tfSaveFrequencySpectrum is being set to false");
            fSaveFrequencySpectrum = false;
        }
        else if (! fSaveFrequencySpectrum && outputFS != NULL)
        {
            KTWARN(wvlog, "A KTFrequencySpectrumDataFFTW** was supplied to store the intermediate frequency spectrum, but fSaveFrequencySpectrum is false."
                    << "\tTo avoid a potential memory leak, the frequency spectra will not be saved.");
        }

        // Calculate the analytic associate
        KTFrequencySpectrumFFTW* tempOutputFS = NULL;
        KTTimeSeriesFFTW* outputTS = CalculateAnalyticAssociate(inputTS, &tempOutputFS);

        // Delete or reassign the intermediate FS
        if (outputFS == NULL) delete tempOutputFS;
        else outputFS = &tempOutputFS;

        return outputTS;
    }

    Bool_t KTWignerVille::Transform(KTFrequencySpectrumFFTW* freqSpectrum)
    {
        if (freqSpectrum == NULL)
        {
            KTERROR(wvlog, "Input frequency spectrum was NULL");
            return false;
        }

        return CalculateAnalyticAssociate(freqSpectrum);
    }
*/

    KTTimeSeriesFFTW* KTWignerVille::CalculateAnalyticAssociate(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS)
    {
        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fFullFFT->Transform(inputTS);
        if (freqSpec == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the forward FFT on the time series.");
            return NULL;
        }
        // copy the address of the frequency spectrum to outputFS
        outputFS = &freqSpec;

        // perform the actual W-V transform
        if (! CalculateAnalyticAssociate(freqSpec))
        {
            KTERROR(wvlog, "Something went wrong with the W-V transform of the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fFullFFT->Transform(freqSpec);
        if (outputTS == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the reverse FFT on the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        return outputTS;
    }


    Bool_t KTWignerVille::CalculateAnalyticAssociate(KTFrequencySpectrumFFTW* freqSpectrum)
    {
        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        fftw_complex* data = freqSpectrum->GetData();
        UInt_t arraySize = freqSpectrum->size();
        UInt_t nyquistPos = arraySize / 2; // either the sole nyquist bin (if even # of bins) or the first of the two (if odd # of bins; bins are sequential in the array).
        for (UInt_t arrayPos=1; arrayPos<nyquistPos; arrayPos++)
        {
            data[arrayPos][0] = data[arrayPos][0] * 2.;
            data[arrayPos][1] = data[arrayPos][1] * 2.;
        }
        for (UInt_t arrayPos=nyquistPos; arrayPos<arraySize; arrayPos++)
        {
            data[arrayPos][0] = 0.;
            data[arrayPos][1] = 0.;
        }
        return true;
    }

    KTTimeSeriesFFTW* KTWignerVille::CrossMultiply(const KTTimeSeriesFFTW* data1, const KTTimeSeriesFFTW* data2)
    {
        KTTimeSeriesFFTW* product = new KTTimeSeriesFFTW(*data1);

        UInt_t size = data1->size();
        UInt_t iBin2 = size - 1;
        for (UInt_t iBin1 = 0; iBin1 < size; iBin1++)
        {
            data1(iBin1) = data1(iBin1) * data2(iBin2);
            iBin2--;
        }

        return product;
    }



    /*
    void KTComplexFFTW::ProcessHeader(const KTEggHeader* header)
    {
        SetSize(header->GetRecordSize());
        InitializeFFT();
        return;
    }
    */
    void KTComplexFFTW::ProcessTimeSeriesData(const KTTimeSeriesData* tsData)
    {
        KTFrequencySpectrumDataFFTW* newData = TransformData(tsData);
        if (tsData->GetEvent() != NULL)
            tsData->GetEvent()->AddData(newData);
        return;
    }

    void KTComplexFFTW::ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData)
    {
        KTTimeSeriesData* newData = TransformData(fsData);
        if (fsData->GetEvent() != NULL)
            fsData->GetEvent()->AddData(newData);
        return;
    }

    void KTWignerVille::ProcessEvent(KTEvent* event)
    {
        KTDEBUG(fftlog_comp, "Performing reverse FFT of event " << event->GetEventNumber());
        const KTTimeSeriesData* tsData = dynamic_cast< KTProgenitorTimeSeriesData* >(event->GetData(KTProgenitorTimeSeriesData::StaticGetName()));
        if (tsData == NULL)
        {
            tsData = dynamic_cast< KTBasicTimeSeriesData* >(event->GetData(KTBasicTimeSeriesData::StaticGetName()));
            if (tsData == NULL)
            {
                KTWARN(fftlog_comp, "No frequency spectrum data was available in the event");
                return;
            }
        }
        KTTimeSeriesData* newData = TransformData(tsData);
        event->AddData(newData);
        return;
    }


} /* namespace Katydid */
