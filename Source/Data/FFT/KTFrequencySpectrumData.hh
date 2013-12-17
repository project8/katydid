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
    template< class XDataType >
    class KTFrequencySpectrumData : public KTExtensibleData< XDataType >
    {
        public:
            KTFrequencySpectrumData();
            virtual ~KTFrequencySpectrumData();

            virtual unsigned GetNComponents() const = 0;

            virtual const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const = 0;
            virtual KTFrequencySpectrum* GetSpectrum(unsigned component = 0) = 0;

            virtual void SetNComponents(unsigned channels) = 0;
/*
#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumMag") const = 0;
            virtual TH1D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPhase") const = 0;

            virtual TH1D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPower") const = 0;

            virtual TH1D* CreatePowerDistributionHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const = 0;
#endif
*/
    };

    template< class XDataType >
    KTFrequencySpectrumData< XDataType >::KTFrequencySpectrumData() :
            KTExtensibleData< XDataType >()
    {
    }

    template< class XDataType >
    KTFrequencySpectrumData< XDataType >::~KTFrequencySpectrumData()
    {
    }


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
