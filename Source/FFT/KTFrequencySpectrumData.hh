/*
 * KTFrequencySpectrumData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATA_HH_
#define KTFREQUENCYSPECTRUMDATA_HH_

#include "KTWriteableData.hh"

#include "KTFFTTypes.hh"

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumData : public KTWriteableData
    {
        public:
            KTFrequencySpectrumData(unsigned nChannels=1);
            virtual ~KTFrequencySpectrumData();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTFrequencySpectrum* GetSpectrum(unsigned channelNum = 0) const;
            unsigned GetNChannels() const;

            void SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTPublisher* publisher);

        protected:
            static std::string fName;

            std::vector< KTFrequencySpectrum* > fSpectra;

    };

    inline const std::string& KTFrequencySpectrumData::GetName() const
    {
        return fName;
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumData::GetSpectrum(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTFrequencySpectrumData::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumData::SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumData::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
