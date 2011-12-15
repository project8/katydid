/*
 * KTCommandLineUser.hh
 *
 *  Created on: Nov 30, 2011
 *      Author: nsoblath
 */

#ifndef KTCOMMANDLINEUSER_H_
#define KTCOMMANDLINEUSER_H_

#include "KTCommandLineHandler.hh"

#include "Rtypes.h"

namespace Katydid
{
    //class KTCommandLineHandler;

    class KTCommandLineUser
    {
        public:
            friend class KTCommandLineHandler;

        public:
            KTCommandLineUser();
            virtual ~KTCommandLineUser();

        protected:
            KTCommandLineHandler* fCLHandler;

            //**************
            // Add options
            //**************

        public:
            virtual void AddCommandLineOptions() = 0;

        protected:
            Bool_t fCommandLineOptionsAdded;

            //**************
            // Use parsed options
            //**************

        public:
            virtual void UseParsedCommandLineImmediately() = 0;

        protected:
            Bool_t fCommandLineIsParsed;

    };

} /* namespace Katydid */

#endif /* KTCOMMANDLINEUSER_H_ */
