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

            const Distribution& GetDistribution(UInt_t freqBins, UInt_t component = 0) const;

            UInt_t GetNComponents() const;
            UInt_t GetNFreqBins() const;

            void SetDistValue(double value, UInt_t freqBin, UInt_t iDistBin, UInt_t component = 0);

            void AddToDist(UInt_t freqBin, UInt_t iDistBin, UInt_t component = 0, double weight = 1.);
            void AddToDist(UInt_t freqBin, double distValue, UInt_t component = 0, double weight = 1.);

            UInt_t FindDistBin(double value, UInt_t freqBin, UInt_t component = 0);

            Bool_t ClearDistributions();

            /// Clear distributions and initialize new vectors with NULL distribution pointers
            Bool_t InitializeNull(UInt_t nComponents, UInt_t nFreqBins);
            /// Clear distributions and initialize new vectors with new, uniform, distributions pointers
            Bool_t InitializeNew(UInt_t nComponents, UInt_t nFreqBins, UInt_t distNBins, double distMin, double distMax);
            /// Initialize a single new distribution (only removes the specified distribution, if it already exists)
            Bool_t InitializeADistribution(UInt_t component, UInt_t freqBin, UInt_t distNBins, double distMin, double distMax);

        protected:
            Distributions fDistributions;

    };

    inline const KTAmplitudeDistribution::Distribution& KTAmplitudeDistribution::GetDistribution(UInt_t freqBins, UInt_t component) const
    {
        return *fDistributions[component][freqBins];
    }

    inline UInt_t KTAmplitudeDistribution::GetNComponents() const
    {
        return UInt_t(fDistributions.size());
    }

    inline UInt_t KTAmplitudeDistribution::GetNFreqBins() const
    {
        return UInt_t(fDistributions[0].size());
    }

    inline Bool_t KTAmplitudeDistribution::ClearDistributions()
    {
        for (UInt_t iComponent = 0; iComponent < fDistributions.size(); iComponent++)
        {
            for (UInt_t iFreqBin = 0; iFreqBin < fDistributions[iComponent].size(); iFreqBin++)
            {
                delete fDistributions[iComponent][iFreqBin];
            }
        }
        return true;
    }

    inline Bool_t KTAmplitudeDistribution::InitializeNull(UInt_t nComponents, UInt_t nFreqBins)
    {
        //fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, NULL));
        fDistributions.resize(nComponents);
        for (UInt_t iComponent = 0; iComponent < nComponents; iComponent++)
        {
            fDistributions[iComponent].resize(nFreqBins, NULL);
        }
        return true;
    }

    inline Bool_t KTAmplitudeDistribution::InitializeNew(UInt_t nComponents, UInt_t nFreqBins, UInt_t distNBins, double distMin, double distMax)
    {
        fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, new Distribution(distNBins, distMin, distMax)));
        return true;
    }

    inline UInt_t KTAmplitudeDistribution::FindDistBin(double value, UInt_t freqBin, UInt_t component)
    {
        return fDistributions[component][freqBin]->FindBin(value);
    }


} /* namespace Katydid */

#endif /* KTAMPLITUDEDISTRIBUTION_HH_ */
