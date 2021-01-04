/*
 * KTFSCDCRESUtils.hh
 *
 *  Created on: Dec 4, 2020
 *      Author: P. T. Surukuchi
 *
 */

#ifndef KTFSCDCRESUTILS_HH_
#define KTFSCDCRESUTILS_HH_

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTMemberVariable.hh"

#include "KTFrequencySpectrumFFTW.hh"

#include "param.hh"

namespace Katydid
{    
    //TODO: add comments and details here
    /*
     @class KTFSCDCRESUtils
     @author P. T. Surukuchi
     
     @brief Multiple channel summation for Phase-III and IV
     
     @details
     More details to come.
     
     Configuration name: "channel-aggregator"
     
     Available configuration options:
     - "wavelength": double -- Wavelength of the cyclotron motion
     - "active-radius": double -- The active radius of the detection volume
     - "use-antispiral-phase-shifts": bool, -- A flag to indicate whether to use antispiral phase shifts
     - "pitch-angle": double, -- The pitch angle of electron, not used currently
     - "gradb-frequency": double, -- Frequency of grad-B motion used in analysis of simulated electrons 
     */


    class KTFSCDCRESUtils
    {
        public:
            KTFSCDCRESUtils(const std::string& name = "fscd-cres-utils");
            virtual ~KTFSCDCRESUtils();

            // PTS: This in reality needs to come from the frequency being used for the phase shift
            MEMBERVARIABLE(double, Wavelength);

            // PTS: This is a temporary variable, in reality needs to a search parameter for the Grad-B motion
            MEMBERVARIABLE(double, PitchAngle);

            // in meters, should not be hard-coded
            MEMBERVARIABLE(double, ArrayRadius);

	        //AN electron undergoiing cyclotron motion has a spiral motion and not all receving channels are in phase.
	        //If selected this option will make sure that there is a relative phase-shift applied 
	        MEMBERVARIABLE(bool,ApplyAntiSpiralPhaseShifts);
            // PTS: This is a temporary variable, in reality we can't measure this value, this will need to be calculated based on the postion of the electron 
            // Lot more work can be done here
            MEMBERVARIABLE(double, GradBFrequency);

            //Apply GrdB-dependent frequency shifts 
	        MEMBERVARIABLE(bool,ApplyGradBDopplerFreqShifts);
	        MEMBERVARIABLE(bool,ApplyGradBNormalFreqShifts);
            MEMBERVARIABLE(bool,ApplyFreqShifts);

        public:
            bool Configure(const scarab::param_node* node);
            /// Returns the phase shift based on a given point, angle of the channel and the wavelength
//            double GetDistanceDependentPhi(double xPosition, double yPosition, double wavelength, double channelAngle) const;

            /// Convert frquency to wavlength
            double ConvertFrequencyToWavelength(double frequency) const;

            /// Convert wavlength to frequency 
            double ConvertWavelengthToFrequency(double wavelength) const;

            /// Returns the magnitude of beta based on the angle between the vector joining the channel to the electron position and the electron's velocity vector 
            double GetGradBBeta(double xPosition, double yPosition, double channelX, double channelY) const;

            /// Apply shift phase to the supplied points based on the phase provided
            bool ApplyPhaseShift(double &realVal, double &imagVal, double phase);
	    
            /// Returns the phase shift based on a given point, angle of the channel and the wavelength
            double GetPhaseShift(double xPosition, double yPosition, double channelAngle) const;

            /// Returns the phase shift based on the normal vector between the channel and the point, this is different from the position dependent phase shift
            double GetAntiSpiralPhaseShift(double xPosition, double yPosition, double channelAngle) const;

            /// Returns the apparent frequency shift based Grad-B dependent doppler motion
            double GetGradBDopplerFreqShift(double xPosition, double yPosition, double channelX, double channelY) const;

            /// Returns the frequency shift based on Grad-B dependent normal vector, manifests aa frequency shift
            double GetGradBNormalFreqShift(double xPosition, double yPosition, double channelX, double channelY) const;

            /// Get location of the point in the grid based on the given grid number and the size of the grid.
            /* Returns true if the assigment went well, false if there was some mistake
             */
            bool GetGridLocation(int gridNumber, int gridSize, double &gridLocation);

            /// Apply a frequency shift to the input spectrum 
            /* A shift of #freqShift is applied to the input #freqSpectrum and is saved to #shiftedFreqSpectrum. The input freqSpectrum is not modified
             * The shift is applied only in the range of #minFreq to # maxFreq
     */
            bool ApplyFrequencyShift(const KTFrequencySpectrumFFTW &freqSpectrum,KTFrequencySpectrumFFTW &shiftedFreqSpectrum ,double freqShift, double minFreq, double maxFreq) const;
            
            /// Apply a frequency shift to the input spectrum 
            /* A shift of #freqShift is applied to the input #freqSpectrum and is saved to itself
             * The shift is applied only in the range of #minFreq to # maxFreq
     */
            bool ApplyFrequencyShift(KTFrequencySpectrumFFTW &freqSpectrum,double freqShift, double minFreq, double maxFreq) const;
            
            bool ApplyFrequencyShifts(const KTFrequencySpectrumFFTW &freqSpectrum,KTFrequencySpectrumFFTW &shiftedFreqSpectrum,double xPosition, double yPosition, double channelAngle, double minFreq, double maxFreq) const;

            bool ApplyFrequencyShifts(KTFrequencySpectrumFFTW &freqSpectrum,double xPosition, double yPosition, double channelAngle, double minFreq, double maxFreq) const;

            //PTS: This needs fixing, currently just setting each element to 0. But why does it have to be done to begin with.
            // Perhaps there is some function in the utilities to do this ?
            bool NullFreqSpectrum(KTFrequencySpectrumFFTW &freqSpectrum);

            double C=299792458;//m/s

            double testConstant=0;
    };
    inline bool KTFSCDCRESUtils::NullFreqSpectrum(KTFrequencySpectrumFFTW &freqSpectrum)
    {
        for (unsigned i = 0; i < freqSpectrum.size(); ++i)
        {
            freqSpectrum.SetRect(i, 0.0, 0.0);
        }
        return true;
    }

} /* namespace Katydid */
#endif /* KTFSCDCRESUTILS_HH_ */
