/*
 * KTSpectrumCollectionData.hh
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#ifndef KTSPECTRUMCOLLECTIONDATA_HH_
#define KTSPECTRUMCOLLECTIONDATA_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include <vector>
#include <map>

namespace Katydid
{
    class KTPowerSpectrum;

    class KTPSCollectionData : public Nymph::KTExtensibleData< KTPSCollectionData >
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
            MEMBERVARIABLEREF(double, StartFreq);
            MEMBERVARIABLEREF(double, EndFreq);
            MEMBERVARIABLEREF(bool, Filling);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTSPECTRUMCOLLECTIONDATA_HH_ */
