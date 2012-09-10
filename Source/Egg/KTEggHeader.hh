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
            ~KTEggHeader();

        public:
            void SetFilename(const std::string& fname);
            const std::string& GetFilename() const;

            void SetAcquisitionMode(UInt_t mode);
            UInt_t GetAcquisitionMode() const;

            void SetRecordSize(std::size_t recsize);
            std::size_t GetRecordSize() const;

            void SetAcquisitionTime(UInt_t acqt);
            UInt_t GetAcquisitionTime() const;

            void SetAcquisitionRate(Double_t acqr);
            Double_t GetAcquisitionRate() const;

        protected:
            std::string fFilename;
            UInt_t fAcquisitionMode;
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

    inline void KTEggHeader::SetRecordSize(std::size_t recsize)
    {
        fRecordSize = recsize;
        return;
    }

    inline std::size_t KTEggHeader::GetRecordSize() const
    {
        return fRecordSize;
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
