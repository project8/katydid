/*
 @file KTLinearDensityProbeFit.hh
 @brief Contains KTLinearDensityProbeFit
 @details Contains two algorithms which analyze the density of thresholded points along a linear track in a spectrogram
 @author: E. Zayas
 @date: Nov 13, 2015
 */

#ifndef KTLINEARDENSITYPROBEFIT_HH
#define KTLINEARDENSITYPROBEFIT_HH

#include "KTGainVariationData.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessor.hh"

#include "KTSlot.hh"
#include "logger.hh"
#include "KTMemberVariable.hh"

#include <vector>

namespace Katydid
{
    LOGGER(avlog_hh, "KTLinearDensityProbeFit.hh");

    /// function to evaluate gaussian function
    inline double GausEval( double arg, double sigma )
    {
        return exp( -0.5*arg*arg/(sigma*sigma) );
    }

    /// Function to define a TF1
    /// Sum of npeaks Gaussian peaks on a linear background
    /// Background is specified by par[0] and par[1]
    /// Gaussian peaks are specified by all other elements of par
    /// npeaks is initialized in the header file
    double SumOfGaussians( double *x, double *par );


    class KTProcessedTrackData;
    class KTDiscriminatedPoints2DData;
    class KTLinearFitResult;
    class KTPSCollectionData;

    /*!
     @class KTLinearDensityProbeFit
     @author E. Zayas

     @brief Contains two algorithms which analyze the density of thresholded points along a linear track in a spectrogram

     @details
     The slope of the linear track is presumed to be known exactly. These algorithms perform a brute-force sweep over the space of the intercept
     The "density" associated to an intercept 'a' is Exp[(y - q*x - a)^2/(2s^2)] summed over all 2D points (x, y)
     The slope q is provided by the track, the intercept a is varied, and the width s is a configurable variable
     The brute-force search has a step size which is also configurable, and defaults to 20% of the narrow 's' value

     The two algorithms are outlined below:

     (1)    Density maximization with two trial values of 's' (wide and narrow). This algorithm should be used on a spectrogram which spans the full
            applicable frequency range, i.e. 50MHz - 150MHz or some slightly narrower band within. The frequency range should be blind to the location of
            any tracks. The range of 'a' is specified by fMin/MaxFrequency, and the value of 'a' with the largest density for each 's' is chosen as a
            best-fit value. It is anticipated that a large value of s ~ 1MHz will consistently assign the best-fit intercept at a sideband track, and a
            small (~100kHz) value will consistently locate a mainband signal track. The best-fit intercept and other results of each sweep are stored
            in KTLinearFitResult, with component 0 corresponding to the wide 's' and component 1 corresponding to the narrow 's'. If the best-fit intercept
            differs between these two components, then the spectrogram likely contains both a signal and sideband which have now been correctly identified.
            This algorithm also performs fourier analysis on the sideband candidate track, to search for a magnetron peak. The results are written in the
            KTLinearFitResult object as well.

    (2)     Analysis of rotated-and-projected power spectrum. This algorithm instead examines a spectrogram which contains only one track, and spans a
            small frequency range surrounding that track. The same intercept sweep is used, this time only with the narrow value of 's' and a range which is
            determined by the spectrogram. All values are binned into a 1D power spectrum which is effectively the total power vs. frequency after the
            spectrum has been rotated to set the track horizontal (q=0) and integrated over time. Peak finding analysis is performed on this spectrum (some
            parameters are configurable), and central moments up to the 4th (kurtosis) are calculated. The results are stored in KTPowerFitData. It is
            anticipated that these results will differ between signal and sideband peaks, in a way which is not necessarily simple.

     Available configuration values:
     - "do-density-maximization": bool -- whether or not to perform the density maximization algorithm
     - "do-projection-analysis": bool -- whether or not to perform the rotate-and-project analysis
     - "min-frequency": double -- minimum frequency bound for intercept
     - "max-frequency": double -- maximum frequency bound for intercept
     - "probe-width-big": double -- wide value of 's' in the above description, the Gaussian width of the error metric
     - "probe-width-small": double -- narrow value of 's'
     - "step-size": double -- increment in the intercept sweep
     - "spectrum-tolerance": double -- 'sigma' in the TSpectrum::Search function; roughly the minimum number of bins which must separate distinct peaks
     - "spectrum-threshold": double -- 'threshold' in the TSpectrum::Search function; peaks with amplitude less than threshold*highest_peak are discarded

     Slots:
     - "thresh-points": void (Nymph::KTDataPtr) -- Performs fit analysis on a set of 2D Points; Requires KTProcessedTrackData, KTDiscriminatedPoints2DData, and KTPSCollectionData; Adds KTLinearFitResult
     - "gv": void (Nymph::KTDataPtr) -- Stores gain variation for later use with spectrogram; Requires KTGainVariationData

     Signals:
     - "density-fit": void (Nymph::KTDataPtr) -- Emitted upon completion of density maximization; Guarantees KTLinearFitResult
     - "power-fit": void (Nymph::KTDataPtr) -- Emitted upon completion of power analysis; Guarantees KTPowerFitData
     */

    class KTLinearDensityProbeFit : public Nymph::KTProcessor
    {
        public:
            KTLinearDensityProbeFit(const std::string& name = "linear-density-fit");
            virtual ~KTLinearDensityProbeFit();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(bool, DoDensityMaximization);
            MEMBERVARIABLE(bool, DoProjectionAnalysis);

            MEMBERVARIABLE(double, MinFrequency);
            MEMBERVARIABLE(double, MaxFrequency);

            MEMBERVARIABLE(double, ProbeWidthBig);
            MEMBERVARIABLE(double, ProbeWidthSmall);

            MEMBERVARIABLE(double, StepSize);
            MEMBERVARIABLE(double, Tolerance);
            MEMBERVARIABLE(double, Threshold);

        public:
            bool ChooseAlgorithm(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool SetPreCalcGainVar(KTGainVariationData& gvData);
            bool DensityMaximization(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool ProjectionAnalysis(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component=0);
            double FindIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width );

        private:
            KTGainVariationData fGVData;

            int fNPeaks;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLinearDensityFitSignal;
            Nymph::KTSignalData fPowerFitSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionThreshPoints( Nymph::KTDataPtr data );
            Nymph::KTSlotDataOneType< KTGainVariationData > fPreCalcSlot;
    };

} /* namespace Katydid */

#endif /* KTLINEARDENSITYPROBEFIT_HH */
