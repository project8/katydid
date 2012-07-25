/*
 * KTPStoreNode.hh
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#ifndef KTPSTORENODE_HH_
#define KTPSTORENODE_HH_

#include <boost/property_tree/ptree.hpp>

#include <string>

namespace Katydid
{
    typedef boost::property_tree::ptree TreeNode;

    class KTPStoreNode
    {
        public:
            KTPStoreNode();
            virtual ~KTPStoreNode();

            KTPStoreNode* GetChildNode(const std::string nodeName);

            template< typename XType >
            XType GetData() const;

        protected:
            TreeNode* fTree;

    };

    template< typename XType >
    XType KTPStoreNode::GetData()

} /* namespace Katydid */
#endif /* KTPSTORENODE_HH_ */
