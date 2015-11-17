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
     - MinFrequency: minimum frequency bound for intercept
     - MaxFrequency: maximum frequency bound for intercept
     - MinBin: bin associated with minimum frequency bound
     - MaxBin: bin associated with maximum frequency bound
     - ProbeWidth: 's' in the above description, the Gaussian width of the error metric
     - StepSize: increment in the intercept sweep

     Slots:
     - "thresh": void (KTDataPtr) -- Requires KTProcessedTrackData, KTDiscriminatedPoints2DData; Adds KTLinearFitResult
     
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

         	double GetProbeWidth() const;
         	void SetProbeWidth( double sigma );

         	double GetStepSize() const;
         	void SetStepSize( double dalpha );

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            double fProbeWidth;
            double fStepSize;

        public:
        	bool KTLinearDensityProbeFit::Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts);
        	
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

    inline double KTLinearDensityProbeFit::GetProbeWidth() const
    {
    	return fProbeWidth;
    }

    inline void KTLinearDensityProbeFit::SetProbeWidth(double sigma)
    {
    	fProbeWidth = sigma;
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
} /* namespace Katydid */

#endif /* KTLINEARDENSITYPROBEFIT_HH */