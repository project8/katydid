/*
 * KTParameterStore.hh
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 */

#ifndef KTPARAMETERINTERFACE_HH_
#define KTPARAMETERINTERFACE_HH_

#include "KTSingleton.hh"

#include "Rtypes.h"

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <vector>

namespace Katydid
{
    class KTPStoreNode;

    class KTParameterStore : public KTSingleton< KTParameterStore >
    {
        protected:
            typedef boost::property_tree::ptree PStoreTree;

            // Config file reading interface
        public:
            Bool_t ReadConfigFile(const std::string& filename);

            // Parameter store interface
        public:
            KTPStoreNode* GetNode(const std::string address) const;

            // Parameter storage
        protected:
            PStoreTree fStore;

            // This is a singleton class
            //  -- Friendships with KTSingleton and KTDestroyer
            //  -- Protected constructor and destructor
        protected:
            friend class KTSingleton< KTParameterStore >;
            friend class KTDestroyer< KTParameterStore >;
            KTParameterStore();
            ~KTParameterStore();
    };

    /*
    template< typename XValueType >
    inline XValueType KTParameterStore::GetParameter(const std::string address) const
    {
        Bool_t foundParam = false;
        for (StoreDB::const_iterator iter = fStore.begin(); iter != fStore.end(); iter++)
        {
            XValueType returnVal = (*iter).get< XValueType >(address);
            foundParam = true;
            break;
        }
    }
     */

} /* namespace Katydid */
#endif /* KTPARAMETERINTERFACE_HH_ */
