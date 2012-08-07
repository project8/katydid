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
        public:
            typedef boost::property_tree::ptree PStoreTree;

            // Config file reading interface
        public:
            Bool_t ReadConfigFile(const std::string& filename);

            // Parameter store interface
        public:
            KTPStoreNode* GetNode(const std::string address) const;

            // Parameter storage
        public:
            const PStoreTree* GetTree() const;

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

    inline const KTParameterStore::PStoreTree* KTParameterStore::GetTree() const
    {
        return &fStore;
    }

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
