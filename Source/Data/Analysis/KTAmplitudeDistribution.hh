/*
 * KTAmplitudeDistribution
 *
 *  Created on: Apr 30, 2013
 *      Author: nsoblath
 */

#ifndef KTAMPLITUDEDISTRIBUTION_HH_
#define KTAMPLITUDEDISTRIBUTION_HH_

#include "KTData.hh"

#include "KTPhysicalArray.hh"

#include <vector>

namespace Katydid
{

    class KTAmplitudeDistribution : public KTExtensibleData< KTAmplitudeDistribution >
    {
        public:
            typedef KTPhysicalArray< 1, double > Distribution; // histogram over amplitude distribution index
            typedef std::vector< Distribution* > ComponentDistributions; // indexed over frequency-axis bins
            typedef std::vector< ComponentDistributions > Distributions; // indexed over component


        public:
            KTAmplitudeDistribution();
            virtual ~KTAmplitudeDistribution();

            const Distribution& GetDistribution(unsigned freqBins, unsigned component = 0) const;

            unsigned GetNComponents() const;
            unsigned GetNFreqBins() const;

            void SetDistValue(double value, unsigned freqBin, unsigned iDistBin, unsigned component = 0);

            void AddToDist(unsigned freqBin, unsigned iDistBin, unsigned component = 0, double weight = 1.);
            void AddToDist(unsigned freqBin, double distValue, unsigned component = 0, double weight = 1.);

            unsigned FindDistBin(double value, unsigned freqBin, unsigned component = 0);

            Bool_t ClearDistributions();

            /// Clear distributions and initialize new vectors with NULL distribution pointers
            Bool_t InitializeNull(unsigned nComponents, unsigned nFreqBins);
            /// Clear distributions and initialize new vectors with new, uniform, distributions pointers
            Bool_t InitializeNew(unsigned nComponents, unsigned nFreqBins, unsigned distNBins, double distMin, double distMax);
            /// Initialize a single new distribution (only removes the specified distribution, if it already exists)
            Bool_t InitializeADistribution(unsigned component, unsigned freqBin, unsigned distNBins, double distMin, double distMax);

        protected:
            Distributions fDistributions;

    };

    inline const KTAmplitudeDistribution::Distribution& KTAmplitudeDistribution::GetDistribution(unsigned freqBins, unsigned component) const
    {
        return *fDistributions[component][freqBins];
    }

    inline unsigned KTAmplitudeDistribution::GetNComponents() const
    {
        return unsigned(fDistributions.size());
    }

    inline unsigned KTAmplitudeDistribution::GetNFreqBins() const
    {
        return unsigned(fDistributions[0].size());
    }

    inline Bool_t KTAmplitudeDistribution::ClearDistributions()
    {
        for (unsigned iComponent = 0; iComponent < fDistributions.size(); iComponent++)
        {
            for (unsigned iFreqBin = 0; iFreqBin < fDistributions[iComponent].size(); iFreqBin++)
            {
                delete fDistributions[iComponent][iFreqBin];
            }
        }
        return true;
    }

    inline Bool_t KTAmplitudeDistribution::InitializeNull(unsigned nComponents, unsigned nFreqBins)
    {
        //fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, NULL));
        fDistributions.resize(nComponents);
        for (unsigned iComponent = 0; iComponent < nComponents; iComponent++)
        {
            fDistributions[iComponent].resize(nFreqBins, NULL);
        }
        return true;
    }

    inline Bool_t KTAmplitudeDistribution::InitializeNew(unsigned nComponents, unsigned nFreqBins, unsigned distNBins, double distMin, double distMax)
    {
        fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, new Distribution(distNBins, distMin, distMax)));
        return true;
    }

    inline unsigned KTAmplitudeDistribution::FindDistBin(double value, unsigned freqBin, unsigned component)
    {
        return fDistributions[component][freqBin]->FindBin(value);
    }


} /* namespace Katydid */

#endif /* KTAMPLITUDEDISTRIBUTION_HH_ */
