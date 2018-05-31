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
#include "KTDiscriminatedPoint.hh"

#include <string>
#include <vector>
#include <set>

namespace Katydid
{
    class KTSequentialLineData : public Nymph::KTExtensibleData< KTSequentialLineData >
    {

        private:

            KTDiscriminatedPoints fLinePoints;
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

            const KTDiscriminatedPoints& GetPoints() const;
            KTDiscriminatedPoints& GetPoints();
            //LineRef(const double& initialSlope);
            void AddPoint(const KTDiscriminatedPoint& point);
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

    inline const KTDiscriminatedPoints& KTSequentialLineData::GetPoints() const
    {
        return fLinePoints;
    }

    inline KTDiscriminatedPoints& KTSequentialLineData::GetPoints()
    {
        return fLinePoints;
    }

} /* namespace Katydid */

#endif /* KTSEQUENTIALLINEDATA_HH_ */


