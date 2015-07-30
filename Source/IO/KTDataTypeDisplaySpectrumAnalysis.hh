/*
 * KTDataTypeDisplaySpectrumAnalysis.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDATATYPEDISPLAYSPECTRUMANALYSIS_HH_
#define KTDATATYPEDISPLAYSPECTRUMANALYSIS_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    using namespace Nymph;
    class KTDataTypeDisplaySpectrumAnalysis : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplaySpectrumAnalysis();
            virtual ~KTDataTypeDisplaySpectrumAnalysis();

            void RegisterSlots();


            //************************
            // Normalized Frequency Spectrum Data
            //************************
        public:
            void DrawNormalizedFSDataPolar(KTDataPtr data);
            void DrawNormalizedFSDataFFTW(KTDataPtr data);
            void DrawNormalizedFSDataPolarPhase(KTDataPtr data);
            void DrawNormalizedFSDataFFTWPhase(KTDataPtr data);
            void DrawNormalizedFSDataPolarPower(KTDataPtr data);
            void DrawNormalizedFSDataFFTWPower(KTDataPtr data);

            //************************
            // Analytic Associate Data
            //************************
        public:
            void DrawAnalyticAssociateData(KTDataPtr data);
            void DrawAnalyticAssociateDataDistribution(KTDataPtr data);

            //************************
            // Correlation Data
            //************************
        public:
            void DrawCorrelationData(KTDataPtr data);
            void DrawCorrelationDataDistribution(KTDataPtr data);

            //************************
            // Correlation TS Data
            //************************
        public:
            void DrawCorrelationTSData(KTDataPtr data);
            void DrawCorrelationTSDataDistribution(KTDataPtr data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void DrawHoughData(KTDataPtr data);

            //************************
            // Gain Variation Data
            //************************
        public:
            void DrawGainVariationData(KTDataPtr data);

            //************************
            // WignerVille Data
            //************************
        public:
            void DrawWignerVilleData(KTDataPtr data);
            void DrawWignerVilleDataDistribution(KTDataPtr data);
            void DrawWV2DData(KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYSPECTRUMANALYSIS_HH_ */
