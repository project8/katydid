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


//using namespace Nymph;

    class KTSeqLine : public Nymph::KTExtensibleData< KTHoughData >
    {
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

                    bool operator > (const Point& str) const
                        {
                            return (fAmplitude > str.fAmplitude);
                        }
                    bool operator < (const Point& str) const
                        {
                            return (fAmplitude < str.fAmplitude);
                        }
                };

        public:


            KTSeqLine();
            virtual ~KTSeqLine();


            unsigned GetNPoints();
            //double GetLength();
            double GetStartFreq();
            //double GetSlope();
            double GetStartTime();
            double GetTimeInRunC();
            double GetTotalScore();
            //unsigned GetLineID();
            double GetStopTime();
            double GetEndFreq();
            //unsigned GetComponent();
            //uint64_t GetAcquisitionID();
            double GetAmplitudeSum();


            bool Configure(double& DeltaT, double& DeltaF);
            bool InvestigatePoint(Point& Point, double& new_trimming_limits);
            bool TrimEdges();
            bool CollectPoint(Point&, double& new_trimming_limits);

            double GetLineScore();

        private:
            //investigation parameters
            MEMBERVARIABLE(double, DeltaT);
            MEMBERVARIABLE(double, DeltaF);
            MEMBERVARIABLE(unsigned, Lambda);
            MEMBERVARIABLE(double, LineThreshold);
            MEMBERVARIABLE(double, Mu);
            MEMBERVARIABLE(int, Nu);

        public:
            //line properties
            MEMBERVARIABLE(double, LineSlope);
            MEMBERVARIABLE(double, LineScore);
            MEMBERVARIABLE(double, Length);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);

            MEMBERVARIABLE(bool, Active);
            MEMBERVARIABLE(bool, Collectable);

            MEMBERVARIABLE(unsigned, Identifier);

            //point lists

            std::vector<double> trimming_limits;
            std::vector<Point> fLinePoints;

    /*		public:
                KTScoredSpectrum& operator=(const KTScoredSpectrum& rhs);

                KTScoredSpectrum& Scale(double scale);
    */
    };

    struct LineRef
    {
        //std::set< TrackSetCIt, TrackSetCItComp > fTrackRefs;
        // Keep track of both the sum and the mean so that the mean can be updated regularly without an extra multiplication

        std::vector<double> trimming_limits;
        std::vector<KTSeqLine::Point> fLinePoints;

        double fStartTimeInRunC;
        double fEndTimeInRunC;
        double fStartFrequency;
        double fEndFrequency;
        double fSlope;
        double fLineScore;
        unsigned fNPoints;
        uint64_t fAcquisitionID;


        LineRef();
        void InsertPoint(const KTSeqLine::Point& Point, const double& new_trimming_limits);
        void LineTrimming(const double& TrimminFactor, const unsigned& MinPoints);
        void CalculateSlope();
        void Clear();

        bool operator < (const LineRef& str) const
        {
            return (fStartTimeInRunC < str.fStartTimeInRunC);
        }
    };
    void LineRef::InsertPoint(const KTSeqLine::Point& Point, const double& new_trimming_limits)
    {
        trimming_limits.push_back(new_trimming_limits);
        fLinePoints.push_back(Point);

        if (Point.fTimeInRunC < fStartTimeInRunC)
        {
            fStartTimeInRunC = Point.fTimeInRunC;
        }
        if (Point.fTimeInRunC > fEndTimeInRunC)
        {
            fEndTimeInRunC = Point.fTimeInRunC;
        }
        if (Point.fPointFreq < fStartFrequency)
        {
            fStartFrequency = Point.fPointFreq;
        }
        if (Point.fPointFreq > fEndFrequency)
        {
            fEndFrequency = Point.fPointFreq;
        }

        fLineScore += Point.fAmplitude;
        fNPoints = fLinePoints.size();
        fAcquisitionID = Point.fAcquisitionID;

        this->CalculateSlope();

    }

    inline void LineRef::CalculateSlope()
    {
        double weightedSlope;
        double wSum;

        for(std::vector<KTSeqLine::Point>::iterator PointIt = fLinePoints.begin(); PointIt != fLinePoints.end(); ++PointIt)
        {
            weightedSlope
        }

    }
    void LineRef::LineTrimming(const double& TrimminFactor, const unsigned& MinPoints)
    {

    }


 /*   struct LineComp
    {
        bool operator() (const LineRef& lhs, const LineRef& rhs)
        {
            if (lhs.fStartTimeInRunC != rhs.fStartTimeInRunC) return lhs.fStartTimeInRunC < rhs.fStartTimeInRunC;
                        return lhs.fEndTimeInRunC < rhs.fEndTimeInRunC;
        }

    };
*/

    double KTSeqLine::GetTotalScore()
    {
        double score;
        unsigned nPoints = this->GetNPoints;
        if (nPoints>0)
        {
            for(unsigned i=0; i<nPoints;i++)
            {
                score+=this->fLinePoints[i].fScore;
            }
            score=score/nPoints;
        }
        return score;
    }

    unsigned inline KTSeqLine::GetNPoints()
    {
        return fLinePoints.size();
    }
    /*double inline KTSeqLine::GetLength()
    {
        return fLength;
    }*/

    double inline KTSeqLine::GetStartFreq()
    {
        return fLinePoints[0].fPointFreq;
    }

    double inline KTSeqLine::GetStartTime()
    {
        return fLinePoints[0].fTimeInAcq;
    }
    double KTSeqLine::GetStopTime()
    {
        return fLinePoints.back().fTimeInAcq-this->GetStartTime();
    }
    double KTSeqLine::GetTimeInRunC()
    {
        return fLinePoints[0].fTimeInRunC;
    }

    /*double inline KTSeqLine::GetSlope()
    {
        return fLineSlope;
    }

    */
    double inline KTSeqLine::GetEndFreq()
    {
        return fLinePoints.back().fPointFreq;
    }
    /*unsigned inline KTSeqLine::GetComponent()
    {
        return fComponent;
    }
    uint64_t inline KTSeqLine::GetAcquisitionID()
    {
        return fAcquisitionID;
    }*/
    double inline KTSeqLine::GetAmplitudeSum()
    {
        double sum = 0.0;
        for (unsigned iPoint = 0; iPoint < fLinePoints.size(); iPoint++)
        {
            sum += fLinePoints[iPoint].fPower;
        }
        return sum;
    }




} /* namespace Katydid */

#endif /* SOURCE_DATA_TRANSFORM_KTSEQLINE_HH_ */


