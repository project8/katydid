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

            bool GetIsCut() const;

            double GetStartTimeInRun() const;
            double GetEndTimeInRun() const;
            double GetTimeLength() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            double GetFrequencyWidth() const;
            double GetSlope() const;
            double GetIntercept() const;

            double GetStartTimeInRunSigma() const;
            double GetEndTimeInRunSigma() const;
            double GetTimeLengthSigma() const;
            double GetMinimumFrequencySigma() const;
            double GetMaximumFrequencySigma() const;
            double GetFrequencyWidthSigma() const;
            double GetSlopeSigma() const;
            double GetInterceptSigma() const;

            void SetComponent(unsigned component);

            void SetIsCut(bool isCut);

            void SetStartTimeInRun(double tir);
            void SetEndTimeInRun(double tir);
            void SetTimeLength(double length);
            void SetMinimumFrequency(double freq);
            void SetMaximumFrequency(double freq);
            void SetFrequencyWidth(double width);
            void SetSlope(double slope);
            void SetIntercept(double intercept);

            void SetStartTimeInRunSigma(double sigma);
            void SetEndTimeInRunSigma(double sigma);
            void SetTimeLengthSigma(double sigma);
            void SetMinimumFrequencySigma(double sigma);
            void SetMaximumFrequencySigma(double sigma);
            void SetFrequencyWidthSigma(double sigma);
            void SetSlopeSigma(double sigma);
            void SetInterceptSigma(double sigma);

        protected:
            unsigned fComponent;

            bool fIsCut;

            double fStartTimeInRun;
            double fEndTimeInRun;
            double fTimeLength;
            double fMinFrequency;
            double fMaxFrequency;
            double fFrequencyWidth;
            double fSlope;
            double fIntercept;

            double fStartTimeInRunSigma;
            double fEndTimeInRunSigma;
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

    inline bool KTProcessedTrackData::GetIsCut() const
    {
        return fIsCut;
    }

    inline double KTProcessedTrackData::GetStartTimeInRun() const
    {
        return fStartTimeInRun;
    }

    inline double KTProcessedTrackData::GetEndTimeInRun() const
    {
        return fEndTimeInRun;
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

    inline double KTProcessedTrackData::GetStartTimeInRunSigma() const
    {
        return fStartTimeInRunSigma;
    }

    inline double KTProcessedTrackData::GetEndTimeInRunSigma() const
    {
        return fEndTimeInRunSigma;
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

    inline void KTProcessedTrackData::SetIsCut(bool isCut)
    {
        fIsCut = isCut;
        return;
    }

    inline void KTProcessedTrackData::SetStartTimeInRun(double tir)
    {
        fStartTimeInRun = tir;
        return;
    }

    inline void KTProcessedTrackData::SetEndTimeInRun(double tir)
    {
        fEndTimeInRun = tir;
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

    inline void KTProcessedTrackData::SetStartTimeInRunSigma(double sigma)
    {
        fStartTimeInRunSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetEndTimeInRunSigma(double sigma)
    {
        fEndTimeInRunSigma = sigma;
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
