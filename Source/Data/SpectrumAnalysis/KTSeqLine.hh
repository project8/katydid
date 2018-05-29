/*
 * KTSeqLine.hh
 *
 *  Created on: Sep 8, 2016
 *      Author: Christine
 */

#ifndef KTSEQLINE_HH_
#define KTSEQLINE_HH_

#include "KTMemberVariable.hh"
//#include "KTSeqTrackCreator.hh"


#include <string>
#include <vector>

namespace Katydid {

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
        double fMean;
        double fVariance;
        double fNeighborhoodAmplitude;
        uint64_t fAcquisitionID;
        double fComponent;

        Point(unsigned binInSclice, double pointFreq, double timeInAcq, double timeInRunC, double amplitude, double threshold, double mean, double variance, double neighborhoodamplitude, uint64_t acqID, unsigned iComponent) : fBinInSlice(binInSclice), fPointFreq(pointFreq),
                fTimeInAcq(timeInAcq), fTimeInRunC(timeInRunC), fAmplitude(amplitude), fThreshold(threshold), fMean(mean), fVariance(variance), fNeighborhoodAmplitude(neighborhoodamplitude), fAcquisitionID(acqID), fComponent(iComponent) {}

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
/*    struct LinePoint
    {
        double fBinInSlice;
        double fPointFreq;
        double fTimeInAcq;
        double fTimeInRunC;
        double fAmplitude;
        double fThreshold;
        uint64_t fAcquisitionID;
        double fComponent;

        LinePoint(unsigned binInSclice, double pointFreq, double timeInAcq, double timeInRunC, double amplitude, double threshold, uint64_t acqID, unsigned iComponent) : fBinInSlice(binInSclice), fPointFreq(pointFreq),
                fTimeInAcq(timeInAcq), fTimeInRunC(timeInRunC), fAmplitude(amplitude), fThreshold(threshold), fAcquisitionID(acqID), fComponent(iComponent) {}
        void Clear();

        bool operator > (const Point& str) const
        {
            return (fTimeInRunC > str.fTimeInRunC);
        }
        bool operator < (const Point& str) const
        {
            return (fTimeInRunC < str.fTimeInRunC);
        }
    };*/

    struct LineRef
    {
        /*enum class slope_method
        {
            weighted_first_point_ref,
            weighted,
            unweighted
        };*/

        std::vector<double> fTrimmingLimits;
        std::vector<Point> fLinePoints;
        std::vector<double> fAmplitudeList;
        std::vector<double> fSNRList;

        double fStartTimeInRunC;
        double fEndTimeInRunC;
        double fStartTimeInAcq;
        double fEndTimeInAcq;
        double fStartFrequency;
        double fEndFrequency;
        double fInitialSlope;
        double fSlope;
        unsigned fComponent;
        double fAmplitudeSum;
        double fSNRSum;
        double fNUPSum;
        unsigned fNPoints;
        uint64_t fAcquisitionID;
        unsigned fMinPoints;
        double fStartFrequencySigma;
        double fEndFrequencySigma;

        double fSumX;
        double fSumY;
        double fSumXY;
        double fSumXX;


        LineRef(const double& initialSlope);
        void InsertPoint(const Point& point);
        void LineTrimming(const double& trimmingFactor, const unsigned& minPoints);
        void LineSNRTrimming(const double& trimmingThreshold, const unsigned& minPoints);
        //void (LineRef::*f_calc_slope_func)();
        //void CalculateSlope();
        //void CalculateNewSlope();
        void UpdateLineProperties();
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


