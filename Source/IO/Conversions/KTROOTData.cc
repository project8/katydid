/*
 * KTROOTData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTROOTData.hh"

#include "logger.hh"

#include "TClonesArray.h"

#include <iostream>

ClassImp(Katydid::TDiscriminatedPoint);
ClassImp(Katydid::TSparseWaterfallCandidateData);
ClassImp(Katydid::TSequentialLineData);

namespace Katydid
{

    //***********************
    // TDiscriminatedPoint
    //***********************

    TDiscriminatedPoint::TDiscriminatedPoint() :
            TObject(),
            fTimeInRunC(0), fFrequency(0), fAmplitude(0), fTimeInAcq(0),
            fMean(0), fVariance(0),fNeighborhoodAmplitude(0)
    {}

    TDiscriminatedPoint::TDiscriminatedPoint(const TDiscriminatedPoint& orig) :
            TObject(orig),
            fTimeInRunC(orig.fTimeInRunC), fFrequency(orig.fFrequency), fAmplitude(orig.fAmplitude), fTimeInAcq(orig.fTimeInAcq),
            fMean(orig.fMean), fVariance(orig.fVariance),fNeighborhoodAmplitude(orig.fNeighborhoodAmplitude)
    {}

    TDiscriminatedPoint::~TDiscriminatedPoint()
    {}

    TObject* TDiscriminatedPoint::Clone(const char* newname)
    {
        TDiscriminatedPoint* newData = new TDiscriminatedPoint(*this);
        return newData;
    }

    TDiscriminatedPoint& TDiscriminatedPoint::operator=(const TDiscriminatedPoint& rhs)
    {
        fTimeInRunC = rhs.fTimeInRunC; fFrequency = rhs.fFrequency; fAmplitude = rhs.fAmplitude; fTimeInAcq = rhs.fTimeInAcq;
        fMean = rhs.fMean; fVariance = rhs.fVariance; fNeighborhoodAmplitude = rhs.fNeighborhoodAmplitude;
        return *this;
    }

    //************************
    // TSparseWaterfallCandidate
    //************************

    TSparseWaterfallCandidateData::TSparseWaterfallCandidateData() :
            TObject(),
            fComponent(0), fAcquisitionID(0), fCandidateID(0),
            fTimeInRunC(0.), fTimeLength(0.),
            fMinFrequency(0.), fMaxFrequency(0.), fFrequencyWidth(0.)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray("Katydid::TDiscriminatedPoint", 20);
    }

    TSparseWaterfallCandidateData::TSparseWaterfallCandidateData(const TSparseWaterfallCandidateData& orig) :
            fComponent(orig.fComponent), fAcquisitionID(orig.fAcquisitionID), fCandidateID(orig.fCandidateID),
            fTimeInRunC(orig.fTimeInRunC), fTimeLength(orig.fTimeLength),
            fMinFrequency(orig.fMinFrequency), fMaxFrequency(orig.fMaxFrequency), fFrequencyWidth(orig.fFrequencyWidth)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray(*orig.fPoints);
    }

    TSparseWaterfallCandidateData::~TSparseWaterfallCandidateData()
    {
        fPoints->Clear();
        delete fPoints;
    }

    TObject* TSparseWaterfallCandidateData::Clone(const char* newname)
    {
        TSparseWaterfallCandidateData* newData = new TSparseWaterfallCandidateData(*this);
        return newData;
    }

    TSparseWaterfallCandidateData& TSparseWaterfallCandidateData::operator=(const TSparseWaterfallCandidateData& rhs)
    {
        fComponent = rhs.fComponent; fAcquisitionID = rhs.fAcquisitionID; fCandidateID = rhs.fCandidateID;
        fTimeInRunC = rhs.fTimeInRunC; fTimeLength = rhs.fTimeLength; 
        fMinFrequency = rhs.fMinFrequency; fMaxFrequency = rhs.fMaxFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fPoints->Clear(); (*fPoints) = *(rhs.fPoints);
        return *this;
    }


    //************************
    // TSequentialLineData
    //************************

    TSequentialLineData::TSequentialLineData() :
            TObject(),
            fComponent(0), fAcquisitionID(0), fCandidateID(0),
            fStartTimeInRunC(0.), fEndTimeInRunC(0.), fStartTimeInAcq(0.), fStartFrequency(0.), fEndFrequency(0.), fSlope(0.),
            fTotalPower(0.), fTotalTrackSNR(0.), fTotalTrackNUP(0.), fTotalWidePower(0.), fTotalWideTrackSNR(0.), fTotalWideTrackNUP(0.)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray("Katydid::TDiscriminatedPoint", 20);
    }

    TSequentialLineData::TSequentialLineData(const TSequentialLineData& orig):
            fComponent(orig.fComponent), fAcquisitionID(orig.fAcquisitionID), fCandidateID(orig.fCandidateID),
            fStartTimeInRunC(orig.fStartTimeInRunC), fEndTimeInRunC(orig.fEndTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fSlope(orig.fSlope), 
            fTotalPower(orig.fTotalPower), fTotalTrackSNR(orig.fTotalTrackSNR), fTotalTrackNUP(orig.fTotalTrackNUP), fTotalWidePower(orig.fTotalWidePower), fTotalWideTrackSNR(orig.fTotalWideTrackSNR), fTotalWideTrackNUP(orig.fTotalWideTrackNUP)
    {
        // this cannot be initialized in the initializer list because ROOT
        fPoints = new TClonesArray(*orig.fPoints);
    }

    TSequentialLineData::~TSequentialLineData()
    {
        fPoints->Clear();
        delete fPoints;
    }

    TObject* TSequentialLineData::Clone(const char* newname)
    {
        TSequentialLineData* newData = new TSequentialLineData(*this);
        return newData;
    }

    TSequentialLineData& TSequentialLineData::operator=(const TSequentialLineData& rhs)
    {
        fComponent = rhs.fComponent; fAcquisitionID = rhs.fAcquisitionID; fCandidateID = rhs.fCandidateID;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fEndTimeInRunC = rhs.fEndTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fSlope = rhs.fSlope; 
        fTotalPower = rhs.fTotalPower; fTotalTrackSNR = rhs.fTotalTrackSNR; fTotalTrackNUP = rhs.fTotalTrackNUP; fTotalWidePower = rhs.fTotalWidePower; fTotalWideTrackSNR = rhs.fTotalWideTrackSNR; fTotalWideTrackNUP = rhs.fTotalWideTrackNUP;
        fPoints->Clear(); (*fPoints) = *(rhs.fPoints);
        return *this;
    }
}
