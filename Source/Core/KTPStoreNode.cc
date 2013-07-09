/*
 * KTPStoreNode.cc
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#include "KTPStoreNode.hh"

#include <boost/foreach.hpp>

#include <sstream>

using std::string;
using std::stringstream;

namespace Katydid
{

    UIntPair ParsePairUInt(const std::string& pairString)
    {
        UInt_t first = 0, second = 0;
        Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> int_[ref(second) = boost::spirit::qi::_1]),
                space);
        if (! parsed)
        {
            KTWARN(utillog_psnode, "Unable to parse pair: " << pairString);
        }
        return std::make_pair< UInt_t, UInt_t >(first, second);
    }

    UIntDoublePair ParsePairUIntDouble(const std::string& pairString)
    {
        UInt_t first = 0;
        Double_t second = 0.;
        Bool_t parsed = phrase_parse(pairString.begin(), pairString.end(),
                (int_[ref(first)=boost::spirit::qi::_1] >> ',' >> double_[ref(second) = boost::spirit::qi::_1]),
                space);
        if (! parsed)
        {
            KTWARN(utillog_psnode, "Unable to parse pair: " << pairString);
        }
        return std::make_pair< UInt_t, Double_t >(first, second);
    }


    KTPStoreNode::KTPStoreNode() :
            fTree(NULL),
            fDefaultValue("DEFAULT VALUE FOR KTPSTORENODE USE")
    {
    }

    KTPStoreNode::KTPStoreNode(const TreeNode* tree) :
            fTree(tree),
            fDefaultValue("DEFAULT VALUE FOR KTPSTORENODE USE")
    {
    }

    KTPStoreNode::KTPStoreNode(const KTPStoreNode& orig) :
            fTree(orig.fTree),
            fDefaultValue(orig.fDefaultValue)
    {
    }

    KTPStoreNode::~KTPStoreNode()
    {
        // this class does not own the node pointed to by fTree
    }

    KTPStoreNode& KTPStoreNode::operator=(const KTPStoreNode& rhs)
    {
        fTree = rhs.fTree;
        fDefaultValue = rhs.fDefaultValue;
        return *this;
    }

    const KTPStoreNode KTPStoreNode::GetChild(const string& nodeName) const
    {
        TreeNode::const_assoc_iterator it = fTree->find(nodeName);
        if (it == fTree->not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return KTPStoreNode(&(it->second));
    }

    KTPStoreNode KTPStoreNode::GetChild(const string& nodeName)
    {
        TreeNode::const_assoc_iterator it = fTree->find(nodeName);
        if (it == fTree->not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return KTPStoreNode(&(it->second));
    }

    std::string KTPStoreNode::Value()
    {
        return fTree->data();
    }


    Bool_t KTPStoreNode::HasData(const string& dataName) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            // subnode with dataName was not found
            return false;
        }

        try
        {
            // get_value will only return data from this node (whereas get can return data from subnodes)
            string temp = it->second.get_value< string >();
        }
        catch (boost::property_tree::ptree_bad_path& e)
        {
            // subnode with dataName was found, but was not data
            return false;
        }
        // subnode with dataName is data!
        return true;
    }

    std::string KTPStoreNode::GetData(const std::string& dataName) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            KTERROR(utillog_psnode, "No subnode was found called <" << dataName << ">.");
            throw dnfException;
        }

        try
        {
            // get_value will only return data from this node (whereas get can return data from subnodes)
            return it->second.get_value< string >();
        }
        catch (boost::property_tree::ptree_bad_path& e)
        {
            KTERROR(utillog_psnode, "Subnode <" << dataName << "> did not contain data.");
            throw dnfException;
        }
        catch (boost::property_tree::ptree_bad_data& e)
        {
            KTERROR(utillog_psnode, "Unable to convert to the specified type for parameter named <" << dataName << ">.");
            throw dnfException;
        }
        KTERROR(utillog_psnode, "Unknown error while attempting to retrieve <" << dataName << ">.");
        throw dnfException;
    }

    std::string KTPStoreNode::GetData(const std::string& dataName, const std::string& defaultValue) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            KTERROR(utillog_psnode, "No subnode was found called <" << dataName << ">.");
            throw dnfException;
        }

        // get_value will only return data from this node (whereas get can return data from subnodes)
        return it->second.get_value< string >(defaultValue);
    }

    void KTPStoreNode::PrintTree() const
    {
        stringstream printStream;
        printStream << '\n';
        PrintSubTree(fTree, "", &printStream);
        KTDEBUG(utillog_psnode, printStream.str());
        return;
    }

    void KTPStoreNode::PrintSubTree(const TreeNode* tree, const string& addressOfTree, stringstream* printStream) const
    {
        for (const_iterator it = tree->begin(); it != tree->end(); it++)
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

    template<>
    UIntPair KTPStoreNode::GetValue< UIntPair >() const
    {
        return ParsePairUInt(fTree->get_value< std::string >());
    }

    template<>
    UIntPair KTPStoreNode::GetData< UIntPair >(const std::string& dataName) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            KTERROR(utillog_psnode, "No subnode was found called <" << dataName << ">.");
            throw dnfException;
        }

        try
        {
            // get_value will only return data from this node (whereas get can return data from subnodes)
            return ParsePairUInt(fTree->get_value< std::string >());
        }
        catch (boost::property_tree::ptree_bad_path& e)
        {
            KTERROR(utillog_psnode, "Subnode <" << dataName << "> did not contain data.");
            throw dnfException;
        }
        catch (boost::property_tree::ptree_bad_data& e)
        {
            KTERROR(utillog_psnode, "Unable to convert to the specified type for parameter named <" << dataName << ">.");
            throw dnfException;
        }
        KTERROR(utillog_psnode, "Unknown error while attempting to retrieve <" << dataName << ">.");
        throw dnfException;
    }


    template<>
    UIntPair KTPStoreNode::GetData< UIntPair >(const std::string& dataName, UIntPair defaultValue) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            return defaultValue;
        }

        // get_value will only return data from this node (whereas get can return data from subnodes)
        return ParsePairUInt(fTree->get_value< std::string >());
    }

} /* namespace Katydid */
