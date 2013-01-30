/*
 * TFrequencyCandidateData.hh
 *
 *  Created on: Jan 25, 2013
 *      Author: nsoblath
 */

#ifndef TFREQUENCYCANDIDATEDATA_HH_
#define TFREQUENCYCANDIDATEDATA_HH_

#include "TObject.h"

class TClonesArray;

namespace Katydid
{
    class KTFrequencyCandidate;
    class KTFrequencyCandidateData;

    //***************************
    // TFrequencyCandidateData
    //***************************

    class TFrequencyCandidateData : public TObject
    {
        public:
            TFrequencyCandidateData();
            TFrequencyCandidateData(const KTFrequencyCandidateData& data);
            virtual ~TFrequencyCandidateData();

            void Load(const KTFrequencyCandidateData& data);

            void Clear();

            Double_t GetBinWidth() const;
            void SetBinWidth(Double_t binWidth);
            TClonesArray* GetComponentData() const;
            UInt_t GetNBins() const;
            void SetNBins(UInt_t nBins);
            Double_t GetTimeInRun() const;
            void SetTimeInRun(Double_t timeInRun);

        private:
            TClonesArray* fComponentData;  //->
            UInt_t fNBins;
            Double_t fBinWidth;
            Double_t fTimeInRun;

            ClassDef(TFrequencyCandidateData, 1);
    };

    inline Double_t TFrequencyCandidateData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void TFrequencyCandidateData::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
    }

    inline TClonesArray* TFrequencyCandidateData::GetComponentData() const
    {
        return fComponentData;
    }

    inline UInt_t TFrequencyCandidateData::GetNBins() const
    {
        return fNBins;
    }

    inline void TFrequencyCandidateData::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
    }

    inline Double_t TFrequencyCandidateData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline void TFrequencyCandidateData::SetTimeInRun(Double_t timeInRun)
    {
        fTimeInRun = timeInRun;
    }


    //************************************
    // TFrequencyCandidateDataComponent
    //************************************

    class TFrequencyCandidateDataComponent : public TObject
    {
        public:
            typedef std::vector< KTFrequencyCandidate > Candidates;

        public:
            TFrequencyCandidateDataComponent();
            TFrequencyCandidateDataComponent(const Candidates& candidates, Double_t threshold);
            virtual ~TFrequencyCandidateDataComponent();

            void Load(const Candidates& candidates, Double_t threshold);

            void Clear();

            TClonesArray* GetCandidates() const;
            Double_t GetThreshold() const;
            void SetThreshold(Double_t threshold);

        private:
            TClonesArray* fCandidates; //->
            Double_t fThreshold;

            ClassDef(TFrequencyCandidateDataComponent, 1);
    };

    inline TClonesArray* TFrequencyCandidateDataComponent::GetCandidates() const
    {
        return fCandidates;
    }

    inline Double_t TFrequencyCandidateDataComponent::GetThreshold() const
    {
        return fThreshold;
    }

    inline void TFrequencyCandidateDataComponent::SetThreshold(Double_t threshold)
    {
        fThreshold = threshold;
        return;
    }


    //************************************
    // TFrequencyCandidateDataCandidate
    //************************************

    class TFrequencyCandidateDataCandidate : public TObject
    {
        public:
            TFrequencyCandidateDataCandidate();
            TFrequencyCandidateDataCandidate(const KTFrequencyCandidate& candidate);
            virtual ~TFrequencyCandidateDataCandidate();

            void Load(const KTFrequencyCandidate& candidate);

            UInt_t GetFirstBin() const;
            void SetFirstBin(UInt_t bin);
            UInt_t GetLastBin() const;
            void SetLastBin(UInt_t bin);
            Double_t GetMeanFrequency() const;
            void SetMeanFrequency(Double_t freq);

        private:
            UInt_t fFirstBin;
            UInt_t fLastBin;
            Double_t fMeanFrequency;

            ClassDef(TFrequencyCandidateDataCandidate, 1);
    };

    inline UInt_t TFrequencyCandidateDataCandidate::GetFirstBin() const
    {
        return fFirstBin;
    }
    inline void TFrequencyCandidateDataCandidate::SetFirstBin(UInt_t bin)
    {
        fFirstBin = bin;
        return;
    }
    inline UInt_t TFrequencyCandidateDataCandidate::GetLastBin() const
    {
        return fLastBin;
    }
    inline void TFrequencyCandidateDataCandidate::SetLastBin(UInt_t bin)
    {
        fLastBin = bin;
        return;
    }
    inline Double_t TFrequencyCandidateDataCandidate::GetMeanFrequency() const
    {
        return fMeanFrequency;
    }
    inline void TFrequencyCandidateDataCandidate::SetMeanFrequency(Double_t freq)
    {
        fMeanFrequency = freq;
        return;
    }

} /* namespace Katydid */
#endif /* TFREQUENCYCANDIDATEDATA_HH_ */
