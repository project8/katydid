/*
 * KT2ROOT.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KT2ROOT_HH_
#define KT2ROOT_HH_

#include <string>
#include <map>

#include "KTPhysicalArray.hh"
#include "KTVarTypePhysicalArray.hh"

class TH1I;
class TH1D;
class TH2D;

namespace Cicada
{
    class TClassifierResultsData;
    class TProcessedTrackData;
    class TProcessedMPTData;
    class TMTEWithClassifierResultsData;
    class TMultiTrackEventData;
}

namespace Katydid
{
    class KTClassifierResultsData;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTFrequencySpectrumVariance;
    class KTPowerSpectrum;
    class KTTimeSeriesDist;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;
    class KTPowerFitData;
    class KTProcessedTrackData;
    class KTProcessedMPTData;
    class KTMultiTrackEventData;
    class KTClassifiedEventData;
    class KTSparseWaterfallCandidateData;
    class KTSequentialLineData;
    class KTDiscriminatedPoint;
    class KTAggregatedFrequencySpectrumDataFFTW;

    class TDiscriminatedPoint;
    class TSparseWaterfallCandidateData;
    class TSequentialLineData;

    class KT2ROOT
    {
        public:
            KT2ROOT();
            virtual ~KT2ROOT();

            //************
            // Time Series
            //************

            /// Parameter complexSampleIndex allows plotting of individual components of complex data:
            /// - Set to -1 (default) if the data is real
            /// - Set to 0 to display the real component of complex data
            /// - Set to 1 to display the imaginary component of complex data
            static TH1I* CreateHistogram(const KTVarTypePhysicalArray< uint64_t >* ts, const std::string& histName = "hRawTimeSeries", int complexSampleIndex = -1);
            /// Parameter complexSampleIndex allows plotting of individual components of complex data:
            /// - Set to -1 (default) if the data is real
            /// - Set to 0 to display the real component of complex data
            /// - Set to 1 to display the imaginary component of complex data
            static TH1I* CreateHistogram(const KTVarTypePhysicalArray< int64_t >* ts, const std::string& histName = "hRawTimeSeries", int complexSampleIndex = -1);
            static TH1I* CreateHistogram(const KTTimeSeriesDist* tsDist, const std::string& histName = "hTSDist");
            //static TH1I* CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTSDist");

            /// Create a histogram from a complex time series; for each sample, sqrt(real*real + imag*imag) is plotted
            static TH1D* CreateHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeries");
            /// Create a histogram from the real components of a complex time series
            static TH1D* CreateHistogramReal(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesReal");
            /// Create a histogram from the imaginary components of a complex time series
            static TH1D* CreateHistogramImag(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesImag");
            //static TH1D* CreateAmplitudeDistributionHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeriesDist");

            static TH1D* CreateHistogram(const KTTimeSeriesReal* ts, const std::string& histName = "hTimeSeries");
            //static TH1D* CreateAmplitudeDistributionHistogram(const KTTimeSeriesReal* ts, const std::string& histName = "hTimeSeriesDist");


            //*******************
            // Frequency Spectrum
            //*******************

            static TH1D* CreateMagnitudeHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name = "hFrequencySpectrumMag");
            static TH1D* CreatePhaseHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name = "hFrequencySpectrumPhase");

            static TH1D* CreatePowerHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name = "hFrequencySpectrumPower");

            static TH1D* CreateMagnitudeDistributionHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name = "hFrequencySpectrumMagDist");
            static TH1D* CreatePowerDistributionHistogram(const KTFrequencySpectrumPolar* fs, const std::string& name = "hFrequencySpectrumPowerDist");

            static TH1D* CreateMagnitudeHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name = "hFrequencySpectrumMag");
            static TH1D* CreatePhaseHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name = "hFrequencySpectrumPhase");

            static TH1D* CreatePowerHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name = "hFrequencySpectrumPower");

            static TH1D* CreateMagnitudeDistributionHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name = "hFrequencySpectrumMagDist");
            static TH1D* CreatePowerDistributionHistogram(const KTFrequencySpectrumFFTW* fs, const std::string& name = "hFrequencySpectrumPowerDist");
      
            static TH2D* CreateGridHistogram(const KTAggregatedFrequencySpectrumDataFFTW& fs, const std::string& name = "hFrequencySpectrumGrid");

            static TH1D* CreateHistogram(const KTFrequencySpectrumVariance* fs, const std::string& name = "hFrequencySpectrumVariance");


            //***************
            // Power Spectrum
            //***************

            static TH1D* CreatePowerHistogram(const KTPowerSpectrum* ps, const std::string& name = "hPowerSpectrum");
            static TH1D* CreatePowerDistributionHistogram(const KTPowerSpectrum* ps, const std::string& name = "hPowerSpectrumPower");

            static TH2D* CreateHistogram(const KTPhysicalArray< 2, double >* ht, const std::string& histName = "hHoughData");


            //*********************
            // Spectrum Collection
            //*********************

            static TH2D* CreatePowerHistogram(std::map< double, KTPowerSpectrum* > psColl, const std::string& name = "hPSCollectionData");

            //***************
            // Set Of Points
            //***************

            static TH1D* CreateMagnitudeHistogram(const KTPowerFitData* pf, const std::string& histName);


            //**********************
            // Processed Track Data
            //**********************

            static void LoadProcTrackData(const KTProcessedTrackData& ptData, Cicada::TProcessedTrackData& rootPTData);
            static void UnloadProcTrackData(KTProcessedTrackData& ptData, const Cicada::TProcessedTrackData& rootPTData);


            //*************************
            // Classifier Results Data
            //*************************

            static void LoadClassifierResultsData(const KTClassifierResultsData& crData, Cicada::TClassifierResultsData& rootCRData);
            static void UnloadClassifierResultsData(KTClassifierResultsData& crData, const Cicada::TClassifierResultsData& rootCRData);


            //********************
            // Processed MPT Data
            //********************

            static void LoadProcMPTData(const KTProcessedMPTData& ptData, Cicada::TProcessedMPTData& rootPTData);
            static void UnloadProcMPTData(KTProcessedMPTData& ptData, const Cicada::TProcessedMPTData& rootPTData);


            //************************
            // Multi-Track Event Data
            //************************

            static void LoadMultiTrackEventData(const KTMultiTrackEventData& mteData, Cicada::TMultiTrackEventData& rootMTEData);
            static void UnloadMultiTrackEventData(KTMultiTrackEventData& mteData, const Cicada::TMultiTrackEventData& rootMTEData);


            //**********************************
            // MTE Data with Classifier Results
            //**********************************

            static void LoadMTEWithClassifierResultsData(const KTMultiTrackEventData& mteData, Cicada::TMTEWithClassifierResultsData& rootMTECRData);
            static void UnloadMTEWithClassifierResultsData(KTMultiTrackEventData& mteData, const Cicada::TMTEWithClassifierResultsData& rootMTECRData);

            //*********************************
            // Sparse Waterfall Candidate Data
            //*********************************

            static void LoadSparseWaterfallCandidateData(const KTSparseWaterfallCandidateData& swfData, TSparseWaterfallCandidateData& rootSWfData);
            // static void UnloadSparseWaterfallCandidateData(KTSparseWaterfallCandidateData& swfData, const TSparseWaterfallCandidateData& rootSWfData);

            //*********************************
            // Sequential Line Data
            //*********************************

            static void LoadSequentialLineData(const KTSequentialLineData& seqData, TSequentialLineData& rootSEQData);
            // static void UnloadSequentialLineData(KTSequentialLineData& seqData, const TSequentialLineData& rootSEQData);

    };

} /* namespace Katydid */
#endif /* KT2ROOT_HH_ */
