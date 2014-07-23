/*
 * KTProcessedTrackData.hh
 *
 *  Created on:July 22, 2014
 *      Author: nsoblath
 */

#ifndef KTPROCESSEDTRACKDATA_HH_
#define KTPROCESSEDTRACKDATA_HH_

#include "KTData.hh"

namespace Katydid
{
    class KTProcessedTrackData : public KTExtensibleData< KTProcessedTrackData >
    {
        public:
            KTProcessedTrackData();
            virtual ~KTProcessedTrackData();

            unsigned GetComponent() const;

            double GetTimeInRun() const;
            double GetTimeLength() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            double GetFrequencyWidth() const;
            double GetSlope() const;
            double GetIntercept() const;

            double GetTimeInRunSigma() const;
            double GetTimeLengthSigma() const;
            double GetMinimumFrequencySigma() const;
            double GetMaximumFrequencySigma() const;
            double GetFrequencyWidthSigma() const;
            double GetSlopeSigma() const;
            double GetInterceptSigma() const;

            void SetComponent(unsigned component);

            void SetTimeInRun(double tir);
            void SetTimeLength(double length);
            void SetMinimumFrequency(double freq);
            void SetMaximumFrequency(double freq);
            void SetFrequencyWidth(double width);
            void SetSlope(double slope);
            void SetIntercept(double intercept);

            void SetTimeInRunSigma(double sigma);
            void SetTimeLengthSigma(double sigma);
            void SetMinimumFrequencySigma(double sigma);
            void SetMaximumFrequencySigma(double sigma);
            void SetFrequencyWidthSigma(double sigma);
            void SetSlopeSigma(double sigma);
            void SetInterceptSigma(double sigma);

        protected:
            unsigned fComponent;

            double fTimeInRun;
            double fTimeLength;
            double fMinFrequency;
            double fMaxFrequency;
            double fFrequencyWidth;
            double fSlope;
            double fIntercept;

            double fTimeInRunSigma;
            double fTimeLengthSigma;
            double fMinFrequencySigma;
            double fMaxFrequencySigma;
            double fFrequencyWidthSigma;
            double fSlopeSigma;
            double fInterceptSigma;
    };

    inline unsigned KTProcessedTrackData::GetComponent() const
    {
        return fComponent;
    }

    inline double KTProcessedTrackData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline double KTProcessedTrackData::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline double KTProcessedTrackData::GetMinimumFrequency() const
    {
        return fMinFrequency;
    }

    inline double KTProcessedTrackData::GetMaximumFrequency() const
    {
        return fMaxFrequency;
    }

    inline double KTProcessedTrackData::GetFrequencyWidth() const
    {
        return fFrequencyWidth;
    }

    inline double KTProcessedTrackData::GetSlope() const
    {
        return fSlope;
    }

    inline double KTProcessedTrackData::GetIntercept() const
    {
        return fIntercept;
    }

    inline double KTProcessedTrackData::GetTimeInRunSigma() const
    {
        return fTimeInRunSigma;
    }

    inline double KTProcessedTrackData::GetTimeLengthSigma() const
    {
        return fTimeLengthSigma;
    }

    inline double KTProcessedTrackData::GetMinimumFrequencySigma() const
    {
        return fMinFrequencySigma;
    }

    inline double KTProcessedTrackData::GetMaximumFrequencySigma() const
    {
        return fMaxFrequencySigma;
    }

    inline double KTProcessedTrackData::GetFrequencyWidthSigma() const
    {
        return fFrequencyWidthSigma;
    }

    inline double KTProcessedTrackData::GetSlopeSigma() const
    {
        return fSlopeSigma;
    }

    inline double KTProcessedTrackData::GetInterceptSigma() const
    {
        return fInterceptSigma;
    }

    inline void KTProcessedTrackData::SetComponent(unsigned component)
    {
        fComponent = component;
        return;
    }

    inline void KTProcessedTrackData::SetTimeInRun(double tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTProcessedTrackData::SetTimeLength(double length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTProcessedTrackData::SetMinimumFrequency(double freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline void KTProcessedTrackData::SetMaximumFrequency(double freq)
    {
        fMaxFrequency = freq;
        return;
    }

    inline void KTProcessedTrackData::SetFrequencyWidth(double width)
    {
        fFrequencyWidth = width;
        return;
    }

    inline void KTProcessedTrackData::SetSlope(double slope)
    {
        fSlope = slope;
        return;
    }

    inline void KTProcessedTrackData::SetIntercept(double intercept)
    {
        fIntercept = intercept;
        return;
    }

    inline void KTProcessedTrackData::SetTimeInRunSigma(double sigma)
    {
        fTimeInRunSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetTimeLengthSigma(double sigma)
    {
        fTimeLengthSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetMinimumFrequencySigma(double sigma)
    {
        fMinFrequencySigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetMaximumFrequencySigma(double sigma)
    {
        fMaxFrequencySigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetFrequencyWidthSigma(double sigma)
    {
        fFrequencyWidthSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetSlopeSigma(double sigma)
    {
        fSlopeSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetInterceptSigma(double sigma)
    {
        fInterceptSigma = sigma;
        return;
    }

} /* namespace Katydid */
#endif /* KTPROCESSEDTRACKDATA_HH_ */
