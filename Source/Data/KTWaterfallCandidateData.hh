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
            Double_t GetTimeBinWidth() const;

            UInt_t GetNFreqBins() const;
            Double_t GetFreqBinWidth() const;

            Double_t GetTimeInRun() const;
            Double_t GetTimeLength() const;
            ULong64_t GetFirstSliceNumber() const;
            ULong64_t GetLastSliceNumber() const;
            Double_t GetMinimumFrequency() const;
            Double_t GetMaximumFrequency() const;
            Double_t GetMeanStartFrequency() const;
            Double_t GetMeanEndFrequency() const;
            Double_t GetFrequencyWidth() const;

            void SetCandidate(KTTimeFrequency* candidate);
            void SetComponent(UInt_t component);

            void SetTimeInRun(Double_t tir);
            void SetTimeLength(Double_t length);
            void SetFirstSliceNumber(ULong64_t slice);
            void SetLastSliceNumber(ULong64_t slice);
            void SetMinimumFrequency(Double_t freq);
            void SetMaximumFrequency(Double_t freq);
            void SetMeanStartFrequency(Double_t freq);
            void SetMeanEndFrequency(Double_t freq);
            void SetFrequencyWidth(Double_t width);

        protected:
            KTTimeFrequency* fCandidate;
            UInt_t fComponent;

            Double_t fTimeInRun;
            Double_t fTimeLength;
            ULong64_t fFirstSliceNumber;
            ULong64_t fLastSliceNumber;
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            Double_t fMeanStartFrequency;
            Double_t fMeanEndFrequency;
            Double_t fFrequencyWidth;
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

    inline Double_t KTWaterfallCandidateData::GetTimeBinWidth() const
    {
        return fCandidate->GetTimeBinWidth();
    }

    inline Double_t KTWaterfallCandidateData::GetFreqBinWidth() const
    {
        return fCandidate->GetFrequencyBinWidth();
    }

    inline Double_t KTWaterfallCandidateData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline Double_t KTWaterfallCandidateData::GetTimeLength() const
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

    inline Double_t KTWaterfallCandidateData::GetMinimumFrequency() const
    {
        return fMinFrequency;
    }

    inline Double_t KTWaterfallCandidateData::GetMaximumFrequency() const
    {
        return fMaxFrequency;
    }

    inline Double_t KTWaterfallCandidateData::GetMeanStartFrequency() const
    {
        return fMeanStartFrequency;
    }

    inline Double_t KTWaterfallCandidateData::GetMeanEndFrequency() const
    {
        return fMeanEndFrequency;
    }

    inline Double_t KTWaterfallCandidateData::GetFrequencyWidth() const
    {
        return fFrequencyWidth;
    }

    inline void KTWaterfallCandidateData::SetComponent(UInt_t component)
    {
        fComponent = component;
        return;
    }

    inline void KTWaterfallCandidateData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTWaterfallCandidateData::SetTimeLength(Double_t length)
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

    inline void KTWaterfallCandidateData::SetMinimumFrequency(Double_t freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMaximumFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMeanStartFrequency(Double_t freq)
    {
        fMeanStartFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetMeanEndFrequency(Double_t freq)
    {
        fMeanEndFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidateData::SetFrequencyWidth(Double_t width)
    {
        fFrequencyWidth = width;
        return;
    }

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
