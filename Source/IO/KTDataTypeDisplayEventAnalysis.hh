/*
 * KTDataTypeDisplayEventAnalysis.hh
 *
 *  Created on: Oct 3, 2016
 *      Author: ezayas
 */

#ifndef KTDATATYPEDISPLAYEVENTANALYSIS_HH_
#define KTDATATYPEDISPLAYEVENTANALYSIS_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    
    class KTDataTypeDisplayEventAnalysis : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplayEventAnalysis();
            virtual ~KTDataTypeDisplayEventAnalysis();

            void RegisterSlots();


            //**************************
            // Spectrum Collection Data
            //**************************
        public:
            void DrawPSCollectionData(Nymph::KTDataPtr data);
            void DrawPowerFitDataPXUnweighted(Nymph::KTDataPtr data);
            void DrawPowerFitDataPXWeighted(Nymph::KTDataPtr data);
            void DrawPowerFitDataPY(Nymph::KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYEVENTANALYSIS_HH_ */
