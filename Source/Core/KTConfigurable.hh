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
            KTConfigurable();
            virtual ~KTConfigurable();

            Bool_t Configure(const KTPStoreNode* node);

        protected:
            virtual Bool_t ConfigureFromPStore(const KTPStoreNode* node);
            virtual Bool_t ConfigureFromCL();

            KTCommandLineHandler* fCLHandler;

        public:
            const std::string& GetConfigName() const;
            void SetConfigName(const std::string& name);

        protected:
            std::string fConfigName;

    };

    inline Bool_t KTConfigurable::Configure(const KTPStoreNode* node)
    {
        if (node != NULL)
        {
            if (! ConfigureFromPStore(node)) return false;
        }
        return ConfigureFromCL();
    }

    inline Bool_t KTConfigurable::ConfigureFromPStore(const KTPStoreNode* node)
    {
        return true;
    }

    inline Bool_t KTConfigurable::ConfigureFromCL()
    {
        return true;
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
