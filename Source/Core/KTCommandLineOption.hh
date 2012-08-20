/*
 * KTCommandLineOption.hh
 *
 *  Created on: Aug 1, 2012
 *      Author: nsoblath
 */

#ifndef KTCOMMANDLINEOPTION_H_
#define KTCOMMANDLINEOPTION_H_

#include "KTCommandLineHandler.hh"

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    //class KTCommandLineHandler;

    template< typename XInputType >
    class KTCommandLineOption
    {
        public:
            KTCommandLineOption();
            KTCommandLineOption(const std::string& group, const std::string& helpMsg, const std::string& longOpt, Char_t shortOpt='#');
            virtual ~KTCommandLineOption();

        protected:
            KTCommandLineHandler* fCLHandler;

    };

    template< typename XInputType >
    KTCommandLineOption< XInputType >::KTCommandLineOption() :
            fCLHandler(KTCommandLineHandler::GetInstance())
    {
    }

    template< typename XInputType >
    KTCommandLineOption< XInputType >::KTCommandLineOption(const std::string& group, const std::string& helpMsg, const std::string& longOpt, Char_t shortOpt) :
            fCLHandler(KTCommandLineHandler::GetInstance())
    {
        fCLHandler->AddOption< XInputType >(group, helpMsg, longOpt, shortOpt);
    }

    template< typename XInputType >
    KTCommandLineOption< XInputType >::~KTCommandLineOption()
    {
    }


    template< >
    class KTCommandLineOption< bool >
    {
        public:
            KTCommandLineOption() :
                fCLHandler(KTCommandLineHandler::GetInstance())
            {
            }
            KTCommandLineOption(const std::string& group, const std::string& helpMsg, const std::string& longOpt, Char_t shortOpt='#') :
                fCLHandler(KTCommandLineHandler::GetInstance())
            {
                fCLHandler->AddOption(group, helpMsg, longOpt, shortOpt);
            }
            virtual ~KTCommandLineOption()
            {
            }

        protected:
            KTCommandLineHandler* fCLHandler;

    };

} /* namespace Katydid */

#endif /* KTCOMMANDLINEOPTION_H_ */
