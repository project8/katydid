/*
 * KTROOTData.hh
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTDATA_HH_
#define KTROOTDATA_HH_

#include "KTMemberVariable.hh"

#include "TObject.h"

class TClonesArray;

namespace Katydid
{
    //***********************
    // TProcessedTrackData
    //***********************

    class KTProcessedTrackData;

    class TProcessedTrackData : public TObject
    {
            MEMBERVARIABLE(UInt_t, Component);
            MEMBERVARIABLE(UInt_t, AcquisitionID);
            MEMBERVARIABLE(UInt_t, TrackID);
            MEMBERVARIABLE(UInt_t, EventID);
            MEMBERVARIABLE(Int_t, EventSequenceID);

            MEMBERVARIABLE(Bool_t, IsCut);

            MEMBERVARIABLE(Double_t, MVAClassifier);
            MEMBERVARIABLE(Bool_t, Mainband);

            MEMBERVARIABLE(Double_t, StartTimeInRunC);
            MEMBERVARIABLE(Double_t, StartTimeInAcq);
            MEMBERVARIABLE(Double_t, EndTimeInRunC);
            MEMBERVARIABLE(Double_t, TimeLength);
            MEMBERVARIABLE(Double_t, StartFrequency);
            MEMBERVARIABLE(Double_t, EndFrequency);
            MEMBERVARIABLE(Double_t, FrequencyWidth);
            MEMBERVARIABLE(Double_t, Slope);
            MEMBERVARIABLE(Double_t, Intercept);
            MEMBERVARIABLE(Double_t, TotalPower);

            MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, TimeLengthSigma);
            MEMBERVARIABLE(Double_t, StartFrequencySigma);
            MEMBERVARIABLE(Double_t, EndFrequencySigma);
            MEMBERVARIABLE(Double_t, FrequencyWidthSigma);
            MEMBERVARIABLE(Double_t, SlopeSigma);
            MEMBERVARIABLE(Double_t, InterceptSigma);
            MEMBERVARIABLE(Double_t, TotalPowerSigma);

        public:
            TProcessedTrackData();
            TProcessedTrackData(const KTProcessedTrackData& data);
            TProcessedTrackData(const TProcessedTrackData& orig);
            virtual ~TProcessedTrackData();
            TObject* Clone(const char* newname="");
            TProcessedTrackData& operator=(const TProcessedTrackData& rhs);

            void Load(const KTProcessedTrackData& data);
            void Unload(KTProcessedTrackData& data) const;

            ClassDef(TProcessedTrackData, 1);
    };

    //***********************
    // TProcessedMPTData
    //***********************

    class KTProcessedMPTData;

    class TProcessedMPTData : public TObject
    {
            MEMBERVARIABLE(UInt_t, Component);
            MEMBERVARIABLE(Int_t, EventSequenceID);
            MEMBERVARIABLE(UInt_t, AcquisitionID);
            MEMBERVARIABLE(UInt_t, TrackID);

            MEMBERVARIABLE(Bool_t, IsCut);

            MEMBERVARIABLE(Double_t, MVAClassifier);
            MEMBERVARIABLE(Bool_t, Mainband);
            MEMBERVARIABLE(Double_t, AxialFrequency);

            MEMBERVARIABLE(Double_t, StartTimeInRunC);
            MEMBERVARIABLE(Double_t, StartTimeInAcq);
            MEMBERVARIABLE(Double_t, EndTimeInRunC);
            MEMBERVARIABLE(Double_t, TimeLength);
            MEMBERVARIABLE(Double_t, StartFrequency);
            MEMBERVARIABLE(Double_t, EndFrequency);
            MEMBERVARIABLE(Double_t, FrequencyWidth);
            MEMBERVARIABLE(Double_t, Slope);
            MEMBERVARIABLE(Double_t, Intercept);
            MEMBERVARIABLE(Double_t, TotalPower);

            MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, TimeLengthSigma);
            MEMBERVARIABLE(Double_t, StartFrequencySigma);
            MEMBERVARIABLE(Double_t, EndFrequencySigma);
            MEMBERVARIABLE(Double_t, FrequencyWidthSigma);
            MEMBERVARIABLE(Double_t, SlopeSigma);
            MEMBERVARIABLE(Double_t, InterceptSigma);
            MEMBERVARIABLE(Double_t, TotalPowerSigma);

        public:
            TProcessedMPTData();
            TProcessedMPTData(const KTProcessedMPTData& data);
            TProcessedMPTData(const TProcessedMPTData& orig);
            virtual ~TProcessedMPTData();
            TObject* Clone(const char* newname="");
            TProcessedMPTData& operator=(const TProcessedMPTData& rhs);

            void Load(const KTProcessedMPTData& data);
            void Unload(KTProcessedMPTData& mptData) const;

            ClassDef(TProcessedMPTData, 1);
    };

    //***********************
    // TClassifierResultsData
    //***********************

    class KTClassifierResultsData;

    class TClassifierResultsData : public TObject
    {
            MEMBERVARIABLE(UInt_t, Component);
            MEMBERVARIABLE(Int_t, MainCarrierHigh);
            MEMBERVARIABLE(Int_t, MainCarrierLow);
            MEMBERVARIABLE(Int_t, SideBand);

        public:
            TClassifierResultsData();
            TClassifierResultsData(const KTClassifierResultsData& data);
            TClassifierResultsData(const TClassifierResultsData& orig);
            virtual ~TClassifierResultsData();
            TObject* Clone(const char* newname="");
            TClassifierResultsData& operator=(const TClassifierResultsData& rhs);

            void Load(const KTClassifierResultsData& data);
            void Unload(KTClassifierResultsData& mptData) const;

            ClassDef(TClassifierResultsData, 1);
    };

    //************************
    // TMultiTrackEventData
    //************************

    class KTMultiTrackEventData;

    class TMultiTrackEventData : public TObject
    {
        MEMBERVARIABLE(UInt_t, Component);
        MEMBERVARIABLE(UInt_t, AcquisitionID);
        MEMBERVARIABLE(UInt_t, EventID);
        MEMBERVARIABLE(UInt_t, TotalEventSequences);

        MEMBERVARIABLE(Double_t, StartTimeInRunC);
        MEMBERVARIABLE(Double_t, StartTimeInAcq);
        MEMBERVARIABLE(Double_t, EndTimeInRunC);
        MEMBERVARIABLE(Double_t, TimeLength);
        MEMBERVARIABLE(Double_t, StartFrequency);
        MEMBERVARIABLE(Double_t, EndFrequency);
        MEMBERVARIABLE(Double_t, MinimumFrequency);
        MEMBERVARIABLE(Double_t, MaximumFrequency);
        MEMBERVARIABLE(Double_t, FrequencyWidth);

        MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, TimeLengthSigma);
        MEMBERVARIABLE(Double_t, StartFrequencySigma);
        MEMBERVARIABLE(Double_t, EndFrequencySigma);
        MEMBERVARIABLE(Double_t, FrequencyWidthSigma);

        MEMBERVARIABLE(UInt_t, FirstTrackID);
        MEMBERVARIABLE(Double_t, FirstTrackTimeLength);
        MEMBERVARIABLE(Double_t, FirstTrackFrequencyWidth);
        MEMBERVARIABLE(Double_t, FirstTrackSlope);
        MEMBERVARIABLE(Double_t, FirstTrackIntercept);
        MEMBERVARIABLE(Double_t, FirstTrackTotalPower);

        MEMBERVARIABLE(Double_t, UnknownEventTopology);

        public:
            TClonesArray* GetTracks() {return fTracks;}

        private:
            TClonesArray* fTracks; //->

        public:
            TMultiTrackEventData();
            TMultiTrackEventData(const KTMultiTrackEventData& data);
            TMultiTrackEventData(const TMultiTrackEventData& orig);
            virtual ~TMultiTrackEventData();
            TObject* Clone(const char* newname="");
            TMultiTrackEventData& operator=(const TMultiTrackEventData& rhs);

            void Load(const KTMultiTrackEventData& data);
            void Unload(KTMultiTrackEventData& data) const;

            ClassDef(TMultiTrackEventData, 1);
    };

    //************************
    // TClassifiedEventData
    //************************

    //class KTClassifiedEventData;

    class TClassifiedEventData : public TObject
    {
        MEMBERVARIABLE(UInt_t, Component);
        MEMBERVARIABLE(UInt_t, AcquisitionID);
        MEMBERVARIABLE(UInt_t, EventID);
        MEMBERVARIABLE(UInt_t, TotalEventSequences);

        MEMBERVARIABLE(Double_t, StartTimeInRunC);
        MEMBERVARIABLE(Double_t, StartTimeInAcq);
        MEMBERVARIABLE(Double_t, EndTimeInRunC);
        MEMBERVARIABLE(Double_t, TimeLength);
        MEMBERVARIABLE(Double_t, StartFrequency);
        MEMBERVARIABLE(Double_t, EndFrequency);
        MEMBERVARIABLE(Double_t, MinimumFrequency);
        MEMBERVARIABLE(Double_t, MaximumFrequency);
        MEMBERVARIABLE(Double_t, FrequencyWidth);

        MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, TimeLengthSigma);
        MEMBERVARIABLE(Double_t, StartFrequencySigma);
        MEMBERVARIABLE(Double_t, EndFrequencySigma);
        MEMBERVARIABLE(Double_t, FrequencyWidthSigma);

        MEMBERVARIABLE(UInt_t, FirstTrackID);
        MEMBERVARIABLE(Double_t, FirstTrackTimeLength);
        MEMBERVARIABLE(Double_t, FirstTrackFrequencyWidth);
        MEMBERVARIABLE(Double_t, FirstTrackSlope);
        MEMBERVARIABLE(Double_t, FirstTrackIntercept);
        MEMBERVARIABLE(Double_t, FirstTrackTotalPower);

        MEMBERVARIABLE(Double_t, UnknownEventTopology);

        public:
            TClonesArray* GetTracks() {return fTracks;}
            TClonesArray* GetClassifierResults() {return fClassifierResults;}

        private:
            TClonesArray* fTracks; //->
            TClonesArray* fClassifierResults; //->

        public:
            TClassifiedEventData();
            TClassifiedEventData(const KTMultiTrackEventData& data);
            TClassifiedEventData(const TClassifiedEventData& orig);
            virtual ~TClassifiedEventData();
            TObject* Clone(const char* newname="");
            TClassifiedEventData& operator=(const TClassifiedEventData& rhs);

            void Load(const KTMultiTrackEventData& data);
            void Unload(KTMultiTrackEventData& data) const;

            ClassDef(TClassifiedEventData, 1);
    };

    //************************
    // TDiscriminatedPoint
    //************************

    class KTDiscriminatedPoint;

    class TDiscriminatedPoint : public TObject
    {

        MEMBERVARIABLE(Double_t, TimeInRunC);
        MEMBERVARIABLE(Double_t, Frequency);
        MEMBERVARIABLE(Double_t, Amplitude);
        MEMBERVARIABLE(Double_t, TimeInAcq);
        MEMBERVARIABLE(Double_t, Mean);
        MEMBERVARIABLE(Double_t, Variance);
        MEMBERVARIABLE(Double_t, NeighborhoodAmplitude);

        public:
            TClonesArray* GetPoints() {return fPoints;}

        private:
            TClonesArray* fPoints;

        public:
            TDiscriminatedPoint();
            TDiscriminatedPoint(const TDiscriminatedPoint& data);
            virtual ~TDiscriminatedPoint();
            TObject* Clone(const char* newname="");
            TDiscriminatedPoint& operator=(const TDiscriminatedPoint& rhs);

            void Load(const KTDiscriminatedPoint& data);
            void Unload(KTDiscriminatedPoint& data) const;

            ClassDef(TDiscriminatedPoint, 1);
    };

    //************************
    // TSparseWaterFallCandidateData
    //************************

    class KTSparseWaterfallCandidateData;

    class TSparseWaterfallCandidateData : public TObject
    {
        MEMBERVARIABLE(UInt_t, Component);
        MEMBERVARIABLE(UInt_t, AcquisitionID);
        MEMBERVARIABLE(UInt_t, CandidateID);

        MEMBERVARIABLE(Double_t, TimeInRunC);
        MEMBERVARIABLE(Double_t, TimeLength);
        MEMBERVARIABLE(Double_t, MinFrequency);
        MEMBERVARIABLE(Double_t, MaxFrequency);
        MEMBERVARIABLE(Double_t, FrequencyWidth);

        public:
            TClonesArray* GetPoints() {return fPoints;}

        private:
            TClonesArray* fPoints;

        public:
            TSparseWaterfallCandidateData();
            TSparseWaterfallCandidateData(const TSparseWaterfallCandidateData& data);
            virtual ~TSparseWaterfallCandidateData();
            TObject* Clone(const char* newname="");
            TSparseWaterfallCandidateData& operator=(const TSparseWaterfallCandidateData& rhs);

            std::string GetBranchName() { return std::string("SparseWaterfall"); } // Defines the default name of the object saved inside the output tree

            // void Load(const TSparseWaterfallCandidateData& data);
            // void Unload(TSparseWaterfallCandidateData& data) const;

            ClassDef(TSparseWaterfallCandidateData, 1);
    };


}


#endif /* KTROOTDATA_HH_ */
