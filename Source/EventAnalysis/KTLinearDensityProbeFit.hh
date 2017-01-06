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
#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessor.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"

#include <vector>

namespace Katydid
{
    KTLOGGER(avlog_hh, "KTLinearDensityProbeFit.hh");

    class KTProcessedTrackData;
    class KTDiscriminatedPoints2DData;
    class KTLinearFitResult;

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
     - "min-frequency": double -- minimum frequency bound for intercept
     - "max-frequency": double -- maximum frequency bound for intercept
     - "min-bin": unsigned -- bin associated with minimum frequency bound
     - "max-bin": unsigned -- bin associated with maximum frequency bound
     - "probe-width-big": double -- wide value of 's' in the above description, the Gaussian width of the error metric
     - "probe-width-small": double -- narrow value of 's'
     - "step-size": double -- increment in the intercept sweep
     - "spectrum-tolerance": double -- 'sigma' in the TSpectrum::Search function; roughly the minimum number of bins which must separate distinct peaks
     - "spectrum-threshold": double -- 'threshold' in the TSpectrum::Search function; peaks with amplitude less than threshold*highest_peak are discarded
     - "algorithm": unsigned -- specifies which algorithm to use, 1 or 2. Any other value will default to performing both algorithms.

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

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

         	double GetProbeWidthBig() const;
         	void SetProbeWidthBig( double sigma );

         	double GetProbeWidthSmall() const;
         	void SetProbeWidthSmall( double sigma );

         	double GetStepSize() const;
         	void SetStepSize( double dalpha );

            double GetTolerance() const;
            void SetTolerance( double sigma );

            double GetThreshold() const;
            void SetThreshold( double threshold );            

         	unsigned GetAlgorithm() const;
         	void SetAlgorithm( unsigned alg );

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            double fProbeWidthBig;
            double fProbeWidthSmall;
            double fStepSize;
            double fTolerance;
            double fThreshold;
            unsigned fAlgorithm;

            int npeaks;

        public:
            bool ChooseAlgorithm(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool SetPreCalcGainVar(KTGainVariationData& gvData);
        	bool DensityMaximization(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool ProjectionAnalysis(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram);
            bool PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component=0);
        	double findIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width );

        private:
            KTGainVariationData fGVData;

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

    inline double KTLinearDensityProbeFit::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTLinearDensityProbeFit::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        return;
    }

    inline double KTLinearDensityProbeFit::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTLinearDensityProbeFit::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        return;
    }

    inline unsigned KTLinearDensityProbeFit::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTLinearDensityProbeFit::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTLinearDensityProbeFit::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTLinearDensityProbeFit::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    inline double KTLinearDensityProbeFit::GetProbeWidthBig() const
    {
    	return fProbeWidthBig;
    }

    inline void KTLinearDensityProbeFit::SetProbeWidthBig(double sigma)
    {
    	fProbeWidthBig = sigma;
    	return;
    }

    inline double KTLinearDensityProbeFit::GetProbeWidthSmall() const
    {
    	return fProbeWidthSmall;
    }

    inline void KTLinearDensityProbeFit::SetProbeWidthSmall(double sigma)
    {
    	fProbeWidthSmall = sigma;
    	return;
    }

    inline double KTLinearDensityProbeFit::GetStepSize() const
    {
    	return fStepSize;
    }
    
    inline void KTLinearDensityProbeFit::SetStepSize(double dalpha)
    {
    	fStepSize = dalpha;
    	return;
    }

    inline double KTLinearDensityProbeFit::GetTolerance() const
    {
        return fTolerance;
    }
    
    inline void KTLinearDensityProbeFit::SetTolerance(double sigma)
    {
        fTolerance = sigma;
        return;
    }

    inline double KTLinearDensityProbeFit::GetThreshold() const
    {
        return fThreshold;
    }
    
    inline void KTLinearDensityProbeFit::SetThreshold(double threshold)
    {
        fThreshold = threshold;
        return;
    }

    inline unsigned KTLinearDensityProbeFit::GetAlgorithm() const
    {
    	return fAlgorithm;
    }
    
    inline void KTLinearDensityProbeFit::SetAlgorithm(unsigned alg)
    {
    	fAlgorithm = alg;
    	return;
    }

    void KTLinearDensityProbeFit::SlotFunctionThreshPoints( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }
        if (! data->Has< KTDiscriminatedPoints2DData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTDiscriminatedPoints2DData >!");
            return;
        }
        if (! data->Has< KTPSCollectionData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTPSCollectionData >!");
            return;
        }

        // Call the function
        if( !ChooseAlgorithm( data->Of< KTProcessedTrackData >(), data->Of< KTDiscriminatedPoints2DData >(), data->Of< KTPSCollectionData >() ) )
        {
            KTERROR(avlog_hh, "Density probe analysis failed.");
            return;
        }

        // Emit appropriate signal
        if( fAlgorithm != 2 )
        {
            fLinearDensityFitSignal( data );
        }
        if( fAlgorithm != 1 )
        {
            fPowerFitSignal( data );
        }
    
        return;
    }


} /* namespace Katydid */

#endif /* KTLINEARDENSITYPROBEFIT_HH */
