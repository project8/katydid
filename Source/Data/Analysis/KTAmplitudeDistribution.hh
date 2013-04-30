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
            typedef KTPhysicalArray< 1, Double_t > Distribution; // histogram over amplitude distribution index
            typedef std::vector< Distribution* > ComponentDistributions; // indexed over frequency-axis bins
            typedef std::vector< ComponentDistributions > Distributions; // indexed over component


        public:
            KTAmplitudeDistribution();
            virtual ~KTAmplitudeDistribution();

            const Distribution& GetDistribution(UInt_t freqBins, UInt_t component = 0) const;

            UInt_t GetNComponents() const;
            UInt_t GetNFreqBins() const;

            void SetDistValue(Double_t value, UInt_t freqBin, UInt_t iDistBin, UInt_t component = 0);

            void AddToDist(UInt_t freqBin, UInt_t iDistBin, UInt_t component = 0, Double_t weight = 1.);
            void AddToDist(UInt_t freqBin, Double_t distValue, UInt_t component = 0, Double_t weight = 1.);

            UInt_t FindDistBin(Double_t value, UInt_t freqBin, UInt_t component = 0);

            Bool_t ClearDistributions();

            /// Clear distributions and initialize new vectors with NULL distribution pointers
            Bool_t InitializeNull(UInt_t nComponents, UInt_t nFreqBins);
            /// Clear distributions and initalize new vectors with new, uniform, distributions pointers
            Bool_t InitializeNew(UInt_t nComponents, UInt_t nFreqBins, UInt_t distNBins, UInt_t distMin, UInt_t distMax);
            /// Initialize a single new distribution (only removes the specified distribution, if it already exists)
            Bool_t InitializeADistribution(UInt_t component, UInt_t freqBin, UInt_t distNBins, UInt_t distMin, UInt_t distMax);

        protected:
            Distributions fDistributions;

    };

    const KTAmplitudeDistribution::Distribution& KTAmplitudeDistribution::GetDistribution(UInt_t freqBins, UInt_t component) const
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
        fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, NULL));
        return true;
    }

    inline Bool_t KTAmplitudeDistribution::InitializeNew(UInt_t nComponents, UInt_t nFreqBins, UInt_t distNBins, UInt_t distMin, UInt_t distMax)
    {
        fDistributions.resize(nComponents, ComponentDistributions(nFreqBins, new Distribution(distNBins, distMin, distMax)));
        return true;
    }

    inline UInt_t KTAmplitudeDistribution::FindDistBin(Double_t value, UInt_t freqBin, UInt_t component)
    {
        return fDistributions[component][freqBin]->FindBin(value);
    }


} /* namespace Katydid */

#endif /* KTAMPLITUDEDISTRIBUTION_HH_ */
