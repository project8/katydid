/*
 * KTArbitraryMetadata.hh
 *
 *  Created on: Sept 20, 2021
 *      Author: N.S. Oblath
 */

#ifndef KTARBITRARY_METADATA_HH_
#define KTARBITRARY_METADATA_HH_

#include "KTData.hh"

#include "param.hh"


namespace Katydid
{
    class KTArbitraryMetadata : public Nymph::KTExtensibleData< KTArbitraryMetadata >
    {
        public:
            KTArbitraryMetadata();
            virtual ~KTArbitraryMetadata();

            void SetMetadata( scarab::param* metadata );
            MEMBERVARIABLE_NOSET(scarab::param*, Metadata);

        public:
            static const std::string sName;

    };

    inline void KTArbitraryMetadata::SetMetadata( scarab::param* metadata )
    {
        delete fMetadata;
        fMetadata = metadata;
        return;
    }

} /* namespace Katydid */
#endif /* KTARBITRARY_METADATA_HH_ */
