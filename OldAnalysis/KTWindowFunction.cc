/*
 * KTWindowFunction.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTWindowFunction.hh"

#include "KTPStoreNode.hh"

namespace Katydid
{

    KTWindowFunction::KTWindowFunction()
    {
    }

    KTWindowFunction::~KTWindowFunction()
    {
    }

    Bool_t KTWindowFunction::Configure(const KTPStoreNode* node)
    {
        if (node->HasData("length")) SetLength(node->GetData< Double_t >("length", 1.));
        if (node->HasData("bin_width")) SetBinWidth(node->GetData< Double_t >("bin_width", 1.));
        if (node->HasData("size")) SetSize(node->GetData< Double_t >("size", 1));

        return ConfigureWindowFunctionSubclass(node);
    }


} /* namespace Katydid */
