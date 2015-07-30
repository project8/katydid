/*
 * KTWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "stdint.h"

#ifndef KTWATERFALLCANDIDATEDATA_HH_
#define KTWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"

#include "KTTimeFrequency.hh"

#include <inttypes.h>

namespace Katydid
{
    using namespace Nymph;
    class KTTimeFrequency;

    class KTWaterfallCandidateData : public KTExtensibleData< KTWaterfallCandidateData >
    {
        public:
            KTWaterfallCandidateData();
            virtual ~KTWaterfallCandidateData();

            KTTimeFrequency* GetCandidate() const;
            unsigned GetComponent() const;

            unsigned GetNTimeBins() const;
            double GetTimeBinWidth() const;

            unsigned GetNFreqBins() const;
            double GetFreqBinWidth() const;

            double GetTimeInRun() const;
            double GetTimeLength() const;
            uint64_t GetFirstSliceNumber() const;
            uint64_t GetLastSliceNumber() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            double GetMeanStartFrequency() const;
            double GetMeanEndFrequency() const;
            double GetFrequencyWidth() const;

            unsigned GetStartRecordNumber() const;
            unsigned GetStartSampleNumber() const;
            unsigned GetEndRecordNumber() const;
            unsigned GetEndSampleNumber() const;

            void SetCandidate(KTTimeFrequency* candidate);
            void SetComponent(unsigned component);

            void SetTimeInRun(double tir);
            void SetTimeLength(double length);
            void SetFirstSliceNumber(uint64_t slice);
            void SetLastSliceNumber(uint64_t slice);
            void SetMinimumFrequency(double freq);
            void SetMaximumFrequency(double freq);
            void SetMeanStartFrequency(double freq);
            void SetMeanEndFrequency(double freq);
            void SetFrequencyWidth(double width);

            void SetStartRecordNumber(unsigned rec);
            void SetStartSampleNumber(unsigned sample);
            void SetEndRecordNumber(unsigned rec);
            void SetEndSampleNumber(unsigned sample);

        private:
            KTTimeFrequency* fCandidate;
            unsigned fComponent;

            double fTimeInRun;
            double fTimeLength;
            uint64_t fFirstSliceNumber;
            uint64_t fLastSliceNumber;
            double fMinFrequency;
            double fMaxFrequency;
            double fMeanStartFrequency;
            double fMeanEndFrequency;
            double fFrequencyWidth;

            unsigned fStartRecordNumber;
            unsigned fStartSampleNumber;
            unsigned fEndRecordNumber;
            unsigned fEndSampleNumber;

        public:
            static const std::string sName;
};

    inline KTTimeFrequency* KTWaterfallCandidateData::GetCandidate() const
    {
        return fCandidate;
    }

    inline unsigned KTWaterfallCandidateData::GetComponent() const
    {
        return fComponent;
    }

    inline unsigned KTWaterfallCandidateData::GetNTimeBins() const
    {
        return fCandidate->GetNTimeBins();
    }

    inline unsigned KTWaterfallCandidateData::GetNFreqBins() const
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

    inline uint64_t KTWaterfallCandidateData::GetFirstSliceNumber() const
    {
        return fFirstSliceNumber;
    }

    inline uint64_t KTWaterfallCandidateData::GetLastSliceNumber() const
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

    inline unsigned KTWaterfallCandidateData::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline unsigned KTWaterfallCandidateData::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline unsigned KTWaterfallCandidateData::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline unsigned KTWaterfallCandidateData::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }

    inline void KTWaterfallCandidateData::SetComponent(unsigned component)
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

    inline void KTWaterfallCandidateData::SetFirstSliceNumber(uint64_t slice)
    {
        fFirstSliceNumber = slice;
        return;
    }

    inline void KTWaterfallCandidateData::SetLastSliceNumber(uint64_t slice)
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

    inline void KTWaterfallCandidateData::SetStartRecordNumber(unsigned rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline void KTWaterfallCandidateData::SetStartSampleNumber(unsigned sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTWaterfallCandidateData::SetEndRecordNumber(unsigned rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTWaterfallCandidateData::SetEndSampleNumber(unsigned sample)
    {
        fEndSampleNumber = sample;
        return;
    }
} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
