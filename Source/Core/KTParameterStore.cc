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

    Bool_t KTParameterStore::ReadConfigFile(const string& filename)
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

    Bool_t KTParameterStore::NodeExists(const string& address) const
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

    KTPStoreNode* KTParameterStore::GetNode(const string& address) const
    {
        /*
        PStoreTree::const_assoc_iterator it = fStore.find(address);
        if (it == fStore.not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return new KTPStoreNode(&(it->second));
        */
        try
        {
            return new KTPStoreNode(&(fStore.get_child(address)));
        }
        catch (std::exception& e)
        {
            return NULL;
        }
        return NULL; // code shouldn't get here
    }

    Bool_t KTParameterStore::ChangeValue(const string& address, const string& newValue)
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
        BOOST_FOREACH( const KTParameterStore::PStoreTree::value_type& treeNode, tree->get_child("") )
        {
            KTParameterStore::PStoreTree subtree = treeNode.second;
            string addressOfNode;
            if (addressOfTree.size() > 0) addressOfNode = addressOfTree + "." + treeNode.first;
            else addressOfNode = treeNode.first;

            *printStream << addressOfNode;
            if (tree->get< string >(treeNode.first).length() > 0)
            {
                *printStream << " = " << tree->get< string >(treeNode.first);
            }
            *printStream << '\n';

            // Recursively go down the hierarchy
            PrintSubTree(&subtree, addressOfNode, printStream);
        }
        return;
    }




} /* namespace Katydid */
