/*
 * KTArbitraryMetadata.cc
 *
 *  Created on: Sept 20, 2021
 *      Author: N.S. Oblath
 */

#include "KTArbitraryMetadata.hh"

namespace Katydid
{
    
    KTArbitraryMetadata::KTArbitraryMetadata() :
            Nymph::Data(),
            fMetadata(nullptr)
    {
    }

    KTArbitraryMetadata::~KTArbitraryMetadata()
    {
        delete fMetadata;
    }

} /* namespace Katydid */
