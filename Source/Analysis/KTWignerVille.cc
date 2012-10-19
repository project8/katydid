/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVille.hh"

#include "KTComplexFFTW.hh"
#include "KTLogger.hh"

#include <algorithm>

using std::copy;

namespace Katydid
{
    KTLOGGER(wvlog, "katydid.analysis");

    KTWignerVille::KTWignerVille() :
            KTProcessor(),
            fFFT(NULL),
            fSaveFrequencySpectrum(false)
    {
    }

    KTWignerVille::~KTWignerVille()
    {
        delete fFFT;
    }

    Bool_t KTWignerVille::Configure(const KTPStoreNode* node)
    {
        const KTPStoreNode* fftNode = node->GetChild("complex-fft");
        if (fftNode != NULL)
        {
            delete fFFT;
            fFFT = new KTComplexFFTW();
            if (! fFFT->Configure(fftNode)) return false;
        }

        return true;
    }

    KTTimeSeriesData* KTWignerVille::TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL)
    {
        if (fFFT == NULL)
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

        KTBasicTimeSeriesData* newTSData = new KTBasicTimeSeriesData(data->GetNChannels());
        KTFrequencySpectrumDataFFTW* newFSData = NULL;
        if (fSaveFrequencySpectrum)
        {
            newFSData = new KTFrequencySpectrumDataFFTW(data->GetNChannels());
            outputFSData = &newFSData;
        }

        for (UInt_t iChannel = 0; iChannel < data->GetNChannels(); iChannel++)
        {
            const KTTimeSeriesFFTW* nextInput = dynamic_cast< const KTTimeSeriesFFTW* >(data->GetRecord(iChannel));
            if (nextInput == NULL)
            {
                KTERROR(wvlog, "Incorrect time series type: time series did not cast to KTTimeSeriesFFTW.");
                delete newTSData;
                return NULL;
            }

            KTFrequencySpectrumFFTW* newFS = NULL;
            KTTimeSeriesFFTW* newTS = NULL;
            if (fSaveFrequencySpectrum)
            {
                newTS = Transform(nextInput, &newFS);
            }
            else
            {
                newTS = Transform(nextInput);
            }

            if (newFS != NULL)
                newFSData->SetSpectrum(newFS, iChannel);

            if (newTS == NULL)
            {
                KTERROR(wvlog, "Channel <" << iChannel << "> did not transform correctly.");
                delete newTSData;
                return NULL;
            }

            newTSData->SetRecord(newTS, iChannel);
        }

        KTDEBUG(fftlog_comp, "W-V transform complete; " << newTSData->GetNChannels() << " channel(s) transformed");

        return newTSData;
    }

    KTTimeSeriesFFTW* KTWignerVille::Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL)
    {
        if (fFFT == NULL)
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

        // Forward FFT
        KTFrequencySpectrumFFTW* freqSpec = fFFT->Transform(inputTS);
        if (freqSpec == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the forward FFT on the time series.");
            return NULL;
        }
        // copy the address of the frequency spectrum to outputFS
        outputFS = &freqSpec;

        // perform the actual W-V transform
        if (! Transform(freqSpec))
        {
            KTERROR(wvlog, "Something went wrong with the W-V transform of the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        // reverse FFT
        KTTimeSeriesFFTW* outputTS = fFFT->Transform(freqSpec);
        if (outputTS == NULL)
        {
            KTERROR(wvlog, "Something went wrong with the reverse FFT on the frequency spectrum.");
            if (outputFS == NULL) delete freqSpec;
            return NULL;
        }

        if (outputFS == NULL) delete freqSpec;
        return outputFS;
    }

    Bool_t KTWignerVille::Transform(KTFrequencySpectrumFFTW* freqSpectrum)
    {
        if (freqSpectrum == NULL)
        {
            KTERROR(wvlog, "Input frequency spectrum was NULL");
            return false;
        }

        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        fftw_complex* data = freqSpectrum->GetData();
        UInt_t arraySize = freqSpectrum.size();
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



} /* namespace Katydid */
