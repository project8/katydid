/*
 * KTFFT.hh
 *
 *  Created on: Sep 16, 2011
 *      Author: nsoblath
 */

#ifndef KTFFT_HH_
#define KTFFT_HH_

namespace Katydid
{
    class KTTimeSeriesData;

    class KTFFT
    {
        public:
            KTFFT();
            virtual ~KTFFT();

            virtual unsigned GetTimeSize() const = 0;
            virtual unsigned GetFrequencySize() const = 0;

            /// Returns the time bin width that corresponds to a frequency bin width (and frequency size).
            double GetTimeBinWidth(double freqBinWidth) const;
            double GetMinTime() const;
            double GetMaxTime(double freqBinWidth) const;

            /// Returns the frequency bin width that corresponds to a time bin width (and time size).
            /// To calculate using sample rate, use the fact that time-bin-width = (sample-rate)^-1
            double GetFrequencyBinWidth(double timeBinWidth) const;
            virtual double GetMinFrequency(double timeBinWidth) const = 0;
            virtual double GetMaxFrequency(double timeBinWidth) const = 0;

    };


    inline double KTFFT::GetTimeBinWidth(double freqBinWidth) const
    {
        return 1. / (freqBinWidth * GetTimeSize());
    }

    inline double KTFFT::GetMinTime() const
    {
        return 0.;
    }

    inline double KTFFT::GetMaxTime(double freqBinWidth) const
    {
        return 1 / freqBinWidth;
    }

    inline double KTFFT::GetFrequencyBinWidth(double timeBinWidth) const
    {
        return 1. / (timeBinWidth * GetTimeSize());
    }



    class KTFFTW : public KTFFT
    {
        public:
            KTFFTW();
            virtual ~KTFFTW();

            void InitializeMultithreaded();

            static unsigned sInstanceCount;
            static bool sMultithreadedIsInitialized;
    };



} /* namespace Katydid */
#endif /* KTFFT_HH_ */
