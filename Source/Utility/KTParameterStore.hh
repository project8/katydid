/*
 * KTParameterStore.hh
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 */

#ifndef KTPARAMETERINTERFACE_HH_
#define KTPARAMETERINTERFACE_HH_

#include "KTSingleton.hh"
#include "KTLogger.hh"

#include <boost/property_tree/ptree.hpp>

#include <string>
#include <vector>

namespace Katydid
{
    KTLOGGER(utillog_pstore, "katydid.utility");

    class KTParameterStore : public KTSingleton< KTParameterStore >
    {
        protected:
            typedef std::vector< boost::property_tree::ptree > StoreDB;

            // Parameter access interface
        public:
            template< typename XValueType >
            XValueType GetParameter(const std::string address) const;

        protected:


            // Parameter storage
        protected:
            std::vector< boost::property_tree::ptree > fStore;

            // This is a singleton class
            //  -- Friendships with KTSingleton and KTDestroyer
            //  -- Protected constructor and destructor
        protected:
            friend class KTSingleton< KTParameterStore >;
            friend class KTDestroyer< KTParameterStore >;
            KTParameterStore();
            ~KTParameterStore();
    };

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
        /*
        try
        {
            XValueType returnVal = boost::any_cast<XValueType>(dummy);
            return returnVal;
        }
        catch(const boost::bad_any_cast &)
        {
            KTFATAL(utillog_pstore, "Parameter address: " << address << " -- Unable to cast to the given type.");
            exit(-1);
            //return XValueType();
        }
        */
    }


} /* namespace Katydid */
#endif /* KTPARAMETERINTERFACE_HH_ */
