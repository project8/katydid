/*
 * KTParameterStore.cc
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 */

#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"
#include "KTLogger.hh"

#include <boost/property_tree/json_parser.hpp>

namespace Katydid
{
    KTLOGGER(utillog_pstore, "katydid.utility");

    // protected
    KTParameterStore::KTParameterStore() :
            fStore()
    {
    }

    // protected
    KTParameterStore::~KTParameterStore()
    {
    }

    Bool_t KTParameterStore::ReadConfigFile(const std::string& filename)
    {
        try
        {
            boost::property_tree::json_parser::read_json< PStoreTree >(filename, fStore);
        }
        catch (boost::property_tree::json_parser::json_parser_error& e)
        {
            KTERROR(utillog_pstore, "Problem occured while parsing config file <" << filename << ">.");
            return false;
        }
        return true;
    }

    KTPStoreNode* KTParameterStore::GetNode(const std::string address) const
    {
        PStoreTree::const_assoc_iterator it = fStore.find(address);
        if (it == fStore.not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return new KTPStoreNode(&(it->second));
    }


} /* namespace Katydid */
