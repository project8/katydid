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
    KTLOGGER(datalog, "KTAmplitudeDistribution");

    const std::string KTAmplitudeDistribution::sName("amplitude-distribution");

    KTAmplitudeDistribution::KTAmplitudeDistribution() :
            KTExtensibleData< KTAmplitudeDistribution >(),
            fDistributions(1, ComponentDistributions(1, NULL))
    {
    }

    KTAmplitudeDistribution::~KTAmplitudeDistribution()
    {
        ClearDistributions();
    }

    void KTAmplitudeDistribution::SetDistValue(double value, unsigned freqBin, unsigned iDistBin, unsigned component)
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

    void KTAmplitudeDistribution::AddToDist(unsigned freqBin, unsigned iDistBin, unsigned component, double weight)
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
        if (iDistBin > fDistributions[component][freqBin]->size())
        {
            KTERROR(datalog, "Data does not contain dist bin " << iDistBin);
            return;
        }
#endif
        (*fDistributions[component][freqBin])(iDistBin) = (*fDistributions[component][freqBin])(iDistBin) + weight;
        return;
    }

    void KTAmplitudeDistribution::AddToDist(unsigned freqBin, double distValue, unsigned component, double weight)
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
        AddToDist(freqBin, unsigned(fDistributions[component][freqBin]->FindBin(distValue)), component, weight);
        return;
    }

    bool KTAmplitudeDistribution::InitializeADistribution(unsigned component, unsigned freqBin, unsigned distNBins, double distMin, double distMax)
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
        KTDEBUG(datalog, "Initializing distribution for component " << component << ", frequency bin " << freqBin << ", (" << distNBins << ", " << distMin << ", " << distMax << ")");
        delete fDistributions[component][freqBin];
        fDistributions[component][freqBin] = new Distribution(distNBins, distMin, distMax);
        return true;
    }


} /* namespace Katydid */

