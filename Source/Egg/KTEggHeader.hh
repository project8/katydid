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

class MonarchHeader;

namespace Katydid
{
    class KTEggHeader
    {
        public:
            KTEggHeader();
            ~KTEggHeader();

            // Member access functions
        public:
            void SetFilename(std::string fname);
            std::string GetFilename() const;

            void SetAcquisitionMode(const AcquisitionMode& mode);
            AcquisitionMode GetAcquisitionMode() const;

            void SetRecordSize(const std::size_t& recsize);
            std::size_t GetRecordSize() const;

            void SetAcquisitionTime(const std::size_t& acqt);
            std::size_t GetAcquisitionTime() const;

            void SetAcquisitionRate(const std::size_t& acqr);
            std::size_t GetAcquisitionRate() const;

            // Access to the MonarchHeader object
            /// Copies the information from the MonarchHeader object
            void TakeInformation(MonarchHeader* header);

        protected:
            std::string fFilename;
            AcquisitionMode fAcquisitionMode;
            std::size_t fRecordSize;
            std::size_t fAcquisitionTime;
            std::size_t fAcquisitionRate;

    };

    inline void KTEggHeader::SetFilename(std::string fname)
    {
        fFilename = fname;
        return;
    }

    inline std::string KTEggHeader::GetFilename() const
    {
        return fFilename;
    }

    inline void KTEggHeader::SetAcquisitionMode(const AcquisitionMode& mode)
    {
        fAcquisitionMode = mode;
        return;
    }

    inline AcquisitionMode KTEggHeader::GetAcquisitionMode() const
    {
        return fAcquisitionMode;
    }

    inline void KTEggHeader::SetRecordSize(const std::size_t& recsize)
    {
        fRecordSize = recsize;
        return;
    }

    inline std::size_t KTEggHeader::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTEggHeader::SetAcquisitionTime(const std::size_t& acqt)
    {
        fAcquisitionTime = acqt;
        return;
    }

    inline std::size_t KTEggHeader::GetAcquisitionTime() const
    {
        return fAcquisitionTime;
    }

    inline void KTEggHeader::SetAcquisitionRate(const std::size_t& acqr)
    {
        fAcquisitionRate = acqr;
        return;
    }

    inline std::size_t KTEggHeader::GetAcquisitionRate() const
    {
        return fAcquisitionRate;
    }


} /* namespace Katydid */
#endif /* KTEGGHEADER_HH_ */
