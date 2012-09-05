/*
 * KTPStoreNode.cc
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{

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

    KTPStoreNode::~KTPStoreNode()
    {
        // this class does not own the node pointed to by fTree
    }

    const KTPStoreNode* KTPStoreNode::GetChild(const string& nodeName) const
    {
        TreeNode::const_assoc_iterator it = fTree->find(nodeName);
        if (it == fTree->not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return new KTPStoreNode(&(it->second));
    }

    KTPStoreNode* KTPStoreNode::GetChild(const string& nodeName)
    {
        TreeNode::const_assoc_iterator it = fTree->find(nodeName);
        if (it == fTree->not_found()) return NULL;
        // eclipse doesn't seem to like this line, but it compiles just fine
        return new KTPStoreNode(&(it->second));
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


} /* namespace Katydid */
