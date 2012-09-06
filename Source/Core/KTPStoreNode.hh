/*
 * KTPStoreNode.hh
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#ifndef KTPSTORENODE_HH_
#define KTPSTORENODE_HH_

#include "KTLogger.hh"

#include <boost/property_tree/ptree.hpp>

#include "Rtypes.h"

#include <exception>
#include <string>
#include <utility>

namespace Katydid
{
    KTLOGGER(utillog_psnode, "katydid.utility");

    class KTPStoreNodeDataNotFound : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Data with requested name was not found in the parameter store node";
        }
    };

    class KTPStoreNode
    {
        public:
            typedef boost::property_tree::ptree::const_assoc_iterator const_sorted_iterator;
            typedef std::pair< KTPStoreNode::const_sorted_iterator, KTPStoreNode::const_sorted_iterator > csi_pair;

        protected:
            typedef boost::property_tree::ptree TreeNode;

        public:
            KTPStoreNode();
            KTPStoreNode(const TreeNode* tree);
            virtual ~KTPStoreNode();

            /// Counts how many immediate-child nodes with nodeName exist (non-recursive).
            UInt_t CountNodes(const std::string& nodeName) const;

            const_sorted_iterator SortedBegin() const;

            const_sorted_iterator Find(const std::string& nodeName) const;

            const_sorted_iterator NotFound() const;

            std::pair< const_sorted_iterator, const_sorted_iterator > EqualRange(const std::string& nodeName) const;

            /// Returns a constant pointer to the child node named nodeName (non-recursive).
            /// Returns NULL if the child doesn't exist.
            /// If multiple nodes exist with this name, the choice of the node that gets returned is not defined.
            const KTPStoreNode* GetChild(const std::string& nodeName) const;
            /// Returns a pointer to the child node named nodeName (non-recursive).
            /// Returns NULL if the child doesn't exist.
            /// If multiple nodes exist with this name, the choice of the node that gets returned is not defined.
            KTPStoreNode* GetChild(const std::string& nodeName);

            /// Returns true if an immediate-child exists with name dataName, and that child contains data (non-recursive).
            Bool_t HasData(const std::string& dataName) const;

            /// Returns data with name dataName as a string.
            /// If a child with name dataName doesn't exist, or the child does not contain data, throws a KTPStoreNodeDataNotFound exception.
            /// If multiple nodes exist with this name, the choice of the node whose data gets returned is not defined.
            std::string GetData(const std::string& dataName) const;
            /// Returns data with name dataName as a string.
            /// If a child with name dataName doesn't exist, or the child does not contain data, returns the default value provided.
            /// If multiple nodes exist with this name, the choice of the node whose data gets returned is not defined.
            std::string GetData(const std::string& dataName, const std::string& defaultValue) const;

            /// Returns data with name dataName, cast to XType (non-recursive).
            /// If a child with name dataName doesn't exist, or the child does not contain data, throws a KTPStoreNodeDataNotFound exception.
            template< typename XType >
            XType GetData(const std::string& dataName) const;
            /// Returns data with name dataName, cast to XType (non-recursive).
            /// If a child with name dataName doesn't exist, or the child does not contain data, returns the default value provided.
            template< typename XType >
            XType GetData(const std::string& dataName, XType defaultValue) const;

            void PrintTree() const;

        protected:
            const TreeNode* fTree;

            void PrintSubTree(const TreeNode* tree, const std::string& addressOfTree, std::stringstream* printStream) const;

        private:
            std::string fDefaultValue;

    };

    template< typename XType >
    XType KTPStoreNode::GetData(const std::string& dataName) const
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
            return it->second.get_value< XType >();
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

    template< typename XType >
    XType KTPStoreNode::GetData(const std::string& dataName, XType defaultValue) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            return defaultValue;
        }

        // get_value will only return data from this node (whereas get can return data from subnodes)
        return it->second.get_value< XType >(defaultValue);
    }

    inline UInt_t KTPStoreNode::CountNodes(const std::string& nodeName) const
    {
        return fTree->count(nodeName);
    }

    inline KTPStoreNode::const_sorted_iterator KTPStoreNode::SortedBegin() const
    {
        return fTree->ordered_begin();
    }

    inline KTPStoreNode::const_sorted_iterator KTPStoreNode::Find(const std::string& nodeName) const
    {
        return fTree->find(nodeName);
    }

    inline KTPStoreNode::const_sorted_iterator KTPStoreNode::NotFound() const
    {
        return fTree->not_found();
    }

    inline std::pair< KTPStoreNode::const_sorted_iterator, KTPStoreNode::const_sorted_iterator > KTPStoreNode::EqualRange(const std::string& nodeName) const
    {
        return fTree->equal_range(nodeName);
    }

} /* namespace Katydid */
#endif /* KTPSTORENODE_HH_ */
