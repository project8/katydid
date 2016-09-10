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
            void DrawNormalizedFSDataPolar(Nymph::KTDataPtr data);
            void DrawNormalizedFSDataFFTW(Nymph::KTDataPtr data);
            void DrawNormalizedFSDataPolarPhase(Nymph::KTDataPtr data);
            void DrawNormalizedFSDataFFTWPhase(Nymph::KTDataPtr data);
            void DrawNormalizedFSDataPolarPower(Nymph::KTDataPtr data);
            void DrawNormalizedFSDataFFTWPower(Nymph::KTDataPtr data);

            //************************
            // Analytic Associate Data
            //************************
        public:
            void DrawAnalyticAssociateData(Nymph::KTDataPtr data);
            void DrawAnalyticAssociateDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Correlation Data
            //************************
        public:
            void DrawCorrelationData(Nymph::KTDataPtr data);
            void DrawCorrelationDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Correlation TS Data
            //************************
        public:
            void DrawCorrelationTSData(Nymph::KTDataPtr data);
            void DrawCorrelationTSDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void DrawHoughData(Nymph::KTDataPtr data);

            //************************
            // Gain Variation Data
            //************************
        public:
            void DrawGainVariationData(Nymph::KTDataPtr data);

            //************************
            // WignerVille Data
            //************************
        public:
            void DrawWignerVilleData(Nymph::KTDataPtr data);
            void DrawWignerVilleDataDistribution(Nymph::KTDataPtr data);
            void DrawWV2DData(Nymph::KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYSPECTRUMANALYSIS_HH_ */
