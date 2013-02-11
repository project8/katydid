/*
 * KTFrequencySpectrumData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATA_HH_
#define KTFREQUENCYSPECTRUMDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumData : public KTWriteableData
    {
        public:
            KTFrequencySpectrumData();
            virtual ~KTFrequencySpectrumData();

            const KTFrequencySpectrumPolar* GetSpectrum(UInt_t channelNum = 0) const = 0;
            KTFrequencySpectrumPolar* GetSpectrum(UInt_t channelNum = 0) = 0;
            UInt_t GetNChannels() const = 0;
            Double_t GetTimeInRun() const = 0;
            Double_t GetTimeLength() const = 0;
            ULong64_t GetSliceNumber() const = 0;

            void SetNChannels(UInt_t channels) = 0;
            void SetTimeInRun(Double_t tir) = 0;
            void SetTimeLength(Double_t length) = 0;
            void SetSliceNumber(ULong64_t slice) = 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const = 0;
            virtual TH1D* CreatePhaseHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const = 0;

            virtual TH1D* CreatePowerHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const = 0;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const = 0;
#endif
    };

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
