/*
 * KTBasicROOTTypeWriterAnalysis.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERANALYSIS_HH_
#define KTBASICROOTTYPEWRITERANALYSIS_HH_

#include "KTBasicROOTFileWriter.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTData;

    class KTBasicROOTTypeWriterAnalysis : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterAnalysis();
            virtual ~KTBasicROOTTypeWriterAnalysis();

            void RegisterSlots();


            //************************
            // Normalized Frequency Spectrum Data
            //************************
        public:
            void WriteNormalizedFSDataPolar(boost::shared_ptr<KTData> data);
            void WriteNormalizedFSDataFFTW(boost::shared_ptr<KTData> data);
            void WriteNormalizedFSDataPolarPhase(boost::shared_ptr<KTData> data);
            void WriteNormalizedFSDataFFTWPhase(boost::shared_ptr<KTData> data);
            void WriteNormalizedFSDataPolarPower(boost::shared_ptr<KTData> data);
            void WriteNormalizedFSDataFFTWPower(boost::shared_ptr<KTData> data);

            //************************
            // Analytic Associate Data
            //************************
        public:
            void WriteAnalyticAssociateData(boost::shared_ptr<KTData> data);
            void WriteAnalyticAssociateDataDistribution(boost::shared_ptr<KTData> data);

            //************************
            // Correlation Data
            //************************
        public:
            void WriteCorrelationData(boost::shared_ptr<KTData> data);
            void WriteCorrelationDataDistribution(boost::shared_ptr<KTData> data);

            //************************
            // Correlation TS Data
            //************************
        public:
            void WriteCorrelationTSData(boost::shared_ptr<KTData> data);
            void WriteCorrelationTSDataDistribution(boost::shared_ptr<KTData> data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void WriteHoughData(boost::shared_ptr<KTData> data);

            //************************
            // Gain Variation Data
            //************************
        public:
            void WriteGainVariationData(boost::shared_ptr<KTData> data);

            //************************
            // WignerVille Data
            //************************
        public:
            void WriteWignerVilleData(boost::shared_ptr<KTData> data);
            void WriteWignerVilleDataDistribution(boost::shared_ptr<KTData> data);
            void WriteWV2DData(boost::shared_ptr<KTData> data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERANALYSIS_HH_ */
