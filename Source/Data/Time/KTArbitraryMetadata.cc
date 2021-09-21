/*
 * KTArbitraryMetadata.cc
 *
 *  Created on: Sept 20, 2021
 *      Author: N.S. Oblath
 */

#include "KTArbitraryMetadata.hh"

namespace Katydid
{
    const std::string KTArbitraryMetadata::sName("arbitrary-metadata");

    KTArbitraryMetadata::KTArbitraryMetadata() :
            KTExtensibleData< KTArbitraryMetadata >(),
            fMetadata( new scarab::param() )
    {
    }

    KTArbitraryMetadata::~KTArbitraryMetadata()
    {
        delete fMetadata;
    }

} /* namespace Katydid */
