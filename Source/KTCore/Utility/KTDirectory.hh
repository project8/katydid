/*
 * KTDirectory.hh
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#ifndef KTDIRECTORY_HH_
#define KTDIRECTORY_HH_

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
            bool GetPathExists() const;
            Access GetAccess() const;

            bool SetPath(const std::string& strPath);

        protected:
            boost::filesystem::path fPath;
            bool fPathExists;
            Access fAccess;

    };

} /* namespace Katydid */
#endif /* KTDIRECTORY_HH_ */
