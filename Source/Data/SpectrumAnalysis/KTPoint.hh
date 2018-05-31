/**
 @file KTPoint.hh
 @brief Defines the point structure
 @details Defines the basic point structure used for any discriminated point: used in KTSparseWaterfallCandidateData, KTTrackProcessing
 @author: M. Guigue
 @date: May 31, 2018
 */

#ifndef KTPOINT_HH_
#define KTPOINT_HH_

namespace Katydid
{
    struct KTPoint
    {
        double fTimeInRunC;
        double fFrequency;
        double fAmplitude;
        double fTimeInAcq;
        KTPoint(double tirc, double freq, double amp, double tiacq) : fTimeInRunC(tirc), fFrequency(freq), fAmplitude(amp), fTimeInAcq(tiacq) {}
    };
}
#endif