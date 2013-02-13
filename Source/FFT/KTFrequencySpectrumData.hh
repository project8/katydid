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

            virtual const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const = 0;
            virtual KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) = 0;
            virtual UInt_t GetNComponents() const = 0;
            virtual Double_t GetTimeInRun() const = 0;
            virtual Double_t GetTimeLength() const = 0;
            virtual ULong64_t GetSliceNumber() const = 0;

            virtual void SetNComponents(UInt_t channels) = 0;
            virtual void SetTimeInRun(Double_t tir) = 0;
            virtual void SetTimeLength(Double_t length) = 0;
            virtual void SetSliceNumber(ULong64_t slice) = 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumMag") const = 0;
            virtual TH1D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPhase") const = 0;

            virtual TH1D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPower") const = 0;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const = 0;
#endif
    };

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
