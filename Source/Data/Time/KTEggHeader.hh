/*
 * KTEggHeader.hh
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGHEADER_HH_
#define KTEGGHEADER_HH_

#include "Data.hh"

#include "MemberVariable.hh"

#include <string>
#include <vector>

namespace Katydid
{
    
    class KTChannelHeader
    {
        public:
            enum TimeSeriesDataType
            {
                kUnknown,
                kReal,
                kComplex,
                kIQ
            };

        public:
            KTChannelHeader();
            KTChannelHeader(const KTChannelHeader& orig);
            ~KTChannelHeader();

            KTChannelHeader& operator=(const KTChannelHeader& rhs);

            MEMVAR( unsigned, Number );
            MEMVAR( std::string, Source );
            MEMVAR(std::size_t, RawSliceSize); /// Number of bins per slice before any modification
            MEMVAR(std::size_t, SliceSize); /// Number of bins per slice after any initial modification (e.g. by the DAC)
            MEMVAR(std::size_t, SliceStride); /// Number of bins between slices
            MEMVAR(std::size_t, RecordSize); /// Number of bins per Monarch record
            MEMVAR( unsigned, SampleSize );
            MEMVAR( unsigned, DataTypeSize );
            MEMVAR( unsigned, DataFormat );
            MEMVAR( unsigned, BitDepth );
            MEMVAR( unsigned, BitAlignment );
            MEMVAR( double, VoltageOffset );
            MEMVAR( double, VoltageRange );
            MEMVAR( double, DACGain );
            MEMVAR(TimeSeriesDataType, TSDataType );
    };


    class KTEggHeader : public Nymph::Data
    {
        public:
            KTEggHeader();
            KTEggHeader(const KTEggHeader& orig);
            ~KTEggHeader();

            KTEggHeader& operator=(const KTEggHeader& rhs);

            MEMVAR_REF(std::string, Filename);
            MEMVAR_REF(std::string, MetadataFilename);
            MEMVAR(unsigned, AcquisitionMode);
            MEMVAR(unsigned, RunDuration);  /// in ms
            MEMVAR(double, AcquisitionRate); /// in Hz
            MEMVAR(double, CenterFrequency); /// in Hz
            MEMVAR(double, MinimumFrequency); /// in Hz
            MEMVAR(double, MaximumFrequency); /// in Hz
            MEMVAR_REF(std::string, Timestamp);
            MEMVAR_REF(std::string, Description);

            unsigned GetNChannels() const;
            KTEggHeader& SetNChannels(unsigned nChannels);

            const KTChannelHeader* GetChannelHeader(unsigned component = 0) const;
            KTChannelHeader* GetChannelHeader(unsigned component = 0);
            void SetChannelHeader(KTChannelHeader* chHeader, unsigned component = 0);

        private:
            std::vector< KTChannelHeader* > fChannelHeaders;
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
