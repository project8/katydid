/*
 * KTDirectory.hh
 *
 *  Created on: Dec 3, 2012
 *      Author: nsoblath
 */

#ifndef KTDIRECTORY_HH_
#define KTDIRECTORY_HH_

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
            const std::string& GetPath();
            Access GetAccess();

        protected:
            std::string fPath;
            Access fAccess;

    };

} /* namespace Katydid */
#endif /* KTDIRECTORY_HH_ */
