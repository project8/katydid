/*
 * KTBasicROOTTypeWriterSpectrumAnalysis.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERSPECTRUMANALYSIS_HH_
#define KTBASICROOTTYPEWRITERSPECTRUMANALYSIS_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{

    class KTBasicROOTTypeWriterSpectrumAnalysis : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterSpectrumAnalysis();
            virtual ~KTBasicROOTTypeWriterSpectrumAnalysis();

            void RegisterSlots();

            //************************
            // SNR
            //************************
        public:
            void WriteSNRPower(Nymph::KTDataPtr data);

            //************************
            // Normalized Frequency Spectrum Data
            //************************
        public:
            void WriteNormalizedFSDataPolar(Nymph::KTDataPtr data);
            void WriteNormalizedFSDataFFTW(Nymph::KTDataPtr data);
            void WriteNormalizedFSDataPolarPhase(Nymph::KTDataPtr data);
            void WriteNormalizedFSDataFFTWPhase(Nymph::KTDataPtr data);
            void WriteNormalizedFSDataPolarPower(Nymph::KTDataPtr data);
            void WriteNormalizedFSDataFFTWPower(Nymph::KTDataPtr data);

            void WriteNormalizedPSData(Nymph::KTDataPtr data);

            //************************
            // Analytic Associate Data
            //************************
        public:
            void WriteAnalyticAssociateData(Nymph::KTDataPtr data);
            void WriteAnalyticAssociateDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Correlation Data
            //************************
        public:
            void WriteCorrelationData(Nymph::KTDataPtr data);
            void WriteCorrelationDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Correlation TS Data
            //************************
        public:
            //void WriteCorrelationTSData(Nymph::KTDataPtr data);
            //void WriteCorrelationTSDataDistribution(Nymph::KTDataPtr data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void WriteHoughData(Nymph::KTDataPtr data);

            //************************
            // Gain Variation Data
            //************************
        public:
            void WriteGainVariationData(Nymph::KTDataPtr data);

            //************************
            // WignerVille Data
            //************************
        public:
            void WriteWignerVilleData(Nymph::KTDataPtr data);
            void WriteWignerVilleDataDistribution(Nymph::KTDataPtr data);
            void WriteWV2DData(Nymph::KTDataPtr data);

            //************************
            // KDTree Data
            //************************
        public:
            void WriteKDTreeSparseSpectrogram(Nymph::KTDataPtr data);

            //************************
            // Aggregated Channel Data
            //************************
        public:
            void WriteAggregatedFrequencySpectrumFFTWData(Nymph::KTDataPtr data);
            void WriteAggregatedFrequencySpectrumGrid(Nymph::KTDataPtr data);
            void WriteAggregatedTimeSeriesData(Nymph::KTDataPtr data);
            void WriteAggregatedTimeSeriesGrid(Nymph::KTDataPtr data);
            void WriteChannelAggregatedPowerSpectrumData(Nymph::KTDataPtr data);
            void WriteChannelAggregatedPowerSpectrumGrid(Nymph::KTDataPtr data);
            void WriteChannelAggregatedPSDSpectrumData(Nymph::KTDataPtr data);
            void WriteChannelAggregatedPSDSpectrumGrid(Nymph::KTDataPtr data);

#ifdef ENABLE_TUTORIAL
            //************************
            // LPF Tutorial Data
            //************************
        public:
            void WriteLowPassFilteredFSDataPolar(Nymph::KTDataPtr data);
            void WriteLowPassFilteredFSDataFFTW(Nymph::KTDataPtr data);
            void WriteLowPassFilteredPSData(Nymph::KTDataPtr data);
#endif /* ENABLE_TUTORIAL */

    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERSPECTRUMANALYSIS_HH_ */
