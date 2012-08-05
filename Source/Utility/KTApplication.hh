/*
 * KTApplication.hh
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#ifndef KTAPPLICATION_HH_
#define KTAPPLICATION_HH_

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTPStoreNode;
    class KTCommandLineHandler;
    class KTParameterStore;

    class KTApplication
    {
        public:
            KTApplication();
            KTApplication(int argC, char** argV);
            virtual ~KTApplication();

            void ProcessCommandLine();

            Bool_t ReadConfigFile(const std::string& filename);
            KTPStoreNode* GetNode(const std::string address) const;

        public:
            KTCommandLineHandler* GetCommandLineHandler() const;
            KTParameterStore* GetParameterStore() const;

        protected:
            KTCommandLineHandler* fCLHandler;
            KTParameterStore* fParamStore;
    };

} /* namespace Katydid */
#endif /* KTAPPLICATION_HH_ */
