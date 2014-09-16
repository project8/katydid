/**
 @file KTAnalyticAssociator.hh
 @brief Contains KTAnalyticAssociator
 @details Creates an analytic associate of a time series
 @author: N. S. Oblath
 @date: Dec 17, 2012
 */

#ifndef KTANALYTICASSOCIATEDATA_HH_
#define KTANALYTICASSOCIATEDATA_HH_

#include "KTTimeSeriesData.hh"

namespace Katydid
{
    class KTAnalyticAssociateData : public KTTimeSeriesDataCore, public KTExtensibleData< KTAnalyticAssociateData >
    {
        public:
            KTAnalyticAssociateData();
            virtual ~KTAnalyticAssociateData();

            virtual KTAnalyticAssociateData& SetNComponents(unsigned num);

        private:
            static const std::string sName;
};

} /* namespace Katydid */
#endif /* KTANALYTICASSOCIATEDATA_HH_ */
