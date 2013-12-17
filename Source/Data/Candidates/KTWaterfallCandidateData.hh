/*
 * KTWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTWATERFALLCANDIDATEDATA_HH_
#define KTWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"

#include "KTTimeFrequency.hh"

namespace Katydid
{
    class KTTimeFrequency;

    class KTWaterfallCandidateData : public KTExtensibleData< KTWaterfallCandidateData >
    {
        public:
            KTWaterfallCandidateData();
            virtual ~KTWaterfallCandidateData();

            KTTimeFrequency* GetCandidate() const;
            UInt_t GetComponent() const;

            UInt_t GetNTimeBins() const;
            double GetTimeBinWidth() const;

            UInt_t GetNFreqBins() const;
            double GetFreqBinWidth() const;

            double GetTimeInRun() const;
            double GetTimeLength() const;
            ULong64_t GetFirstSliceNumber() const;
            ULong64_t GetLastSliceNumber() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            double GetMeanStartFrequency() const;
            double GetMeanEndFrequency() const;
            double GetFrequencyWidth() const;

            UInt_t GetStartRecordNumber() const;
            UInt_t GetStartSampleNumber() const;
            UInt_t GetEndRecordNumber() const;
            UInt_t GetEndSampleNumber() const;

            void SetCandidate(KTTimeFrequency* candidate);
            void SetComponent(UInt_t component);

            void SetTimeInRun(double tir);
            void SetTimeLength(double length);
            void SetFirstSliceNumber(ULong64_t slice);
            void SetLastSliceNumber(ULong64_t slice);
            void SetMinimumFrequency(double freq);
            void SetMaximumFrequency(double freq);
            void SetMeanStartFrequency(double freq);
            void SetMeanEndFrequency(double freq);
            void SetFrequencyWidth(double width);

            void SetStartRecordNumber(UInt_t rec);
            void SetStartSampleNumber(UInt_t sample);
            void SetEndRecordNumber(UInt_t rec);
            void SetEndSampleNumber(UInt_t sample);

        protected:
            KTTimeFrequency* fCandidate;
            UInt_t fComponent;

            double fTimeInRun;
            double fTimeLength;
            ULong64_t fFirstSliceNumber;
            ULong64_t fLastSliceNumber;
            double fMinFrequency;
            double fMaxFrequency;
            double fMeanStartFrequency;
            double fMeanEndFrequency;
            double fFrequencyWidth;

            UInt_t fStartRecordNumber;
            UInt_t fStartSampleNumber;
            UInt_t fEndRecordNumber;
            UInt_t fEndSampleNumber;
    };

    inline KTTimeFrequency* KTWaterfallCandidateData::GetCandidate() const
    {
        return fCandidate;
    }

    inline UInt_t KTWaterfallCandidateData::GetComponent() const
    {
        return fComponent;
    }

    inline UInt_t KTWaterfallCandidateData::GetNTimeBins() const
    {
        return fCandidate->GetNTimeBins();
    }

    inline UInt_t KTWaterfallCandidateData::GetNFreqBins() const
    {
        return fCandidate->GetNFrequencyBins();
    }

    inline double KTWaterfallCandidateData::GetTimeBinWidth() const
    {
        return fCandidate->GetTimeBinWidth();
    }

    inline double KTWaterfallCandidateData::GetFreqBinWidth() const
    {
        return fCandidate->GetFrequencyBinWidth();
    }

    inline double KTWaterfallCandidateData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline double KTWaterfallCandidateData::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline ULong64_t KTWaterfallCandidateData::GetFirstSliceNumber() const
    {
        return fFirstSliceNumber;
    }

    inline ULong64_t KTWaterfallCandidateData::GetLastSliceNumber() const
    {
        return fLastSliceNumber;
    }

    inline double KTWaterfallCandidateData::GetMinimumFrequency() const
    {
        return fMinFrequency;
    }

    inline double KTWaterfallCandidateData::GetMaximumFrequency() const
    {
        return fMaxFrequency;
    }

    inline double KTWaterfallCandidateData::GetMeanStartFrequency() const
    {
        return fMeanStartFrequency;
    }

    inline double KTWaterfallCandidateData::GetMeanEndFrequency() const
    {
        return fMeanEndFrequency;
    }

    inline double KTWaterfallCandidateData::GetFrequencyWidth() const
    {
        return fFrequencyWidth;
    }

    inline UInt_t KTWaterfallCandidateData::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline UInt_t KTWaterfallCandidateData::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline UInt_t KTWaterfallCandidateData::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline UInt_t KTWaterfallCandidateData::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }

    inline void KTWaterfallCandidateData::SetComponent(UInt_t component)
    {
        fComponent = component;
        return;
    }

    inline void KTWaterfallCandidateData::SetTimeInRun(double tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTWaterfallCandidateData::SetTimeLength(double length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTWaterfallCandidateData::SetFirstSliceNumber(ULong64_t slice)
    {
        fFirstSliceNumber = slice;
        return;
    }

    inline void KTWaterfallCandidateData::SetLastSliceNumber(ULong64_t slice)
    {
        fLastSliceNumber = slice;
        return;
    }

    inline void KTWaterfallCandidateData::SetMinimumFrequency(double freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMaximumFrequency(double freq)
    {
        fMaxFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMeanStartFrequency(double freq)
    {
        fMeanStartFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMeanEndFrequency(double freq)
    {
        fMeanEndFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetFrequencyWidth(double width)
    {
        fFrequencyWidth = width;
        return;
    }

    inline void KTWaterfallCandidateData::SetStartRecordNumber(UInt_t rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline void KTWaterfallCandidateData::SetStartSampleNumber(UInt_t sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTWaterfallCandidateData::SetEndRecordNumber(UInt_t rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTWaterfallCandidateData::SetEndSampleNumber(UInt_t sample)
    {
        fEndSampleNumber = sample;
        return;
    }
} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
