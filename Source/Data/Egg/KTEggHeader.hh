/*
 * KTEggHeader.hh
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGHEADER_HH_
#define KTEGGHEADER_HH_

#include "MonarchTypes.hpp"

#include "Rtypes.h"

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

            void SetAcquisitionMode(UInt_t mode);
            UInt_t GetAcquisitionMode() const;

            void SetNChannels(UInt_t channels);
            UInt_t GetNChannels() const;

            void SetSliceSize(std::size_t recsize);
            std::size_t GetSliceSize() const;

            void SetRecordSize(std::size_t mrecsize);
            std::size_t GetRecordSize() const;

            void SetRunDuration(UInt_t acqt);
            UInt_t GetRunDuration() const;

            void SetAcquisitionRate(Double_t acqr);
            Double_t GetAcquisitionRate() const;

            void SetTimestamp( const std::string& aTimestamp );
            const std::string& GetTimestamp() const;

            void SetDescription( const std::string& aDescription );
            const std::string& GetDescription() const;

            void SetRunType( RunType aRunType );
            RunType GetRunType() const;

            void SetRunSource( RunSourceType aRunSource );
            RunSourceType GetRunSource() const;

            void SetFormatMode( FormatModeType aFormatMode );
            FormatModeType GetFormatMode() const;

        protected:
            std::string fFilename;
            UInt_t fAcquisitionMode;
            UInt_t fNChannels;
            std::size_t fSliceSize; /// Number of bins per record-written-to-disk
            std::size_t fRecordSize; /// Number of bins per Katydid record
            UInt_t fRunDuration;
            Double_t fAcquisitionRate; /// in Hz
            std::string fTimestamp;
            std::string fDescription;
            RunType fRunType;
            RunSourceType fRunSource;
            FormatModeType fFormatMode;

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

    inline void KTEggHeader::SetAcquisitionMode(UInt_t mode)
    {
        fAcquisitionMode = mode;
        return;
    }

    inline UInt_t KTEggHeader::GetAcquisitionMode() const
    {
        return fAcquisitionMode;
    }

    inline void KTEggHeader::SetNChannels(UInt_t channels)
    {
        fNChannels = channels;
        return;
    }

    inline UInt_t KTEggHeader::GetNChannels() const
    {
        return fNChannels;
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

    inline void KTEggHeader::SetRunDuration(UInt_t acqt)
    {
        fRunDuration = acqt;
        return;
    }

    inline UInt_t KTEggHeader::GetRunDuration() const
    {
        return fRunDuration;
    }

    inline void KTEggHeader::SetAcquisitionRate(Double_t acqr)
    {
        fAcquisitionRate = acqr;
        return;
    }

    inline Double_t KTEggHeader::GetAcquisitionRate() const
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

    inline void KTEggHeader::SetRunType( RunType aRunType )
    {
        fRunType = aRunType;
        return;
    }

    inline RunType KTEggHeader::GetRunType() const
    {
        return fRunType;
    }

    inline void KTEggHeader::SetRunSource( RunSourceType aRunSource )
    {
        fRunSource = aRunSource;
        return;
    }

    inline RunSourceType KTEggHeader::GetRunSource() const
    {
        return fRunSource;
    }

    inline void KTEggHeader::SetFormatMode( FormatModeType aFormatMode )
    {
        fFormatMode = aFormatMode;
        return;
    }

    inline FormatModeType KTEggHeader::GetFormatMode() const
    {
        return fFormatMode;
    }



} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
