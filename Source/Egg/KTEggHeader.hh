/*
 * KTEggHeader.hh
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGHEADER_HH_
#define KTEGGHEADER_HH_

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

        public:
            void SetFilename(const std::string& fname);
            const std::string& GetFilename() const;

            void SetAcquisitionMode(UInt_t mode);
            UInt_t GetAcquisitionMode() const;

            void SetNChannels(UInt_t channels);
            UInt_t GetNChannels() const;

            void SetRecordSize(std::size_t recsize);
            std::size_t GetRecordSize() const;

            void SetMonarchRecordSize(std::size_t mrecsize);
            std::size_t GetMonarchRecordSize() const;

            void SetAcquisitionTime(UInt_t acqt);
            UInt_t GetAcquisitionTime() const;

            void SetAcquisitionRate(Double_t acqr);
            Double_t GetAcquisitionRate() const;

        protected:
            std::string fFilename;
            UInt_t fAcquisitionMode;
            UInt_t fNChannels;
            std::size_t fMonarchRecordSize;
            std::size_t fRecordSize;
            UInt_t fAcquisitionTime;
            Double_t fAcquisitionRate;

    };

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

    inline void KTEggHeader::SetRecordSize(std::size_t recsize)
    {
        fRecordSize = recsize;
        return;
    }

    inline std::size_t KTEggHeader::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTEggHeader::SetMonarchRecordSize(std::size_t mrecsize)
    {
        fMonarchRecordSize = mrecsize;
        return;
    }

    inline std::size_t KTEggHeader::GetMonarchRecordSize() const
    {
        return fMonarchRecordSize;
    }

    inline void KTEggHeader::SetAcquisitionTime(UInt_t acqt)
    {
        fAcquisitionTime = acqt;
        return;
    }

    inline UInt_t KTEggHeader::GetAcquisitionTime() const
    {
        return fAcquisitionTime;
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


} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
