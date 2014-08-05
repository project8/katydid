/*
 * KTSparseWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#ifndef KTSPARSEWATERFALLCANDIDATEDATA_HH_
#define KTSPARSEWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include <set>

namespace Katydid
{
    class KTSparseWaterfallCandidateData : public KTExtensibleData< KTSparseWaterfallCandidateData >
    {
        public:
            struct Point
            {
                double fTimeInRunC;
                double fFrequency;
                double fAmplitude;
                Point(double tirc, double freq, double amp) : fTimeInRunC(tirc), fFrequency(freq), fAmplitude(amp) {}
            };

            struct PointCompare
            {
                bool operator() (const Point& lhs, const Point& rhs)
                {
                    return lhs.fTimeInRunC < rhs.fTimeInRunC || (lhs.fTimeInRunC == rhs.fTimeInRunC && lhs.fFrequency < rhs.fFrequency);
                }
            };

            typedef std::set< Point, PointCompare > Points;

        public:
            KTSparseWaterfallCandidateData();
            virtual ~KTSparseWaterfallCandidateData();

            const Points& GetPoints() const;
            Points& GetPoints();

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(unsigned, CandidateID);

            //MEMBERVARIABLE(unsigned, NTimeBins);
            MEMBERVARIABLE(double, TimeBinWidth);

            //MEMBERVARIABLE(unsigned, NFreqBins);
            MEMBERVARIABLE(double, FreqBinWidth);

            MEMBERVARIABLE(double, TimeInRunC);
            MEMBERVARIABLE(double, TimeLength);
            //MEMBERVARIABLE(uint64_t, FirstSliceNumber);
            //MEMBERVARIABLE(uint64_t, LastSliceNumber);
            MEMBERVARIABLE(double, MinimumFrequency);
            MEMBERVARIABLE(double, MaximumFrequency);
            //MEMBERVARIABLE(double, MeanStartFrequency);
            //MEMBERVARIABLE(double, MeanEndFrequency);
            MEMBERVARIABLE(double, FrequencyWidth);

            //MEMBERVARIABLE(unsigned, StartRecordNumber);
            //MEMBERVARIABLE(unsigned, StartSampleNumber);
            //MEMBERVARIABLE(unsigned, EndRecordNumber);
            //MEMBERVARIABLE(unsigned, EndSampleNumber);

            void AddPoint(const Point& point);

        protected:
            Points fPoints;
    };

    inline const KTSparseWaterfallCandidateData::Points& KTSparseWaterfallCandidateData::GetPoints() const
    {
        return fPoints;
    }

    inline KTSparseWaterfallCandidateData::Points& KTSparseWaterfallCandidateData::GetPoints()
    {
        return fPoints;
    }

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
