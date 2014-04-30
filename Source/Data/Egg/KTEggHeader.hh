/*
 * KTEggHeader.hh
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGHEADER_HH_
#define KTEGGHEADER_HH_

#include "MonarchTypes.hpp"

#include <string>

namespace Katydid
{
    class KTEggHeader
    {
        public:
            KTEggHeader();
            KTEggHeader(const KTEggHeader& orig);
            ~KTEggHeader();

            KTEggHeader& operator=(const KTEggHeader& rhs);

        public:
            void SetFilename(const std::string& fname);
            const std::string& GetFilename() const;

            void SetAcquisitionMode(unsigned mode);
            unsigned GetAcquisitionMode() const;

            void SetNChannels(unsigned channels);
            unsigned GetNChannels() const;

            void SetRawSliceSize(std::size_t size);
            std::size_t GetRawSliceSize() const;

            void SetSliceSize(std::size_t size);
            std::size_t GetSliceSize() const;

            void SetRecordSize(std::size_t recsize);
            std::size_t GetRecordSize() const;

            void SetRunDuration(unsigned acqt);
            unsigned GetRunDuration() const;

            void SetAcquisitionRate(double acqr);
            double GetAcquisitionRate() const;

            void SetTimestamp( const std::string& aTimestamp );
            const std::string& GetTimestamp() const;

            void SetDescription( const std::string& aDescription );
            const std::string& GetDescription() const;

            void SetRunType( monarch::RunType aRunType );
            monarch::RunType GetRunType() const;

            void SetRunSource( monarch::RunSourceType aRunSource );
            monarch::RunSourceType GetRunSource() const;

            void SetFormatMode( monarch::FormatModeType aFormatMode );
            monarch::FormatModeType GetFormatMode() const;

            void SetDataTypeSize(unsigned aSize);
            unsigned GetDataTypeSize() const;

            void SetBitDepth(unsigned aBD);
            unsigned GetBitDepth() const;

            void SetVoltageMin(double aVoltage);
            double GetVoltageMin() const;

            void SetVoltageRange(double aVoltage);
            double GetVoltageRange() const;

        protected:
            std::string fFilename;
            unsigned fAcquisitionMode;
            unsigned fNChannels;
            std::size_t fRawSliceSize; /// Number of bins per slice before any modification
            std::size_t fSliceSize; /// Number of bins per slice after any initial modification (e.g. by the DAC)
            std::size_t fRecordSize; /// Number of bins per Monarch record
            unsigned fRunDuration;
            double fAcquisitionRate; /// in Hz
            std::string fTimestamp;
            std::string fDescription;
            monarch::RunType fRunType;
            monarch::RunSourceType fRunSource;
            monarch::FormatModeType fFormatMode;
            unsigned fDataTypeSize; /// in bytes
            unsigned fBitDepth; /// in bits
            double fVoltageMin; /// in V
            double fVoltageRange; /// in V

    };

    std::ostream& operator<<(std::ostream& out, const KTEggHeader& header);

    inline void KTEggHeader::SetFilename(const std::string& fname)
    {
        fFilename = fname;
        return;
    }

    inline const std::string& KTEggHeader::GetFilename() const
    {
        return fFilename;
    }

    inline void KTEggHeader::SetAcquisitionMode(unsigned mode)
    {
        fAcquisitionMode = mode;
        return;
    }

    inline unsigned KTEggHeader::GetAcquisitionMode() const
    {
        return fAcquisitionMode;
    }

    inline void KTEggHeader::SetNChannels(unsigned channels)
    {
        fNChannels = channels;
        return;
    }

    inline unsigned KTEggHeader::GetNChannels() const
    {
        return fNChannels;
    }

    inline void KTEggHeader::SetRawSliceSize(std::size_t slicesize)
    {
        fRawSliceSize = slicesize;
        return;
    }

    inline std::size_t KTEggHeader::GetRawSliceSize() const
    {
        return fRawSliceSize;
    }

    inline void KTEggHeader::SetSliceSize(std::size_t slicesize)
    {
        fSliceSize = slicesize;
        return;
    }

    inline std::size_t KTEggHeader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTEggHeader::SetRecordSize(std::size_t recsize)
    {
        fRecordSize = recsize;
        return;
    }

    inline std::size_t KTEggHeader::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTEggHeader::SetRunDuration(unsigned acqt)
    {
        fRunDuration = acqt;
        return;
    }

    inline unsigned KTEggHeader::GetRunDuration() const
    {
        return fRunDuration;
    }

    inline void KTEggHeader::SetAcquisitionRate(double acqr)
    {
        fAcquisitionRate = acqr;
        return;
    }

    inline double KTEggHeader::GetAcquisitionRate() const
    {
        return fAcquisitionRate;
    }

    inline void KTEggHeader::SetTimestamp( const std::string& aTimestamp )
    {
        fTimestamp = aTimestamp;
        return;
    }

    inline const std::string& KTEggHeader::GetTimestamp() const
    {
        return fTimestamp;
    }

    inline void KTEggHeader::SetDescription( const std::string& aDescription )
    {
        fDescription = aDescription;
        return;
    }

    inline const std::string& KTEggHeader::GetDescription() const
    {
        return fDescription;
    }

    inline void KTEggHeader::SetRunType( monarch::RunType aRunType )
    {
        fRunType = aRunType;
        return;
    }

    inline monarch::RunType KTEggHeader::GetRunType() const
    {
        return fRunType;
    }

    inline void KTEggHeader::SetRunSource( monarch::RunSourceType aRunSource )
    {
        fRunSource = aRunSource;
        return;
    }

    inline monarch::RunSourceType KTEggHeader::GetRunSource() const
    {
        return fRunSource;
    }

    inline void KTEggHeader::SetFormatMode( monarch::FormatModeType aFormatMode )
    {
        fFormatMode = aFormatMode;
        return;
    }

    inline monarch::FormatModeType KTEggHeader::GetFormatMode() const
    {
        return fFormatMode;
    }

    inline void KTEggHeader::SetDataTypeSize(unsigned aSize)
    {
        fDataTypeSize = aSize;
        return;
    }

    inline unsigned KTEggHeader::GetDataTypeSize() const
    {
        return fDataTypeSize;
    }

    inline void KTEggHeader::SetBitDepth(unsigned aBD)
    {
        fBitDepth = aBD;
        return;
    }

    inline unsigned KTEggHeader::GetBitDepth() const
    {
        return fBitDepth;
    }

    inline void KTEggHeader::SetVoltageMin(double aVoltage)
    {
        fVoltageMin = aVoltage;
        return;
    }

    inline double KTEggHeader::GetVoltageMin() const
    {
        return fVoltageMin;
    }

    inline void KTEggHeader::SetVoltageRange(double aVoltage)
    {
        fVoltageRange = aVoltage;
        return;
    }

    inline double KTEggHeader::GetVoltageRange() const
    {
        return fVoltageRange;
    }


} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
