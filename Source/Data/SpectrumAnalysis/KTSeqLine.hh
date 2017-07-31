/*
 * KTSeqLine.hh
 *
 *  Created on: Sep 8, 2016
 *      Author: Christine
 */

#ifndef KTSEQLINE_HH_
#define KTSEQLINE_HH_


#include "KTFrequencyDomainArray.hh"
#include "KTPhysicalArray.hh"
#include "KTMemberVariable.hh"
//#include "KTSeqTrackCreator.hh"


#include <string>
#include <vector>

namespace Katydid {
    KTLOGGER(sllog, "KTSeqLine");


    //using namespace Nymph;

    struct Point
    {
        double fBinInSlice;
        double fPointFreq;
        double fTimeInAcq;
        double fTimeInRunC;
        double fAmplitude;
        uint64_t fAcquisitionID;
        double fComponent;

        Point(unsigned BinInSclice, double PointFreq, double TimeInAcq, double TimeInRunC, double Amplitude, uint64_t AcqID, unsigned iComponent) : fBinInSlice(BinInSclice), fPointFreq(PointFreq),
                fTimeInAcq(TimeInAcq), fTimeInRunC(TimeInRunC), fAmplitude(Amplitude), fAcquisitionID(AcqID), fComponent(iComponent) {}

        void Clear();
        bool operator > (const Point& str) const
        {
            return (fAmplitude > str.fAmplitude);
        }
        bool operator < (const Point& str) const
        {
            return (fAmplitude < str.fAmplitude);
        }
    };

    struct LinePoint
    {
        double fBinInSlice;
        double fPointFreq;
        double fTimeInAcq;
        double fTimeInRunC;
        double fAmplitude;
        uint64_t fAcquisitionID;
        double fComponent;

        LinePoint(unsigned BinInSclice, double PointFreq, double TimeInAcq, double TimeInRunC, double Amplitude, uint64_t AcqID, unsigned iComponent) : fBinInSlice(BinInSclice), fPointFreq(PointFreq),
                fTimeInAcq(TimeInAcq), fTimeInRunC(TimeInRunC), fAmplitude(Amplitude), fAcquisitionID(AcqID), fComponent(iComponent) {}
        void Clear();

        bool operator > (const Point& str) const
        {
            return (fTimeInRunC > str.fTimeInRunC);
        }
        bool operator < (const Point& str) const
        {
            return (fTimeInRunC < str.fTimeInRunC);
        }
    };


    struct LineRef
    {
        //std::set< TrackSetCIt, TrackSetCItComp > fTrackRefs;
        // Keep track of both the sum and the mean so that the mean can be updated regularly without an extra multiplication

        std::vector<double> fTrimmingLimits;
        std::vector<LinePoint> fLinePoints;
        std::vector<double> fAmplitudeList;

        double fStartTimeInRunC;
        double fEndTimeInRunC;
        double fStartTimeInAcq;
        double fEndTimeInAcq;
        double fStartFrequency;
        double fEndFrequency;
        double fSlope;
        double fAmplitudeSum;
        unsigned fNPoints;
        uint64_t fAcquisitionID;


        LineRef();
        void InsertPoint(const Point& Point, const double& new_trimming_limits);
        void LineTrimming(const double& TrimminFactor, const unsigned& MinPoints);
        void CalculateSlope();
        void UpdateLineParameters();
        void Clear();
        virtual ~LineRef();

        bool operator < (const LineRef& str) const
        {
            return (fStartTimeInRunC < str.fStartTimeInRunC);
        }
    };



 /*   struct LineComp
    {
        bool operator() (const LineRef& lhs, const LineRef& rhs)
        {
            if (lhs.fStartTimeInRunC != rhs.fStartTimeInRunC) return lhs.fStartTimeInRunC < rhs.fStartTimeInRunC;
                        return lhs.fEndTimeInRunC < rhs.fEndTimeInRunC;
        }

    };
*/




} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_ */


