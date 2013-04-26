/*
 * KTAmplitudeDistribution.cc
 *
 *  Created on: Apr 26, 2013
 *      Author: nsoblath
 */

#include "KTAmplitudeDistribution.hh"

#include "KTCorrelationData.hh"
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

    static KTDerivedRegistrar< KTProcessor, KTAmplitudeDistribution > sADRegistrar("amplitude-distribution");

    KTAmplitudeDistribution::KTAmplitudeDistribution(const string& name) :
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
            fAmpDistSignal("disc-1d", this),
            fFSPolarSlot("fs-polar", this, &KTAmplitudeDistribution::AddValues),
            fFSFFTWSlot("fs-fftw", this, &KTAmplitudeDistribution::AddValues),
            fNormFSPolarSlot("norm-fs-polar", this, &KTAmplitudeDistribution::AddValues),
            fNormFSFFTWSlot("norm-fs-fftw", this, &KTAmplitudeDistribution::AddValues),
            fCorrSlot("corr", this, &KTAmplitudeDistribution::AddValues),
            fWVSlot("wv", this, &KTAmplitudeDistribution::AddValues),
            fAmpDistSignal("amp-dist", this, &KTAmplitudeDistribution::FinishAmpDist)
    {
    }

    KTAmplitudeDistribution::~KTAmplitudeDistribution()
    {
    }

    Bool_t KTAmplitudeDistribution::Configure(const KTPStoreNode* node)
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

        SetDistNBins(node->GetData< UInt_t >("dist-n-bins"));

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

    Bool_t KTAmplitudeDistribution::AddValues(KTFrequencySpectrumDataPolar& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::AddValues(KTFrequencySpectrumDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::AddValues(KTNormalizedFSDataPolar& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::AddValues(KTNormalizedFSDataFFTW& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::AddValues(KTCorrelationData& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::AddValues(KTWignerVilleData& data)
    {
        return CoreAddValues(data);
    }

    Bool_t KTAmplitudeDistribution::CoreAddValues(KTFrequencySpectrumDataFFTWCore& data)
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

        UInt_t nComponents = data.GetNComponents();



        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumFFTW* spectrum = data.GetSpectrumFFTW(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(adlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }
            if (spectrum->size() != fDistributions.size())
            {
                KTERROR(adlog, "Received spectrum with a different size, " << spectrum->size() << ", than expected, " << fDistributions.size());
                return false;
            }




        }

        KTINFO(adlog, "Completed addition of values for " << nComponents << " components");

        return true;
    }

    Bool_t KTAmplitudeDistribution::CoreAddValues(KTFrequencySpectrumDataPolarCore& data)
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

        UInt_t nComponents = data.GetNComponents();


        for (UInt_t iComponent=0; iComponent<nComponents; iComponent++)
        {
            const KTFrequencySpectrumPolar* spectrum = data.GetSpectrumPolar(iComponent);
            if (spectrum == NULL)
            {
                KTERROR(adlog, "Frequency spectrum pointer (component " << iComponent << ") is NULL!");
                return false;
            }
            if (spectrum->size() != fDistributions.size())
            {
                KTERROR(adlog, "Received spectrum with a different size, " << spectrum->size() << ", than expected, " << fDistributions.size());
                return false;
            }




        }

        KTINFO(adlog, "Completed addition of values for " << nComponents << " components");

        return true;
    }


    void KTAmplitudeDistribution::FinishAmpDist()
    {

    }


} /* namespace Katydid */
