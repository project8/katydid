/*
 * KTAmplitudeDistribution.cc
 *
 *  Created on: Apr 30, 2013
 *      Author: nsoblath
 */

#include "KTAmplitudeDistribution.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(datalog, "katydid.data");

    KTAmplitudeDistribution::KTAmplitudeDistribution() :
            KTExtensibleData< KTAmplitudeDistribution >(),
            fDistributions(1, ComponentDistributions(1, NULL))
    {
    }

    KTAmplitudeDistribution::~KTAmplitudeDistribution()
    {
    }

    void KTAmplitudeDistribution::AddValue(UInt_t freqBin, UInt_t iDistBin, Double_t value, UInt_t component)
    {
#ifdef Katydid_DEBUG
        if (component > fDistributions.size())
        {
            KTERROR(datalog, "Data does not contain component " << component);
            return;
        }
        if (freqBin > fDistributions[component].size() || fDistributions[component][freqBin] == NULL)
        {
            KTERROR(datalog, "Data does not contain frequency bin " << freqBin << " for component " << component << ", or it hasn't been initialized");
            return;
        }
#endif
        (*fDistributions[component][freqBin])(iDistBin) = value;
        return;
    }

    void KTAmplitudeDistribution::IncrementValue(UInt_t freqBin, UInt_t iDistBin, UInt_t component, Double_t increment)
    {
#ifdef Katydid_DEBUG
        if (component > fDistributions.size())
        {
            KTERROR(datalog, "Data does not contain component " << component);
            return;
        }
        if (freqBin > fDistributions[component].size() || fDistributions[component][freqBin] == NULL)
        {
            KTERROR(datalog, "Data does not contain frequency bin " << freqBin << " for component " << component << ", or it hasn't been initialized");
            return;
        }
#endif
        (*fDistributions[component][freqBin])(iDistBin) = (*fDistributions[component][freqBin])(iDistBin) + increment;
        return;
    }

    Bool_t KTAmplitudeDistribution::InitializeADistribution(UInt_t component, UInt_t freqBin, UInt_t distNBins, UInt_t distMin, UInt_t distMax)
    {
        if (component > fDistributions.size())
        {
            KTERROR(datalog, "Data does not contain component " << component);
            return false;
        }
        if (freqBin > fDistributions[component].size())
        {
            KTERROR(datalog, "Data does not contain frequency bin " << freqBin << " for component " << component);
            return false;
        }
        fDistributions[component][freqBin] = new Distribution(distNBins, distMin, distMax);
        return true;
    }


} /* namespace Katydid */

