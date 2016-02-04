/*
 * KTSpectrumCollectionData.hh
 *
 *  Created on:Oct 9, 2015
 *      Author: nsoblath
 */

#ifndef KTSPECTRUMCOLLECTIONDATA_HH_
#define KTSPECTRUMCOLLECTIONDATA_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include <vector>

namespace Katydid
{
    using namespace Nymph;
    class KTPowerSpectrum;

    class KTPSCollectionData : public KTExtensibleData< KTPSCollectionData >
    {
        public:
            typedef std::map< double, KTPowerSpectrum* > collection;

            KTPSCollectionData();
            KTPSCollectionData(const KTPSCollectionData& orig);
            virtual ~KTPSCollectionData();

            KTPSCollectionData& operator=(const KTPSCollectionData& rhs);

            void AddSpectrum(double t, KTPowerSpectrum* spectrum);
            MEMBERVARIABLEREF_NOSET(collection, Spectra);
            MEMBERVARIABLEREF_NOSET(double, StartTime);
            MEMBERVARIABLEREF_NOSET(double, EndTime);
            MEMBERVARIABLEREF_NOSET(double, DeltaT);
            MEMBERVARIABLEREF_NOSET(bool, Filling);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTSPECTRUMCOLLECTIONDATA_HH_ */
