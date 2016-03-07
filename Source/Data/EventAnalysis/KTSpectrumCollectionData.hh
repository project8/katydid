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
#include <map>

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
            MEMBERVARIABLEREF(double, StartTime);
            MEMBERVARIABLEREF(double, EndTime);
            MEMBERVARIABLEREF(double, DeltaT);
            MEMBERVARIABLEREF(bool, Filling);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTSPECTRUMCOLLECTIONDATA_HH_ */
