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
            MEMBERVARIABLE( int, MCH ); // main carrier high theta
            MEMBERVARIABLE( int, MCL ); // main carrier low theta
            MEMBERVARIABLE( int, SB );  // sideband
          
        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTClassifierResultsData& hdr);

} /* namespace Katydid */
#endif /* KTCLASSIFIERRESULTSDATA_HH_ */
