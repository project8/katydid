/*
 * KTClassifierResultsData.hh
 *
 *  Created on: Feb 16, 2018
 *      Author: ezayas, L. Saldana
 */

#ifndef KTCLASSIFIERRESULTSDATA_HH_
#define KTCLASSIFIERRESULTSDATA_HH_

#include "KTData.hh"

namespace Katydid
{
    
    class KTClassifierResultsData : public Nymph::KTExtensibleData< KTClassifierResultsData >
    {  

        public:
            KTClassifierResultsData();
            KTClassifierResultsData(const KTClassifierResultsData& orig);
            virtual ~KTClassifierResultsData();

            KTClassifierResultsData& operator=(const KTClassifierResultsData& rhs);

        public:
            // member variable(s) to describe classification
            MEMBERVARIABLE( unsigned, Component );
            MEMBERVARIABLE( int, MainCarrierHigh ); // main carrier high theta
            MEMBERVARIABLE( int, MainCarrierLow ); // main carrier low theta
            MEMBERVARIABLE( int, SideBand );  // sideband
          
        public:
            static const std::string sName;
    };

} /* namespace Katydid */
#endif /* KTCLASSIFIERRESULTSDATA_HH_ */
