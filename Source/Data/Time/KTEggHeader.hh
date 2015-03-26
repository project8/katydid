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

#include "M2Types.hh"

#include <string>
#include <vector>

namespace Katydid
{
    class KTChannelHeader
    {
        public:
            KTChannelHeader();
            KTChannelHeader(const KTChannelHeader& orig);
            ~KTChannelHeader();

            KTChannelHeader& operator=(const KTChannelHeader& rhs);

            MEMBERVARIABLE( unsigned, Number );
            MEMBERVARIABLE( std::string, Source );
            MEMBERVARIABLE(std::size_t, RawSliceSize); /// Number of bins per slice before any modification
            MEMBERVARIABLE(std::size_t, SliceSize); /// Number of bins per slice after any initial modification (e.g. by the DAC)
            MEMBERVARIABLE(std::size_t, SliceStride); /// Number of bins between slices
            MEMBERVARIABLE(std::size_t, RecordSize); /// Number of bins per Monarch record
            MEMBERVARIABLE( unsigned, SampleSize );
            MEMBERVARIABLE( unsigned, DataTypeSize );
            MEMBERVARIABLE( unsigned, DataFormat );
            MEMBERVARIABLE( unsigned, BitDepth );
            MEMBERVARIABLE( double, VoltageMin );
            MEMBERVARIABLE( double, VoltageRange );
            MEMBERVARIABLE( double, DACGain );
    };


    class KTEggHeader : public KTExtensibleData< KTEggHeader >
    {
        public:
            KTEggHeader();
            KTEggHeader(const KTEggHeader& orig);
            ~KTEggHeader();

            KTEggHeader& operator=(const KTEggHeader& rhs);

            MEMBERVARIABLEREF(std::string, Filename);
            MEMBERVARIABLE(unsigned, AcquisitionMode);
            MEMBERVARIABLE(unsigned, RunDuration);
            MEMBERVARIABLE(double, AcquisitionRate); /// in Hz
            MEMBERVARIABLE(double, CenterFrequency);
            MEMBERVARIABLE(double, MinimumFrequency);
            MEMBERVARIABLE(double, MaximumFrequency);
            MEMBERVARIABLEREF(std::string, Timestamp);
            MEMBERVARIABLEREF(std::string, Description);
            MEMBERVARIABLE(monarch2::RunType, RunType);
            MEMBERVARIABLE(monarch2::RunSourceType, RunSource);
            MEMBERVARIABLE(monarch2::FormatModeType, FormatMode);
            //MEMBERVARIABLE(unsigned, DataTypeSize); /// in bytes
            //MEMBERVARIABLE(unsigned, BitDepth); /// in bits
            //MEMBERVARIABLE(double, VoltageMin); /// in V
            //MEMBERVARIABLE(double, VoltageRange); /// in V

            unsigned GetNChannels() const;
            KTEggHeader& SetNChannels(unsigned nChannels);

            const KTChannelHeader* GetChannelHeader(unsigned component = 0) const;
            KTChannelHeader* GetChannelHeader(unsigned component = 0);
            void SetChannelHeader(KTChannelHeader* chHeader, unsigned component = 0);

        private:
            std::vector< KTChannelHeader* > fChannelHeaders;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTChannelHeader& header);
    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header);


    inline unsigned KTEggHeader::GetNChannels() const
    {
        return fChannelHeaders.size();
    }

    inline const KTChannelHeader* KTEggHeader::GetChannelHeader(unsigned component) const
    {
        return fChannelHeaders[component];
    }

    inline KTChannelHeader* KTEggHeader::GetChannelHeader(unsigned component)
    {
        return fChannelHeaders[component];
    }

    inline void KTEggHeader::SetChannelHeader(KTChannelHeader* header, unsigned component)
    {
        if (component >= fChannelHeaders.size()) SetNChannels(component+1);
        fChannelHeaders[component] = header;
        return;
    }

} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
