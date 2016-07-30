/*
 * KTAmplitudeDistributor.cc
 *
 *  Created on: Apr 26, 2013
 *      Author: nsoblath
 */

#include "KTAmplitudeDistributor.hh"

#include "KTCorrelationData.hh"
#include "KTEggHeader.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTNormalizedFSData.hh"
#include "KTWignerVilleData.hh"

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(adlog, "KTAmplitudeDistributor");

    KT_REGISTER_PROCESSOR(KTAmplitudeDistributor, "amplitude-distributor");

    KTAmplitudeDistributor::KTAmplitudeDistributor(const string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fDistNBins(100),
            fBufferSize(0),
            fDistMin(0.),
            fDistMax(1.),
            fUseBuffer(true),
            fTakeValuesPolar(&KTAmplitudeDistributor::TakeValuesToBuffer),
            fTakeValuesFFTW(&KTAmplitudeDistributor::TakeValuesToBuffer),
            fInvDistBinWidth(1.),
            fNFreqBins(1),
            fNComponents(1),
            fBuffer(),
            fNBuffered(0),
            fNSlicesProcessed(0),
            fDistributionData(KTDataPtr()),
            fDistributions(NULL),
            fAmpDistSignal("amp-dist", this),
            fFSPolarSlot("fs-polar", this, &KTAmplitudeDistributor::AddValues),
            fFSFFTWSlot("fs-fftw", this, &KTAmplitudeDistributor::AddValues),
            fNormFSPolarSlot("norm-fs-polar", this, &KTAmplitudeDistributor::AddValues),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTAmplitudeDistributor::AddValues),
            fCorrSlot("corr", this, &KTAmplitudeDistributor::AddValues),
            fWVSlot("wv", this, &KTAmplitudeDistributor::AddValues),
            fCompleteDistributions("finish", this, &KTAmplitudeDistributor::FinishAmpDist)
    {
    }

    KTAmplitudeDistributor::~KTAmplitudeDistributor()
    {
    }

    bool KTAmplitudeDistributor::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        if (node->has("min-bin"))
        {
            SetMinBin(node->get_value< unsigned >("min-bin"));
        }
        if (node->has("max-bin"))
        {
            SetMaxBin(node->get_value< unsigned >("max-bin"));
        }

        SetDistNBins(node->get_value< unsigned >("dist-n-bins", fDistNBins));

        SetUseBuffer(node->get_value< bool >("use-buffers", fUseBuffer));

        if (node->has("buffer-size"))
        {
            SetBufferSize(node->get_value< unsigned >("buffer-size"));
        }

        if (node->has("dist-min"))
        {
            SetDistMin(node->get_value< double >("dist-min"));
        }
        if (node->has("dist-max"))
        {
            SetDistMax(node->get_value< double >("dist-max"));
        }

        return true;
    }

    bool KTAmplitudeDistributor::Initialize(unsigned nComponents, unsigned nFreqBins)
    {
        fNComponents = nComponents;
        fNFreqBins = nFreqBins;

        fNSlicesProcessed = 0;

        fBuffer.clear();
        if (fUseBuffer)
        {
            // This command initializes the nested vectors with the correct number of elements
            // It's assumed that fBufferSize is set before this function is called.
            fBuffer.resize(fBufferSize);
            for (unsigned iBuffer = 0; iBuffer < fBufferSize; iBuffer++)
            {
                fBuffer[iBuffer].resize(nComponents);
                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    fBuffer[iBuffer][iComponent].resize(fNFreqBins);
                }
            }
            fNBuffered = 0;

            // Set the TakeValues function pointers
            fTakeValuesPolar = &KTAmplitudeDistributor::TakeValuesToBuffer;
            fTakeValuesFFTW = &KTAmplitudeDistributor::TakeValuesToBuffer;
            KTDEBUG(adlog, "Function pointers set to take values to buffer");
        }
        else
        {
            // Set the TakeValues function pointers
            fTakeValuesPolar = &KTAmplitudeDistributor::TakeValuesToDistributions;
            fTakeValuesFFTW = &KTAmplitudeDistributor::TakeValuesToDistributions;
            KTDEBUG(adlog, "Function pointers set to take values to distributions");
        }

        fDistributionData.reset(new KTData());

        fDistributions = &(fDistributionData->Of< KTAmplitudeDistribution >());
        if (! fUseBuffer)
        {
            // In the case that the buffer is being used, fDistributions will be initialized to the correct size later
            fDistributions->InitializeNew(fNComponents, fNFreqBins, fDistNBins, fDistMin, fDistMax);
        }

        return true;
    }


    bool KTAmplitudeDistributor::AddValues(KTFrequencySpectrumDataPolar& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::AddValues(KTFrequencySpectrumDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::AddValues(KTNormalizedFSDataPolar& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::AddValues(KTNormalizedFSDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::AddValues(KTCorrelationData& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::AddValues(KTWignerVilleData& data)
    {
        return CoreAddValues(data);
    }

    bool KTAmplitudeDistributor::CoreAddValues(KTFrequencySpectrumDataFFTWCore& data)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumFFTW(0)->FindBin(fMinFrequency));
            KTDEBUG(adlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumFFTW(0)->FindBin(fMaxFrequency));
            KTDEBUG(adlog, "Maximum bin set to " << fMaxBin);
        }

        if (! fDistributions)
        {
            if (! Initialize(data.GetNComponents(), data.GetSpectrumFFTW(0)->GetNBins()))
            {
                KTERROR(adlog, "Something went wrong while initializing the processor");
                return false;
            }
        }

        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(adlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }

            if (! (this->*fTakeValuesFFTW)(spectrum, iComponent))
            {
                KTERROR(adlog, "Something went wrong while taking values (FFTW)");
                return false;
            }
        }

        fNSlicesProcessed++;

        KTINFO(adlog, "Completed addition of values for " << nComponents << " components");

        return true;
    }

    bool KTAmplitudeDistributor::CoreAddValues(KTFrequencySpectrumDataPolarCore& data)
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrumPolar(0)->FindBin(fMinFrequency));
            KTDEBUG(adlog, "Minimum bin set to " << fMinBin << " (frequency: " << fMinFrequency << ")");
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrumPolar(0)->FindBin(fMaxFrequency));
            KTDEBUG(adlog, "Maximum bin set to " << fMaxBin << " (frequency: " << fMaxFrequency << ")");
        }

        if (! fDistributions)
        {
            if (! Initialize(data.GetNComponents(), data.GetSpectrumPolar(0)->GetNBins()))
            {
                KTERROR(adlog, "Something went wrong while initializing the processor");
                return false;
            }
        }

        unsigned nComponents = data.GetNComponents();
        for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(adlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }

            if (! (this->*fTakeValuesPolar)(spectrum, iComponent))
            {
                KTERROR(adlog, "Something went wrong while taking values (polar)");
                return false;
            }

        }

        fNSlicesProcessed++;

        KTINFO(adlog, "Completed addition of values for " << nComponents << " components");

        return true;
    }

    bool KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumPolar* spectrum, unsigned component)
    {
        if (fNSlicesProcessed == fBufferSize)
        {
            KTINFO(adlog, "Switching to direct-to-distribution setup");
            KTDEBUG(adlog, "Creating distributions from buffer");
            if (! CreateDistributionsFromBuffer())
            {
                KTERROR(adlog, "A problem occurred while creating distributions from the buffer");
                return false;
            }
            KTDEBUG(adlog, "Continuing with direct-to-distribution processing");
            fTakeValuesPolar = &KTAmplitudeDistributor::TakeValuesToDistributions;
            return TakeValuesToDistributions(spectrum, component);
        }

        for (unsigned iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNSlicesProcessed][component][iBin] = (*spectrum)(iBin).abs();
        }
        fNBuffered++;
        KTDEBUG(adlog, "Buffer now contains " << fNBuffered << " distributions; " << fNSlicesProcessed + 1 << " slices processed");
        return true;
    }

    bool KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumPolar* spectrum, unsigned component)
    {
        for (unsigned iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fDistributions->AddToDist(iBin, (*spectrum)(iBin).abs(), component);
        }
        return true;
    }

    bool KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumFFTW* spectrum, unsigned component)
    {
        if (fNSlicesProcessed == fBufferSize)
        {
            KTINFO(adlog, "Switching to direct-to-distribution setup");
            KTDEBUG(adlog, "Creating distributions from buffer");
            if (! CreateDistributionsFromBuffer())
            {
                KTERROR(adlog, "A problem occurred while creating distributions from the buffer");
                return false;
            }
            KTDEBUG(adlog, "Continuing with direct-to-distribution processing");
            fTakeValuesPolar = &KTAmplitudeDistributor::TakeValuesToDistributions;
            return TakeValuesToDistributions(spectrum, component);
        }

        for (unsigned iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNSlicesProcessed][component][iBin] = sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]);
        }
        fNBuffered++;
        KTDEBUG(adlog, "Buffer now contains " << fNBuffered << " distributions; " << fNSlicesProcessed + 1 << " slices processed");
        return true;
    }

    bool KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumFFTW* spectrum, unsigned component)
    {
        for (unsigned iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fDistributions->AddToDist(iBin, sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]), component);
        }
        return true;
    }


    bool KTAmplitudeDistributor::CreateDistributionsFromBuffer()
    {
        if (fBuffer.size() == 0)
        {
            KTERROR(adlog, "Buffer is empty, but buffer use was requested");
            return false;
        }

        KTDEBUG(adlog, "Initializing NULL distributions: " << fNComponents << " components with " << fNFreqBins << " frequency bins");
        fDistributions->InitializeNull(fNComponents, fNFreqBins);
        if (fDistributions->GetNComponents() != fNComponents)
        {
            KTERROR(adlog, "The distributions data doesn't have the correct number of components: " << fDistributions->GetNComponents());
            return false;
        }
        if (fDistributions->GetNFreqBins() != fNFreqBins)
        {
            KTERROR(adlog, "The distributions data doesn't have the correct number of frequency bins: " << fDistributions->GetNFreqBins());
            return false;
        }

        if (fBuffer.size() > fNSlicesProcessed)
        {
            KTDEBUG(adlog, "Buffer is larger than the number of slices processed; resizing to " << fNSlicesProcessed);
            fBuffer.resize(fNSlicesProcessed);
        }

        double distMin, distMax, value;
        //unsigned distBin;
        for (unsigned iComponent = 0; iComponent < fNComponents; ++iComponent)
        {
            for (unsigned iBin = fMinBin; iBin <= fMaxBin; iBin++)
            {
                KTWARN(adlog, "0  " << iComponent << "  " << iBin << "  " << fBuffer[0][iComponent][iBin]);
                distMin = fBuffer[0][iComponent][iBin];
                distMax = distMin;
                KTERROR(adlog, distMin << "  " << distMax << "  buffer size: " << fBuffer.size());
                for (unsigned iSpectrum = 1; iSpectrum < fBuffer.size(); iSpectrum++)
                {
                    value = fBuffer[iSpectrum][iComponent][iBin];
                    if (value < distMin) distMin = value;
                    else if (value > distMax) distMax = value;
                    KTERROR(adlog, "   " << distMin << "  " << distMax);
                }
                if (! fDistributions->InitializeADistribution(iComponent, iBin, fDistNBins, distMin, distMax))
                {
                    KTERROR(adlog, "There was a problem initializing a distribution: component " << iComponent << "; frequency bin " << iBin);
                    return false;
                }
                KTDEBUG(adlog, "Distribution initialized; filling in from buffer");
                for (unsigned iSpectrum = 1; iSpectrum < fBuffer.size(); iSpectrum++)
                {
                    value = fBuffer[iSpectrum][iComponent][iBin];
                    fDistributions->AddToDist(iBin, value, iComponent);
                }
            }
        }

        // The condition (fUseBuffer && fBuffer.empty()) implies that if data has been taken, it's been transferred to distributions
        fBuffer.clear();

        return true;
    }


    void KTAmplitudeDistributor::FinishAmpDist()
    {
        if (fUseBuffer && ! fBuffer.empty())
        {
            CreateDistributionsFromBuffer();
        }

        fAmpDistSignal(fDistributionData);

        return;
    }


} /* namespace Katydid */
