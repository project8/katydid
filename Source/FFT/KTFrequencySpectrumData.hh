/*
 * KTFrequencySpectrumData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATA_HH_
#define KTFREQUENCYSPECTRUMDATA_HH_

#include "KTData.hh"

#include "complexpolar.hh"
#include "KTPhysicalArray.hh"

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumData : public KTData
    {
        public:
            typedef KTPhysicalArray< 1, complexpolar< Double_t > > FrequencySpectrum;

        public:
            KTFrequencySpectrumData();
            virtual ~KTFrequencySpectrumData();

            const FrequencySpectrum& GetSpectrum(unsigned channelNum = 0) const;

            void SetSpectrum(FrequencySpectrum* record, unsigned channelNum = 0);

        protected:
            std::vector< FrequencySpectrum* > fSpectra;

    };

    inline const KTFrequencySpectrumData::FrequencySpectrum& KTFrequencySpectrumData::GetSpectrum(unsigned channelNum) const
    {
        return (*fSpectra[channelNum]);
    }

    inline void KTFrequencySpectrumData::SetSpectrum(FrequencySpectrum* record, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }


} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
