/*
 * KTSequentialLineData.hh
 *
 *  Created on: Sep 8, 2016
 *      Author: Christine
 */

#ifndef KTSEQUENTIALLINEDATA_HH_
#define KTSEQUENTIALLINEDATA_HH_

#include "KTMemberVariable.hh"
#include "KTData.hh"

#include <string>
#include <vector>
#include <set>

namespace Katydid
{
    class KTSequentialLineData : public Nymph::KTExtensibleData< KTSequentialLineData >
    {
        public:
            // this is a point from a power spectrum slice
            // sorting it will sort it by power
            struct Point
            {
                double fBinInSlice;
                double fFrequency;
                double fTimeInAcq;
                double fTimeInRunC;
                double fAmplitude;
                double fThreshold;
                double fMean;
                double fVariance;
                double fNeighborhoodAmplitude;
                uint64_t fAcquisitionID;
                double fComponent;

                Point(unsigned binInSclice, double pointFreq, double timeInAcq, double timeInRunC, double amplitude, double threshold, double mean, double variance, double neighborhoodamplitude, uint64_t acqID, unsigned iComponent) :
                    fBinInSlice(binInSclice),
                    fFrequency(pointFreq),
                    fTimeInAcq(timeInAcq),
                    fTimeInRunC(timeInRunC),
                    fAmplitude(amplitude),
                    fThreshold(threshold),
                    fMean(mean),
                    fVariance(variance),
                    fNeighborhoodAmplitude(neighborhoodamplitude),
                    fAcquisitionID(acqID),
                    fComponent(iComponent)
                {}

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
            struct PointCompare
            {
                bool operator() (const Point& lhs, const Point& rhs) const
                {
                    return lhs.fTimeInRunC < rhs.fTimeInRunC || (lhs.fTimeInRunC == rhs.fTimeInRunC && lhs.fFrequency < rhs.fFrequency);
                }
            };

            typedef std::set< Point, PointCompare > Points;

        private:

            std::vector<double> fTrimmingLimits;
            //std::vector<Point> fLinePoints;
            Points fLinePoints;
            std::vector<double> fAmplitudeList;
            std::vector<double> fSNRList;

            //Points fPoints;

        public:

            static const std::string sName;

            MEMBERVARIABLE(double, StartTimeInRunC);
            MEMBERVARIABLE(double, EndTimeInRunC);
            MEMBERVARIABLE(double, StartTimeInAcq);
            MEMBERVARIABLE(double, EndTimeInAcq);
            MEMBERVARIABLE(double, StartFrequency);
            MEMBERVARIABLE(double, EndFrequency);
            MEMBERVARIABLE(double, InitialSlope);
            MEMBERVARIABLE(double, Slope);
            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(unsigned, CandidateID)
            MEMBERVARIABLE(double, AmplitudeSum);
            MEMBERVARIABLE(double, SNRSum);
            MEMBERVARIABLE(double, NUPSum);
            MEMBERVARIABLE(unsigned, NPoints);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, MinPoints);
            MEMBERVARIABLE(double, StartFrequencySigma);
            MEMBERVARIABLE(double, EndFrequencySigma);

            double fSumX;
            double fSumY;
            double fSumXY;
            double fSumXX;

            KTSequentialLineData();
            virtual ~KTSequentialLineData();

            const KTSequentialLineData::Points& GetPoints() const;
            KTSequentialLineData::Points& GetPoints();
            //LineRef(const double& initialSlope);
            void AddPoint(const Point& point);
            void LineTrimming(const double& trimmingFactor, const unsigned& minPoints);
            void LineSNRTrimming(const double& trimmingThreshold, const unsigned& minPoints);
            void UpdateLineProperties();
            void FinishTrack();
            //virtual ~LineRef();

            /*bool operator < (const LineRef& str) const
            {
                return (fStartTimeInRunC < str.fStartTimeInRunC);
            }*/
    };

    inline const KTSequentialLineData::Points& KTSequentialLineData::GetPoints() const
    {
        return fLinePoints;
    }

    inline KTSequentialLineData::Points& KTSequentialLineData::GetPoints()
    {
        return fLinePoints;
    }
    /*void KTSequentialLineData::AddPoint(const Point& point)
    {
        //Points.insert(point);
        return;
    }*/
} /* namespace Katydid */

#endif /* KTSEQUENTIALLINEDATA_HH_ */


