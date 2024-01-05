/*
 * KTArbitraryMetadata.hh
 *
 *  Created on: Sept 20, 2021
 *      Author: N.S. Oblath
 */

#ifndef KTARBITRARY_METADATA_HH_
#define KTARBITRARY_METADATA_HH_

#include "Data.hh"

#include "MemberVariable.hh"

#include "param.hh"


namespace Katydid
{
    class KTArbitraryMetadata : public Nymph::Data
    {
        public:
            KTArbitraryMetadata();
            virtual ~KTArbitraryMetadata();

            MEMVAR_PTR(scarab::param*, Metadata);
    };
} /* namespace Katydid */
#endif /* KTARBITRARY_METADATA_HH_ */
