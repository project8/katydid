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

            /// Returns the frequency bin width that corresponds to a time bin width (and time size).
            /// To calculate using sample rate, use the fact that time-bin-width = (sample-rate)^-1
            virtual Double_t GetFrequencyBinWidth(Double_t timeBinWidth) const;
            virtual Double_t GetMinFrequency(Double_t timeBinWidth) const;
            virtual Double_t GetMaxFrequency(Double_t timeBinWidth) const;

    };


    inline Double_t KTFFT::GetFrequencyBinWidth(Double_t timeBinWidth) const
    {
        return 1. / (timeBinWidth / GetTimeSize());
    }

    inline Double_t KTFFT::GetMinFrequency(Double_t timeBinWidth) const
    {
        return -0.5 * GetFrequencyBinWidth(timeBinWidth);
    }

    inline Double_t KTFFT::GetMaxFrequency(Double_t timeBinWidth) const
    {
        return GetFrequencyBinWidth(timeBinWidth) * ((Double_t)GetFrequencySize() - 0.5);
    }

} /* namespace Katydid */
#endif /* KTFFT_HH_ */
