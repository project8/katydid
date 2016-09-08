/*
 * KTROOTTreeTypeWriterEventAnalysis.hh
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITEREVENTANALYSIS_HH_
#define KTROOTTREETYPEWRITEREVENTANALYSIS_HH_

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
            UInt_t fAcquisitionID;
            UInt_t fCandidateID;
            //Double_t fTimeBinWidth;
            //Double_t fFreqBinWidth;
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


    class KTROOTTreeTypeWriterEventAnalysis : public KTROOTTreeTypeWriter//, public KTTypeWriterEventAnalysis
    {
        public:
            KTROOTTreeTypeWriterEventAnalysis();
            virtual ~KTROOTTreeTypeWriterEventAnalysis();

            void RegisterSlots();

        public:
            void WriteFrequencyCandidates(Nymph::KTDataPtr data);

            void WriteWaterfallCandidate(Nymph::KTDataPtr data);

            void WriteSparseWaterfallCandidate(Nymph::KTDataPtr data);

            void WriteProcessedTrack(Nymph::KTDataPtr data);

            void WriteMultiTrackEvent(Nymph::KTDataPtr data);

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

    inline TTree* KTROOTTreeTypeWriterEventAnalysis::GetFrequencyCandidateTree() const
    {
        return fFreqCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterEventAnalysis::GetWaterfallCandidateTree() const
    {
        return fWaterfallCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterEventAnalysis::GetSparseWaterfallCandidateTree() const
    {
        return fSparseWaterfallCandidateTree;
    }

    inline TTree* KTROOTTreeTypeWriterEventAnalysis::GetProcessedTrackTree() const
    {
        return fProcessedTrackTree;
    }

    inline TTree* KTROOTTreeTypeWriterEventAnalysis::GetMultiTrackEventTree() const
    {
        return fMultiTrackEventTree;
    }


} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITEREVENTANALYSIS_HH_ */
