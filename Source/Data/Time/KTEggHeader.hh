/*
 * KTEggHeader.hh
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGHEADER_HH_
#define KTEGGHEADER_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include "MonarchTypes.hpp"

#include <string>

namespace Katydid
{
    class KTEggHeader : public KTExtensibleData< KTEggHeader >
    {
        public:
            enum TimeSeriesDataType
            {
                kReal,
                kComplex,
                kIQ
            };

        public:
            KTEggHeader();
            KTEggHeader(const KTEggHeader& orig);
            ~KTEggHeader();

            KTEggHeader& operator=(const KTEggHeader& rhs);

            MEMBERVARIABLEREF(std::string, Filename);
            MEMBERVARIABLE(double, CenterFrequency);
            MEMBERVARIABLE(double, MinimumFrequency);
            MEMBERVARIABLE(double, MaximumFrequency);
            MEMBERVARIABLE(unsigned, AcquisitionMode);
            MEMBERVARIABLE(unsigned, NChannels);
            MEMBERVARIABLE(std::size_t, RawSliceSize); /// Number of bins per slice before any modification
            MEMBERVARIABLE(std::size_t, SliceSize); /// Number of bins per slice after any initial modification (e.g. by the DAC)
            MEMBERVARIABLE(std::size_t, SliceStride); /// Number of bins between slices
            MEMBERVARIABLE(std::size_t, RecordSize); /// Number of bins per Monarch record
            MEMBERVARIABLE(unsigned, RunDuration);
            MEMBERVARIABLE(double, AcquisitionRate); /// in Hz
            MEMBERVARIABLEREF(std::string, Timestamp);
            MEMBERVARIABLEREF(std::string, Description);
            MEMBERVARIABLE(monarch::RunType, RunType);
            MEMBERVARIABLE(monarch::RunSourceType, RunSource);
            MEMBERVARIABLE(monarch::FormatModeType, FormatMode);
            MEMBERVARIABLE(unsigned, DataTypeSize); /// in bytes
            MEMBERVARIABLE(unsigned, BitDepth); /// in bits
            MEMBERVARIABLE(double, VoltageMin); /// in V
            MEMBERVARIABLE(double, VoltageRange); /// in V
            MEMBERVARIABLE(TimeSeriesDataType, TSDataType);

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header);

} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
