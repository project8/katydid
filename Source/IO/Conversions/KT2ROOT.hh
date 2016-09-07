/*
 * KT2ROOT.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KT2ROOT_HH_
#define KT2ROOT_HH_

#include <string>

#include "KTPhysicalArray.hh"
#include "KTVarTypePhysicalArray.hh"

class TH1I;
class TH1D;
class TH2D;

namespace Katydid
{
    using namespace Nymph;
    class KTFrequencySpectrumFFTW;
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;
    class KTTimeSeriesDist;
    class KTTimeSeriesFFTW;
    class KTTimeSeriesReal;
    class KTScoredSpectrum;

    class KT2ROOT
    {
        public:
            KT2ROOT();
            virtual ~KT2ROOT();

            static TH1I* CreateHistogram(const KTVarTypePhysicalArray< uint64_t >* ts, const std::string& histName = "hRawTimeSeries");
            static TH1I* CreateHistogram(const KTVarTypePhysicalArray< int64_t >* ts, const std::string& histName = "hRawTimeSeries");
            static TH1I* CreateHistogram(const KTTimeSeriesDist* tsDist, const std::string& histName = "hTSDist");
            //static TH1I* CreateAmplitudeDistributionHistogram(const KTRawTimeSeries* ts, const std::string& histName = "hRawTSDist");

            static TH1D* CreateHistogram(const KTTimeSeriesFFTW* ts, const std::string& histName = "hTimeSeries");
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


            //***************
            // Power Spectrum
            //***************

            static TH1D* CreatePowerHistogram(const KTPowerSpectrum* ps, const std::string& name = "hPowerSpectrum");
            static TH1D* CreatePowerDistributionHistogram(const KTPowerSpectrum* ps, const std::string& name = "hPowerSpectrumPower");

            static TH2D* CreateHistogram(const KTPhysicalArray< 2, double >* ht, const std::string& histName = "hHoughData");

            static TH1D* CreateScoredHistogram(const KTScoredSpectrum* ps, const std::string& name = "hScoredSpectrum");
    };

} /* namespace Katydid */
#endif /* KT2ROOT_HH_ */
