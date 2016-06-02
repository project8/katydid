/**
 @file KTLinearDensityProbeFit.hh
 @brief Contains KTLinearDensityProbeFit
 @details Fits a set of 2D points to a line of maximum point density using a Gaussian error metric
 @author: E. Zayas
 @date: Nov 13, 2015
 */

#ifndef KTLINEARDENSITYPROBEFIT_HH
#define KTLINEARDENSITYPROBEFIT_HH

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include <vector>

namespace Katydid
{
    using namespace Nymph;
    class KTProcessedTrackData;
    class KTDiscriminatedPoints2DData;
    class KTLinearFitResult;

    /*!
     @class KTLinearDensityProbeFit
     @author E. Zayas

     @brief Fits a set of 2D points to a line of maximum point density using a Gaussian error metric

     @details
     The line is described by a slope (presumed known), and an intercept (unknown). Performs a brute-force search over the space of the intercept
     The search minimizes the function Exp[(y - q*x - a)^2/(2s^2)] summed over all 2D points (x, y)
     The slope q is provided by the track, the intercept a is determined by the minimization, and the width s is configured by the user
     The brute-force search over the intercept is comprised of a sweep with default step size s/5
     It is anticipated that with s << 1 MHz, the signal will be consistently found, and with s ~ 1 MHz, the sideband will be found

     Available configuration values:
     - "min-frequency": minimum frequency bound for intercept
     - "max-frequency": maximum frequency bound for intercept
     - "min-bin": bin associated with minimum frequency bound
     - "max-bin": bin associated with maximum frequency bound
     - "probe-width-big": 's' in the above description, the Gaussian width of the error metric
     - "probe-width-small": same as above for the 2nd component
     - "step-size-big": increment in the intercept sweep
     - "step-size-small": same as above for the 2nd component

     Slots:
     - "thresh-points": void (KTDataPtr) -- Performs fit analysis on a set of 2D Points; Requires KTProcessedTrackData and KTDiscriminatedPoints2DData; Adds KTLinearFitResult
     
     Signals:
     - "fit-result": void (KTDataPtr) Emitted upon minimization; Guarantees KTLinearFitResult
    */

    class KTLinearDensityProbeFit : public KTProcessor
    {
    	public:
            KTLinearDensityProbeFit(const std::string& name = "linear-density-fit");
            virtual ~KTLinearDensityProbeFit();

            bool Configure(const KTParamNode* node);

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

         	double GetStepSizeBig() const;
         	void SetStepSizeBig( double dalpha );

         	double GetStepSizeSmall() const;
         	void SetStepSizeSmall( double dalpha );

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            double fProbeWidthBig;
            double fProbeWidthSmall;
            double fStepSizeBig;
            double fStepSizeSmall;

        public:
        	bool Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts);
        	bool PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component=0);
        	double findIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width );

            //***************
            // Signals
            //***************

        private:
            KTSignalData fLinearDensityFitSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTProcessedTrackData, KTDiscriminatedPoints2DData > fThreshPointsSlot;
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

    inline double KTLinearDensityProbeFit::GetStepSizeBig() const
    {
    	return fStepSizeBig;
    }
    
    inline void KTLinearDensityProbeFit::SetStepSizeBig(double dalpha)
    {
    	fStepSizeBig = dalpha;
    	return;
    }

    inline double KTLinearDensityProbeFit::GetStepSizeSmall() const
    {
    	return fStepSizeSmall;
    }
    
    inline void KTLinearDensityProbeFit::SetStepSizeSmall(double dalpha)
    {
    	fStepSizeSmall = dalpha;
    	return;
    }


} /* namespace Katydid */

#endif /* KTLINEARDENSITYPROBEFIT_HH */