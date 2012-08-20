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
        protected:
            typedef boost::property_tree::ptree TreeNode;

        public:
            KTPStoreNode();
            KTPStoreNode(const TreeNode* tree);
            virtual ~KTPStoreNode();

            const KTPStoreNode* GetChild(const std::string& nodeName) const;

            Bool_t HasData(const std::string& dataName) const;

            template< typename XType >
            XType GetData(const std::string& dataName) const;
            template< typename XType >
            XType GetData(const std::string& dataName, XType defaultValue) const;

        protected:
            const TreeNode* fTree;

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
        KTERROR(utillog_psnode, "Unkown error while attempting to retrieve <" << dataName << ">.");
        throw dnfException;
    }

    template< typename XType >
    XType KTPStoreNode::GetData(const std::string& dataName, XType defaultValue) const
    {
        KTPStoreNodeDataNotFound dnfException;
        TreeNode::const_assoc_iterator it = fTree->find(dataName);
        if (it == fTree->not_found())
        {
            KTERROR(utillog_psnode, "No subnode was found called <" << dataName << ">.");
            throw dnfException;
        }

        // get_value will only return data from this node (whereas get can return data from subnodes)
        return it->second.get_value< XType >(defaultValue);
    }


} /* namespace Katydid */
#endif /* KTPSTORENODE_HH_ */
