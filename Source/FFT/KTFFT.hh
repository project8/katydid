/*
 * KTFFT.hh
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#ifndef KTFFT_HH_
#define KTFFT_HH_

#include "Rtypes.h"

namespace Katydid
{
    class KTTimeSeriesData;

    class KTFFT
    {
        public:
            KTFFT();
            virtual ~KTFFT();

            virtual UInt_t GetTimeSize() const = 0;
            virtual UInt_t GetFrequencySize() const = 0;

            /// Returns the time bin width that corresponds to a frequency bin width (and frequency size).
            Double_t GetTimeBinWidth(Double_t freqBinWidth) const;
            Double_t GetMinTime() const;
            Double_t GetMaxTime(Double_t freqBinWidth) const;

            /// Returns the frequency bin width that corresponds to a time bin width (and time size).
            /// To calculate using sample rate, use the fact that time-bin-width = (sample-rate)^-1
            Double_t GetFrequencyBinWidth(Double_t timeBinWidth) const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const = 0;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const = 0;

    };


    inline Double_t KTFFT::GetTimeBinWidth(Double_t freqBinWidth) const
    {
        return 1. / (freqBinWidth * GetTimeSize());
    }

    inline Double_t KTFFT::GetMinTime() const
    {
        return 0.;
    }

    inline Double_t KTFFT::GetMaxTime(Double_t freqBinWidth) const
    {
        return 1 / freqBinWidth;
    }

    inline Double_t KTFFT::GetFrequencyBinWidth(Double_t timeBinWidth) const
    {
        return 1. / (timeBinWidth * GetTimeSize());
    }

} /* namespace Katydid */
#endif /* KTFFT_HH_ */
