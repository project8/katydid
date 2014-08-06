/*
 * KTROOTTreeTypeWriterCandidates.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERCANDIDATES_HH_
#define KTROOTTREETYPEWRITERCANDIDATES_HH_

#include "KTROOTTreeWriter.hh"

#include "KTData.hh"
#include "KTROOTData.hh"

#include "Rtypes.h"

class TGraph2D;
class TH2D;
class TTree;

namespace Katydid
{
    //class KTFrequencyCandidateData;
    //class KTWaterfallCandidateData;

    struct TFrequencyCandidateData
    {
        UInt_t fComponent;
        ULong64_t fSlice;
        Double_t fTimeInRun;
        Double_t fThreshold;
        UInt_t fFirstBin;
        UInt_t fLastBin;
        Double_t fMeanFrequency;
        Double_t fPeakAmplitude;
        Double_t fAmplitudeSum;
    };

    struct TWaterfallCandidateData
    {
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
        TH2D* fCandidate;
    };

    // commented-out fields match fields not yet implemented in KTSparseWaterfallCandidateData
    struct TSparseWaterfallCandidateData
    {
            TGraph2D* fPoints;
            UInt_t fComponent;
            UInt_t fCandidateID;
            Double_t fTimeBinWidth;
            Double_t fFreqBinWidth;
            Double_t fTimeInRunC;
            Double_t fTimeLength;
            //ULong64_t fFirstSliceNumber;
            //ULong64_t fLastSliceNumber;
            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            //Double_t fMeanStartFrequency;
            //Double_t fMeanEndFrequency;
            Double_t fFrequencyWidth;
            //UInt_t fStartRecordNumber;
            //UInt_t fStartSampleNumber;
            //UInt_t fEndRecordNumber;
            //UInt_t fEndSampleNumber;

    };


    class KTROOTTreeTypeWriterCandidates : public KTROOTTreeTypeWriter//, public KTTypeWriterCandidates
    {
        public:
            KTROOTTreeTypeWriterCandidates();
            virtual ~KTROOTTreeTypeWriterCandidates();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(KTDataPtr data);

            void WriteWaterfallCandidate(KTDataPtr data);

            void WriteSparseWaterfallCandidate(KTDataPtr data);

            void WriteProcessedTrack(KTDataPtr data);

            void WriteMultiTrackEvent(KTDataPtr data);

        public:
            TTree* GetFrequencyCandidateTree() const;
            TTree* GetWaterfallCandidateTree() const;
            TTree* GetSparseWaterfallCandidateTree() const;
            TTree* GetProcessedTrackTree() const;
            TTree* GetMultiTrackEventTree() const;

        private:
            bool SetupFrequencyCandidateTree();
            bool SetupWaterfallCandidateTree();
            bool SetupSparseWaterfallCandidateTree();
            bool SetupProcessedTrackTree();
            bool SetupMultiTrackEventTree();

            TTree* fFreqCandidateTree;
            TTree* fWaterfallCandidateTree;
            TTree* fSparseWaterfallCandidateTree;
            TTree* fProcessedTrackTree;
            TTree* fMultiTrackEventTree;

            TFrequencyCandidateData fFreqCandidateData;
            TWaterfallCandidateData fWaterfallCandidateData;
            TSparseWaterfallCandidateData fSparseWaterfallCandidateData;
            TProcessedTrackData* fProcessedTrackDataPtr;
            TMultiTrackEventData* fMultiTrackEventDataPtr;

    };

    inline TTree* KTROOTTreeTypeWriterCandidates::GetFrequencyCandidateTree() const
    {
        return fFreqCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterCandidates::GetWaterfallCandidateTree() const
    {
        return fWaterfallCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterCandidates::GetSparseWaterfallCandidateTree() const
    {
        return fSparseWaterfallCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterCandidates::GetProcessedTrackTree() const
    {
        return fProcessedTrackTree;
    }

    inline TTree* KTROOTTreeTypeWriterCandidates::GetMultiTrackEventTree() const
    {
        return fMultiTrackEventTree;
    }


} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERCANDIDATES_HH_ */
