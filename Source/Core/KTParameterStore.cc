/*
 * KTParameterStore.cc
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 */

#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"
#include "KTLogger.hh"

#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
using std::stringstream;

using std::string;

namespace Katydid
{
    KTLOGGER(utillog_pstore, "KTParameterStore");

    // protected
    KTParameterStore::KTParameterStore() :
            fStore()
    {
    }

    // protected
    KTParameterStore::~KTParameterStore()
    {
    }

    bool KTParameterStore::ReadConfigFile(const string& filename)
    {
        if (filename.empty())
        {
            KTDEBUG(utillog_pstore, "No config. filename given; parameter store is empty.");
            return true;
        }
        try
        {
            boost::property_tree::json_parser::read_json< PStoreTree >(filename, fStore);
        }
        catch (boost::property_tree::json_parser::json_parser_error& e)
        {
            KTERROR(utillog_pstore, "Problem occurred while parsing config file <" << filename << ">.\n" << e.what());
            return false;
        }
        PrintTree();
        return true;
    }

    bool KTParameterStore::NodeExists(const string& address) const
    {
        try
        {
            fStore.get_child(address);
        }
        catch (std::exception& e)
        {
            return false;
        }
        return true;
    }

    KTPStoreNode KTParameterStore::GetNode(const string& address) const
    {
        /*
        PStoreTree::const_assoc_iterator it = fStore.find(address);
        if (it == fStore.not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return new KTPStoreNode(&(it->second));
        */
        try
        {
            return KTPStoreNode(&(fStore.get_child(address)));
        }
        catch (std::exception& e)
        {
            KTWARN(utillog_pstore, "The parameter store does not contain anything at address <" << address << ">");
            return KTPStoreNode(NULL);
        }
        return KTPStoreNode(NULL); // the code should not reach this point
    }

    bool KTParameterStore::ChangeValue(const string& address, const string& newValue)
    {
        if (! NodeExists(address))
        {
            KTERROR(utillog_pstore, "Address not found: " << address);
            return false;
        }

        try
        {
            fStore.put(address, newValue);
        }
        catch (std::exception& e)
        {
            KTERROR(utillog_pstore, "Unable to translate value <" << newValue << ">\n" << e.what());
            return false;
        }
        return true;
    }

    void KTParameterStore::PrintTree() const
    {
        stringstream printStream;
        printStream << '\n';
        PrintSubTree(&fStore, "", &printStream);
        KTDEBUG(utillog_pstore, printStream.str());
        return;
    }

    void KTParameterStore::PrintSubTree(const PStoreTree* tree, const string& addressOfTree, stringstream* printStream) const
    {
        for (PStoreTree::const_iterator it = tree->begin(); it != tree->end(); it++)
        {
            string addressOfNode;
            if (addressOfTree.size() > 0) addressOfNode = addressOfTree + "." + it->first;
            else addressOfNode = it->first;

            if (it->second.get_value< string >().length() > 0)
            {
                *printStream << addressOfNode << " = " << it->second.get_value< string >() << '\n';
            }
            else
            {
                *printStream << addressOfNode << '\n';
                PrintSubTree(&(it->second), addressOfNode, printStream);
            }
        }
        return;
    }




} /* namespace Katydid */
