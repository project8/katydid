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

            double GetStartTimeInRunC() const;
            double GetEndTimeInRunC() const;
            double GetTimeLength() const;
            double GetStartFrequency() const;
            double GetEndFrequency() const;
            double GetFrequencyWidth() const;
            double GetSlope() const;
            double GetIntercept() const;
            double GetTotalPower() const;

            double GetStartTimeInRunCSigma() const;
            double GetEndTimeInRunCSigma() const;
            double GetTimeLengthSigma() const;
            double GetStartFrequencySigma() const;
            double GetEndFrequencySigma() const;
            double GetFrequencyWidthSigma() const;
            double GetSlopeSigma() const;
            double GetInterceptSigma() const;
            double GetTotalPowerSigma() const;

            void SetComponent(unsigned component);

            void SetIsCut(bool isCut);

            void SetStartTimeInRunC(double tirc);
            void SetEndTimeInRunC(double tirc);
            void SetTimeLength(double length);
            void SetStartFrequency(double freq);
            void SetEndFrequency(double freq);
            void SetFrequencyWidth(double width);
            void SetSlope(double slope);
            void SetIntercept(double intercept);
            void SetTotalPower(double power);

            void SetStartTimeInRunCSigma(double sigma);
            void SetEndTimeInRunCSigma(double sigma);
            void SetTimeLengthSigma(double sigma);
            void SetStartFrequencySigma(double sigma);
            void SetEndFrequencySigma(double sigma);
            void SetFrequencyWidthSigma(double sigma);
            void SetSlopeSigma(double sigma);
            void SetInterceptSigma(double sigma);
            void SetTotalPowerSigma(double sigma);

        protected:
            unsigned fComponent;

            bool fIsCut;

            double fStartTimeInRunC;
            double fEndTimeInRunC;
            double fTimeLength;
            double fStartFrequency;
            double fEndFrequency;
            double fFrequencyWidth;
            double fSlope;
            double fIntercept;
            double fTotalPower;

            double fStartTimeInRunCSigma;
            double fEndTimeInRunCSigma;
            double fTimeLengthSigma;
            double fStartFrequencySigma;
            double fEndFrequencySigma;
            double fFrequencyWidthSigma;
            double fSlopeSigma;
            double fInterceptSigma;
            double fTotalPowerSigma;
    };

    inline unsigned KTProcessedTrackData::GetComponent() const
    {
        return fComponent;
    }

    inline bool KTProcessedTrackData::GetIsCut() const
    {
        return fIsCut;
    }

    inline double KTProcessedTrackData::GetStartTimeInRunC() const
    {
        return fStartTimeInRunC;
    }

    inline double KTProcessedTrackData::GetEndTimeInRunC() const
    {
        return fEndTimeInRunC;
    }

    inline double KTProcessedTrackData::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline double KTProcessedTrackData::GetStartFrequency() const
    {
        return fStartFrequency;
    }

    inline double KTProcessedTrackData::GetEndFrequency() const
    {
        return fEndFrequency;
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

    inline double KTProcessedTrackData::GetTotalPower() const
    {
        return fTotalPower;
    }

    inline double KTProcessedTrackData::GetStartTimeInRunCSigma() const
    {
        return fStartTimeInRunCSigma;
    }

    inline double KTProcessedTrackData::GetEndTimeInRunCSigma() const
    {
        return fEndTimeInRunCSigma;
    }

    inline double KTProcessedTrackData::GetTimeLengthSigma() const
    {
        return fTimeLengthSigma;
    }

    inline double KTProcessedTrackData::GetStartFrequencySigma() const
    {
        return fStartFrequencySigma;
    }

    inline double KTProcessedTrackData::GetEndFrequencySigma() const
    {
        return fEndFrequencySigma;
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

    inline double KTProcessedTrackData::GetTotalPowerSigma() const
    {
        return fTotalPowerSigma;
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

    inline void KTProcessedTrackData::SetStartTimeInRunC(double tirc)
    {
        fStartTimeInRunC = tirc;
        return;
    }

    inline void KTProcessedTrackData::SetEndTimeInRunC(double tirc)
    {
        fEndTimeInRunC = tirc;
        return;
    }

    inline void KTProcessedTrackData::SetTimeLength(double length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTProcessedTrackData::SetStartFrequency(double freq)
    {
        fStartFrequency = freq;
        return;
    }

    inline void KTProcessedTrackData::SetEndFrequency(double freq)
    {
        fEndFrequency = freq;
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

    inline void KTProcessedTrackData::SetTotalPower(double power)
    {
        fTotalPower = power;
        return;
    }

    inline void KTProcessedTrackData::SetStartTimeInRunCSigma(double sigma)
    {
        fStartTimeInRunCSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetEndTimeInRunCSigma(double sigma)
    {
        fEndTimeInRunCSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetTimeLengthSigma(double sigma)
    {
        fTimeLengthSigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetStartFrequencySigma(double sigma)
    {
        fStartFrequencySigma = sigma;
        return;
    }

    inline void KTProcessedTrackData::SetEndFrequencySigma(double sigma)
    {
        fEndFrequencySigma = sigma;
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

    inline void KTProcessedTrackData::SetTotalPowerSigma(double sigma)
    {
        fTotalPowerSigma = sigma;
        return;
    }

} /* namespace Katydid */
#endif /* KTPROCESSEDTRACKDATA_HH_ */
