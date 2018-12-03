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

#include "KTMultiPSData.hh"

#include <vector>
#include <map>

namespace Katydid
{
    class KTPowerSpectrum;
    class KTTimeSeriesFFTW;
    class KTSliceHeader;

    class KTPSCollectionData : public KTMultiPSData
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
            MEMBERVARIABLEREF(double, MinFreq);
            MEMBERVARIABLEREF(double, MaxFreq);
            MEMBERVARIABLEREF(unsigned, MinBin);
            MEMBERVARIABLEREF(unsigned, MaxBin);
            MEMBERVARIABLEREF(bool, Filling);

        public:
            static const std::string sName;

    };
/*
    class KTTSCollectionData : public Nymph::KTExtensibleData< KTTSCollectionData >
    {
        public:
            typedef std::vector< std::pair< KTSliceHeader*, KTTimeSeriesFFTW* > > collection;

            KTTSCollectionData();
            KTTSCollectionData(const KTTSCollectionData& orig);
            virtual ~KTTSCollectionData();

            KTTSCollectionData& operator=(const KTTSCollectionData& rhs);

            void AddTimeSeries( KTSliceHeader* slice, KTTimeSeriesFFTW* ts );
            MEMBERVARIABLEREF_NOSET(collection, Series);
            MEMBERVARIABLEREF(double, StartTime);
            MEMBERVARIABLEREF(double, EndTime);
            MEMBERVARIABLEREF(double, DeltaT);
            MEMBERVARIABLEREF(bool, Filling);

        public:
            static const std::string sName;
    };
*/
} /* namespace Katydid */

#endif /* KTSPECTRUMCOLLECTIONDATA_HH_ */
