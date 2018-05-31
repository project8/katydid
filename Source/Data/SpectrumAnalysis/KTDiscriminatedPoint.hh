/**
 @file KTDiscriminatedPoint.hh
 @brief Defines the point structure
 @details Defines the basic point structure used for any discriminated point: used in KTSparseWaterfallCandidateData, KTTrackProcessing
 @author: M. Guigue
 @date: May 31, 2018
 */

#ifndef KTDISCRIMINATEDPOINT_HH_
#define KTDISCRIMINATEDPOINT_HH_

namespace Katydid
{
    struct KTDiscriminatedPoint
    {
        double fTimeInRunC;
        double fFrequency;
        double fAmplitude;
        double fTimeInAcq;
        KTDiscriminatedPoint(double tirc, double freq, double amp, double tiacq) : fTimeInRunC(tirc), fFrequency(freq), fAmplitude(amp), fTimeInAcq(tiacq) {}
    };
    struct KTDiscriminatedPointCompare
            {
                bool operator() (const KTDiscriminatedPoint& lhs, const KTDiscriminatedPoint& rhs) const
                {
                    return lhs.fTimeInRunC < rhs.fTimeInRunC || (lhs.fTimeInRunC == rhs.fTimeInRunC && lhs.fFrequency < rhs.fFrequency);
                }
            };

    typedef std::set< KTDiscriminatedPoint, KTDiscriminatedPointCompare > KTDiscriminatedPoints;
}
#endif