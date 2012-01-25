/**
 @file KTSimpleClusteringProcessor.hh
 @brief Contains KTSimpleClusteringProcessor
 @details Simple cluster-finding algorithm that works by looking for lines of high-peaked bins increasing in frequency
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTSIMPLECLUSTERINGPROCESSOR_HH_
#define KTGAINNORMALIZATIONPROCESSOR_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTSimpleClusteringProcessor : public KTProcessor
    {
        public:
            KTSimpleClusteringProcessor();
            virtual ~KTSimpleClusteringProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            void ProcessPowerSpectrum(KTPhysicalArray< 1, Double_t >* powerSpec);

    };

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATIONPROCESSOR_HH_ */
