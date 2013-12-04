/*
 * KTAmplitudeDistributor.cc
 *
 *  Created on: Apr 26, 2013
 *      Author: nsoblath
 */

#include "KTAmplitudeDistributor.hh"

#include "KTCorrelationData.hh"
#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTPStoreNode.hh"
#include "KTWignerVilleData.hh"

using std::string;
using std::vector;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(adlog, "katydid.analysis");

    static KTDerivedNORegistrar< KTProcessor, KTAmplitudeDistributor > sADRegistrar("amplitude-distributor");

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
            fDistributionData(shared_ptr< KTData >()),
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

    Bool_t KTAmplitudeDistributor::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        if (node->HasData("min-frequency"))
        {
            SetMinFrequency(node->GetData< Double_t >("min-frequency"));
        }
        if (node->HasData("max-frequency"))
        {
            SetMaxFrequency(node->GetData< Double_t >("max-frequency"));
        }

        if (node->HasData("min-bin"))
        {
            SetMinBin(node->GetData< UInt_t >("min-bin"));
        }
        if (node->HasData("max-bin"))
        {
            SetMaxBin(node->GetData< UInt_t >("max-bin"));
        }

        SetDistNBins(node->GetData< UInt_t >("dist-n-bins", fDistNBins));

        SetUseBuffer(node->GetData< Bool_t >("use-buffers", fUseBuffer));

        if (node->HasData("buffer-size"))
        {
            SetBufferSize(node->GetData< UInt_t >("buffer-size"));
        }

        if (node->HasData("dist-min"))
        {
            SetDistMin(node->GetData< Double_t >("dist-min"));
        }
        if (node->HasData("dist-max"))
        {
            SetDistMax(node->GetData< Double_t >("dist-max"));
        }

        return true;
    }

    Bool_t KTAmplitudeDistributor::Initialize(UInt_t nComponents, UInt_t nFreqBins)
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
            for (UInt_t iBuffer = 0; iBuffer < fBufferSize; iBuffer++)
            {
                fBuffer[iBuffer].resize(nComponents);
                for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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


    Bool_t KTAmplitudeDistributor::AddValues(KTFrequencySpectrumDataPolar& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::AddValues(KTFrequencySpectrumDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::AddValues(KTNormalizedFSDataPolar& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::AddValues(KTNormalizedFSDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::AddValues(KTCorrelationData& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::AddValues(KTWignerVilleData& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistributor::CoreAddValues(KTFrequencySpectrumDataFFTWCore& data)
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

        UInt_t nComponents = data.GetNComponents();
        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
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

    Bool_t KTAmplitudeDistributor::CoreAddValues(KTFrequencySpectrumDataPolarCore& data)
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

        UInt_t nComponents = data.GetNComponents();
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
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

    Bool_t KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumPolar* spectrum, UInt_t component)
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

        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNSlicesProcessed][component][iBin] = (*spectrum)(iBin).abs();
        }
        fNBuffered++;
        KTDEBUG(adlog, "Buffer now contains " << fNBuffered << " distributions; " << fNSlicesProcessed + 1 << " slices processed");
        return true;
    }

    Bool_t KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumPolar* spectrum, UInt_t component)
    {
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fDistributions->AddToDist(iBin, (*spectrum)(iBin).abs(), component);
        }
        return true;
    }

    Bool_t KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumFFTW* spectrum, UInt_t component)
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

        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNSlicesProcessed][component][iBin] = sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]);
        }
        fNBuffered++;
        KTDEBUG(adlog, "Buffer now contains " << fNBuffered << " distributions; " << fNSlicesProcessed + 1 << " slices processed");
        return true;
    }

    Bool_t KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumFFTW* spectrum, UInt_t component)
    {
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fDistributions->AddToDist(iBin, sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]), component);
        }
        return true;
    }


    Bool_t KTAmplitudeDistributor::CreateDistributionsFromBuffer()
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

        Double_t distMin, distMax, value;
        //UInt_t distBin;
        for (UInt_t iComponent = 0; iComponent < fNComponents; iComponent++)
        {
            for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
            {
                KTWARN(adlog, "0  " << iComponent << "  " << iBin << "  " << fBuffer[0][iComponent][iBin]);
                distMin = fBuffer[0][iComponent][iBin];
                distMax = distMin;
                KTERROR(adlog, distMin << "  " << distMax << "  buffer size: " << fBuffer.size());
                for (UInt_t iSpectrum = 1; iSpectrum < fBuffer.size(); iSpectrum++)
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
                for (UInt_t iSpectrum = 1; iSpectrum < fBuffer.size(); iSpectrum++)
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
