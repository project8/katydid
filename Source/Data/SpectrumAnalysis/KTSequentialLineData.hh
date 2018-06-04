/**
 @file KTSequentialLineData.hh
 @brief Contains KTSequentialLineData
 @details KTDiscriminatedPoint cluster with some track properties
 @author: C. Claessens
 @date: May 31, 2018
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

        public:

            static const std::string sName;

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


