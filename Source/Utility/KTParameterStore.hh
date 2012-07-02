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

#include <boost/any.hpp>

#include <string>

namespace Katydid
{
    KTLOGGER(utillog_pstore, "katydid.utility");

    class KTParameterStore : public KTSingleton< KTParameterStore >
    {
        public:
            template< typename XValueType >
            XValueType GetParameter(const std::string address) const;

        protected:
            boost::any dummy;

        protected:
            friend class KTSingleton< KTParameterStore >;
            friend class KTDestroyer< KTParameterStore >;
            KTParameterStore();
            ~KTParameterStore();
    };

    template< typename XValueType >
    inline XValueType KTParameterStore::GetParameter(const std::string address) const
    {
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
    }


} /* namespace Katydid */
#endif /* KTPARAMETERINTERFACE_HH_ */
