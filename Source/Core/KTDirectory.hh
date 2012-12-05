/*
 * KTDirectory.hh
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#ifndef KTDIRECTORY_HH_
#define KTDIRECTORY_HH_

#include "Rtypes.h"

#include <boost/filesystem.hpp>

#include <string>

namespace Katydid
{

    class KTDirectory
    {
        public:
            enum Access
            {
                eNone,
                eRead,
                eReadWrite
            };

        public:
            KTDirectory();
            virtual ~KTDirectory();

        public:
            const std::string& GetPath() const;
            Bool_t GetPathExists() const;
            Access GetAccess() const;

            Bool_t SetPath(const std::string& strPath);

        protected:
            boost::filesystem::path fPath;
            Bool_t fPathExists;
            Access fAccess;

    };

} /* namespace Katydid */
#endif /* KTDIRECTORY_HH_ */
