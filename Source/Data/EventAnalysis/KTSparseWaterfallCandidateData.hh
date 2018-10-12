/*
 * KTSparseWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTSPARSEWATERFALLCANDIDATEDATA_HH_
#define KTSPARSEWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"
#include "KTDiscriminatedPoint.hh"

#include "KTMemberVariable.hh"

#include <set>

namespace Katydid
{
    class KTSparseWaterfallCandidateData : public Nymph::KTExtensibleData< KTSparseWaterfallCandidateData >
    {

        public:
            KTSparseWaterfallCandidateData();
            virtual ~KTSparseWaterfallCandidateData();

            const KTDiscriminatedPoints& GetPoints() const;
            KTDiscriminatedPoints& GetPoints();

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, CandidateID);

            //MEMBERVARIABLE(unsigned, NTimeBins);
            //MEMBERVARIABLE(double, TimeBinWidth);

            //MEMBERVARIABLE(unsigned, NFreqBins);
            //MEMBERVARIABLE(double, FreqBinWidth);

            MEMBERVARIABLE(double, TimeInRunC);
            MEMBERVARIABLE(double, TimeLength);
            MEMBERVARIABLE(double, TimeInAcq);
            //MEMBERVARIABLE(uint64_t, FirstSliceNumber);
            //MEMBERVARIABLE(uint64_t, LastSliceNumber);
            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);
            //MEMBERVARIABLE(double, MeanStartFrequency);
            //MEMBERVARIABLE(double, MeanEndFrequency);
            MEMBERVARIABLE(double, FrequencyWidth);

            //MEMBERVARIABLE(unsigned, StartRecordNumber);
            //MEMBERVARIABLE(unsigned, StartSampleNumber);
            //MEMBERVARIABLE(unsigned, EndRecordNumber);
            //MEMBERVARIABLE(unsigned, EndSampleNumber);

            void AddPoint(const KTDiscriminatedPoint& point);

        private:
            KTDiscriminatedPoints fPoints;

        public:
            static const std::string sName;

    };

    inline const KTDiscriminatedPoints& KTSparseWaterfallCandidateData::GetPoints() const
    {
        return fPoints;
    }

    inline KTDiscriminatedPoints& KTSparseWaterfallCandidateData::GetPoints()
    {
        return fPoints;
    }

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
