/*
 * KTROOTTreeTypeWriterCandidates.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERCANDIDATES_HH_
#define KTROOTTREETYPEWRITERCANDIDATES_HH_

#include "KTROOTTreeWriter.hh"

#include "TObject.h"

namespace Katydid
{
    class KTFrequencyCandidateData;

    class KTROOTTreeTypeWriterCandidates : public KTJSONTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTROOTTreeTypeWriterCandidates();
            virtual ~KTROOTTreeTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(const KTFrequencyCandidateData* header);

        public:
            TTree* GetFrequencyCandidateTree() const;

        private:
            void SetupFrequencyCandidateTree();

            TTree* fFreqCandidateTree;

    };

    TTree* KTROOTTreeTypeWriterCandidates::GetFrequencyCandidateTree() const
    {
        return fFreqCandidateTree;
    }



    //********************************
    // Tree Classes/Structures
    //********************************

    class TFrequencyCandidateData : public TObject
    {
        public:
            TFrequencyCandidateData(const KTFrequencyCandidateData& data);
            virtual ~TFrequencyCandidateData();

            Double_t GetBinWidth() const;
            void SetBinWidth(Double_t binWidth);
            TClonesArray* GetComponentData() const;
            void AddComponentData(const KTFrequencyCandidateDataComponent& componentData);
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

    class TFrequencyCandidateDataComponent : public TObject
    {
        public:
            TFrequencyCandidateDataComponent(const KTFrequencyCandidateData::Candidates& candidates, Double_t threshold);
            virtual ~TFrequencyCandidateDataComponent();

            TClonesArray* GetCandidates() const;
            void AddCandidate(const KTFrequencyCandidate& candidate);
            Double_t GetThreshold() const;
            void SetThreshold(Double_t threshold);

        private:
            TClonesArray* fCandidates; //->
            Double_t fThreshold;

            ClassDef(TFrequencyCandidateDataComponent, 1);
    };

    class TFrequencyCandidateDataCandidate : public TObject
    {
        public:
            TFrequencyCandidateDataCandidate(const KTFrequencyCandidate& candidate);
            virtual ~TFrequencyCandidateDataCandidate();

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

} /* namespace Katydid */
#endif /* KTROOTTREETYPEWRITERCANDIDATES_HH_ */
