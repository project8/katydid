/*
 * KTWaterfallCandidateData.hh
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "stdint.h"

#ifndef KTWATERFALLCANDIDATEDATA_HH_
#define KTWATERFALLCANDIDATEDATA_HH_

#include "KTData.hh"

#include "KTTimeFrequency.hh"
#include "KTMemberVariable.hh"

#include <inttypes.h>

namespace Katydid
{
    class KTTimeFrequency;

    class KTWaterfallCandidateData : public Nymph::KTExtensibleData< KTWaterfallCandidateData >
    {
        public:
            KTWaterfallCandidateData();
            virtual ~KTWaterfallCandidateData();

            static const std::string sName;

        MEMBERVARIABLE(KTTimeFrequency*, Candidate);
        MEMBERVARIABLE(unsigned, Component);

        MEMBERVARIABLE(double, TimeInRun);
        MEMBERVARIABLE(double, TimeLength);
        MEMBERVARIABLE(uint64_t, FirstSliceNumber);
        MEMBERVARIABLE(uint64_t, LastSliceNumber);
        MEMBERVARIABLE(double, MinFrequency);
        MEMBERVARIABLE(double, MaxFrequency);
        MEMBERVARIABLE(double, MeanStartFrequency);
        MEMBERVARIABLE(double, MeanEndFrequency);
        MEMBERVARIABLE(double, FrequencyWidth);

        MEMBERVARIABLE(unsigned, StartRecordNumber);
        MEMBERVARIABLE(unsigned, StartSampleNumber);
        MEMBERVARIABLE(unsigned, EndRecordNumber);
        MEMBERVARIABLE(unsigned, EndSampleNumber);

};

} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATEDATA_HH_ */
