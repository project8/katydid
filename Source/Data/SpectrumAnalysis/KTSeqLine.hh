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


    // this is a point from a power spectrum slice
    // sorting it will sort it by power
    struct Point
    {
        double fBinInSlice;
        double fPointFreq;
        double fTimeInAcq;
        double fTimeInRunC;
        double fAmplitude;
        double fThreshold;
        uint64_t fAcquisitionID;
        double fComponent;

        Point(unsigned BinInSclice, double PointFreq, double TimeInAcq, double TimeInRunC, double Amplitude, double Threshold, uint64_t AcqID, unsigned iComponent) : fBinInSlice(BinInSclice), fPointFreq(PointFreq),
                fTimeInAcq(TimeInAcq), fTimeInRunC(TimeInRunC), fAmplitude(Amplitude), fThreshold(Threshold), fAcquisitionID(AcqID), fComponent(iComponent) {}

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

    // this is a line point. Its amplitude is the sum of all bins in the power slice of its frequency +/- the line width
    // sort by time
    struct LinePoint
    {
        double fBinInSlice;
        double fPointFreq;
        double fTimeInAcq;
        double fTimeInRunC;
        double fAmplitude;
        double fThreshold;
        uint64_t fAcquisitionID;
        double fComponent;

        LinePoint(unsigned BinInSclice, double PointFreq, double TimeInAcq, double TimeInRunC, double Amplitude, double Threshold, uint64_t AcqID, unsigned iComponent) : fBinInSlice(BinInSclice), fPointFreq(PointFreq),
                fTimeInAcq(TimeInAcq), fTimeInRunC(TimeInRunC), fAmplitude(Amplitude), fThreshold(Threshold), fAcquisitionID(AcqID), fComponent(iComponent) {}
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
        double fInitialSlope;
        unsigned fComponent;
        double fAmplitudeSum;
        unsigned fNPoints;
        uint64_t fAcquisitionID;
        unsigned fProcTrackMinPoints;
        double fProcTrackAssError;
        double fIntercept;
        double fSlopeSigma;
        double fInterceptSigma;
        double fStartFrequencySigma;
        double fEndFrequencySigma;


        LineRef(const double& InitialSlope);
        void InsertPoint(const Point& Point, const double& new_trimming_limits);
        void LineTrimming(const double& TrimminFactor, const unsigned& MinPoints);
        void CalculateSlope();
        void UpdateLineParameters();
        void FinishTrack();
        void Clear();
        virtual ~LineRef();

        bool operator < (const LineRef& str) const
        {
            return (fStartTimeInRunC < str.fStartTimeInRunC);
        }
    };



} /* namespace Katydid */

#endif /* SOURCE_DATA_SPECTRUM_ANALYSIS_KTSEQLINE_HH_ */


