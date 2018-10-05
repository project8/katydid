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
        MEMBERVARIABLE(UInt_t, BinInSlice);

        public:
            TDiscriminatedPoint();
            TDiscriminatedPoint(const TDiscriminatedPoint& orig);
            virtual ~TDiscriminatedPoint();
            TObject* Clone(const char* newname="");
            TDiscriminatedPoint& operator=(const TDiscriminatedPoint& rhs);

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
            TSparseWaterfallCandidateData(const TSparseWaterfallCandidateData& orig);
            virtual ~TSparseWaterfallCandidateData();
            TObject* Clone(const char* newname="");
            TSparseWaterfallCandidateData& operator=(const TSparseWaterfallCandidateData& rhs);

            std::string GetBranchName() { return std::string("SparseWaterfall"); } // Defines the default name of the object saved inside the output tree

            ClassDef(TSparseWaterfallCandidateData, 1);
    };

    //************************
    // TSequentialLineData
    //************************

    class KTSequentialLineData;

    class TSequentialLineData : public TObject
    {
        MEMBERVARIABLE(UInt_t, Component);
        MEMBERVARIABLE(UInt_t, AcquisitionID);
        MEMBERVARIABLE(UInt_t, CandidateID);

        MEMBERVARIABLE(Double_t, StartTimeInRunC);
        MEMBERVARIABLE(Double_t, EndTimeInRunC);
        MEMBERVARIABLE(Double_t, StartTimeInAcq);
        MEMBERVARIABLE(Double_t, StartFrequency);
        MEMBERVARIABLE(Double_t, EndFrequency);
        MEMBERVARIABLE(Double_t, Slope);

        MEMBERVARIABLE(Double_t, TotalPower);
        MEMBERVARIABLE(Double_t, TotalTrackSNR);
        MEMBERVARIABLE(Double_t, TotalTrackNUP);
        MEMBERVARIABLE(Double_t, TotalWidePower);
        MEMBERVARIABLE(Double_t, TotalWideTrackSNR);
        MEMBERVARIABLE(Double_t, TotalWideTrackNUP);

        public:
            TClonesArray* GetPoints() {return fPoints;}

        private:
            TClonesArray* fPoints;

        public:
            TSequentialLineData();
            TSequentialLineData(const TSequentialLineData& orig);
            virtual ~TSequentialLineData();
            TObject* Clone(const char* newname="");
            TSequentialLineData& operator=(const TSequentialLineData& rhs);

            std::string GetBranchName() { return std::string("SequentialLine"); } // Defines the default name of the object saved inside the output tree

            ClassDef(TSequentialLineData, 1);
    };
}

#endif /* KTROOTDATA_HH_ */
