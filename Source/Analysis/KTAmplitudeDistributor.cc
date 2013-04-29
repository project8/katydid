/*
 * KTAmplitudeDistributor.cc
 *
 *  Created on: Apr 26, 2013
 *      Author: nsoblath
 */

#include "KTAmplitudeDistributor.hh"

#include "KTCorrelationData.hh"
#include "KTEggHeader.hh"
#include "KTFactory.hh"
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

    static KTDerivedRegistrar< KTProcessor, KTAmplitudeDistributor > sADRegistrar("amplitude-distributor");

    KTAmplitudeDistributor::KTAmplitudeDistributor(const string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fDistNBins(100),
            fUseBuffer(true),
            fBufferSize(0),
            fDistMin(0.),
            fDistMax(1.),
            fTakeValuesPolar(&KTAmplitudeDistributor::TakeValuesToBuffer),
            fTakeValuesFFTW(&KTAmplitudeDistributor::TakeValuesToBuffer),
            fInvDistBinWidth(1.),
            fNFreqBins(1),
            fNComponents(1),
            fBuffer(),
            fNBuffered(0),
            fDistributions(),
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

        fBuffer.clear();
        if (fUseBuffer)
        {
            // This command initializes the nested vectors with the correct number of elements
            // It's assumed that fBufferSize is set before this function is called.
            fBuffer.resize(fBufferSize, Spectra(nComponents, Spectrum(fNFreqBins)));
        }
        else
        {
            if (CreateDistributionsEmpty())
                return false;
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

        if (fDistributions.empty())
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

            if (spectrum->size() != fDistributions[iComponent].size())
            {
                KTERROR(adlog, "Received spectrum with a different size, " << spectrum->size() << ", than expected, " << fDistributions.size());
                return false;
            }

            fTakeValuesFFTW(spectrum, iComponent);
        }

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

        if (fDistributions.empty())
        {
            if (! Initialize(data.GetNComponents(), data.GetSpectrumPolar(0)->GetNBins()))
            {
                KTERROR(adlog, "Something went wrong while initializing the processor");
                return false;
            }
        }

        UInt_t nComponents = data.GetNComponents();
        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(adlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }

            if (spectrum->size() != fDistributions[iComponent].size())
            {
                KTERROR(adlog, "Received spectrum with a different size, " << spectrum->size() << ", than expected, " << fDistributions[iComponent].size());
                return false;
            }

            fTakeValuesPolar(spectrum, iComponent);
        }

        KTINFO(adlog, "Completed addition of values for " << nComponents << " components");

        return true;
    }

    void KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumPolar* spectrum, UInt_t component)
    {
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNBuffered][component][iBin] = (*spectrum)(iBin).abs();
        }
        fNBuffered++;
        if (fNBuffered == fBufferSize)
        {
            CreateDistributionsFromBuffer();
            fTakeValuesPolar = &KTAmplitudeDistributor::TakeValuesToDistributions;
        }
        return;
    }

    void KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumPolar* spectrum, UInt_t component)
    {
        UInt_t distBin = 0;
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            distBin = CalculateBin((*spectrum)(iBin).abs());
            fDistributions[component][distBin] = fDistributions[component][distBin] + 1;
        }
        return;
    }

    void KTAmplitudeDistributor::TakeValuesToBuffer(const KTFrequencySpectrumFFTW* spectrum, UInt_t component)
    {
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            fBuffer[fNBuffered][component][iBin] = sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]);
        }
        fNBuffered++;
        if (fNBuffered == fBufferSize)
        {
            CreateDistributionsFromBuffer();
            fTakeValuesFFTW = &KTAmplitudeDistributor::TakeValuesToDistributions;
        }
        return;
    }

    void KTAmplitudeDistributor::TakeValuesToDistributions(const KTFrequencySpectrumFFTW* spectrum, UInt_t component)
    {
        UInt_t distBin = 0;
        for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
        {
            distBin = CalculateBin(sqrt((*spectrum)(iBin)[0]*(*spectrum)(iBin)[0] + (*spectrum)(iBin)[1]*(*spectrum)(iBin)[1]));
            fDistributions[component][distBin] = fDistributions[component][distBin] + 1;
        }
        return;
    }


    Bool_t KTAmplitudeDistributor::CreateDistributionsEmpty()
    {
        fDistributions.clear();

        fDistributions.resize(fNComponents, ComponentDistributions(fNFreqBins, new Distribution(fDistNBins, fDistMin, fDistMax)));

        return true;
    }

    Bool_t KTAmplitudeDistributor::CreateDistributionsFromBuffer()
    {
        fDistributions.clear();

        if (fBuffer.size() == 0)
        {
            KTERROR(adlog, "Buffer is empty, but buffer use was requested");
            return false;
        }

        fDistributions.resize(fNComponents, ComponentDistributions(fNFreqBins, NULL));

        Double_t distMin, distMax, value;
        for (UInt_t iComponent = 0; iComponent < fNComponents; iComponent++)
        {
            for (UInt_t iBin = fMinBin; iBin <= fMaxBin; iBin++)
            {
                distMin = fBuffer[0][iComponent][iBin];
                distMax = distMin;
                for (UInt_t iSpectrum = 1; iSpectrum < fBuffer.size(); iSpectrum++)
                {
                    value = fBuffer[iSpectrum][iComponent][iBin];
                    if (value < distMin) distMin = value;
                    else if (value > distMax) distMax = value;
                }
                fDistributions[iComponent][iBin] = new Distribution(fDistNBins, distMin, distMax);
            }
        }

        // The condition (fUserBuffer && fBuffer.empty()) implies that if data has been taken, it's been transferred to distributions
        fBuffer.clear();

        return true;
    }


    void KTAmplitudeDistributor::FinishAmpDist()
    {
        if (fUseBuffer && ! fBuffer.empty())
        {
            CreateDistributionsFromBuffer();
        }

        // now emit the signal for the data

        return;
    }


} /* namespace Katydid */
