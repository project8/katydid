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

    class KTPSCollectionData : public KTMultiPSDataCore, public Nymph::KTExtensibleData< KTPSCollectionData >
    {
        public:

            KTPSCollectionData();
            //KTPSCollectionData(const KTPSCollectionData& orig);
            virtual ~KTPSCollectionData();

            //KTPSCollectionData& operator=(const KTPSCollectionData& rhs);

            KTPSCollectionData& SetNComponents(unsigned component);

            void AddSpectrum(double timeStamp, const KTPowerSpectrum& spectrum, unsigned iComponent);
            MEMBERVARIABLEREF(double, StartTime);
            MEMBERVARIABLEREF(double, EndTime);
            MEMBERVARIABLEREF(double, DeltaT);
            MEMBERVARIABLEREF(double, MinFreq);
            MEMBERVARIABLEREF(double, MaxFreq);
            MEMBERVARIABLEREF(unsigned, MinBin);
            MEMBERVARIABLEREF(unsigned, MaxBin);
            MEMBERVARIABLEREF(bool, Filling);
            MEMBERVARIABLEREF(uint64_t, SpectrogramCounter);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTSPECTRUMCOLLECTIONDATA_HH_ */
