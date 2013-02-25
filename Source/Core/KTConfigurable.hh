/*
 * KTConfigurable.hh
 *
 *  Created on: Aug 21, 2012
 *      Author: nsoblath
 */

#ifndef KTCONFIGURABLE_HH_
#define KTCONFIGURABLE_HH_

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTCommandLineHandler;
    class KTPStoreNode;

    class KTConfigurable
    {
        public:
            KTConfigurable(const std::string& name="default-name");
            virtual ~KTConfigurable();

            /// Should perform parameter store and command-line configurations
            virtual Bool_t Configure(const KTPStoreNode* node) = 0;
            /// Implement the option for calling Configure without passing a parameter store node.
            Bool_t Configure();

        protected:
            KTCommandLineHandler* fCLHandler;

        public:
            const std::string& GetConfigName() const;
            void SetConfigName(const std::string& name);

        protected:
            std::string fConfigName;

    };

    inline Bool_t KTConfigurable::Configure()
    {
        return Configure(NULL);
    }

    inline const std::string& KTConfigurable::GetConfigName() const
    {
        return fConfigName;
    }

    inline void KTConfigurable::SetConfigName(const std::string& name)
    {
        fConfigName = name;
    }

} /* namespace Katydid */
#endif /* KTCONFIGURABLE_HH_ */
