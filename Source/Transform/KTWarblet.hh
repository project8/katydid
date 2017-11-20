/*
 * KTWarblet.hh
 *
 *  Created on: Nov 20, 2017
 *      Author: buzinsky
 */

#ifndef KTWARBLET_HH_
#define KTWARBLET_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTWarblet.hh");

    class KTSliceHeader;
    class KTEggHeader;
    class KTFrequencySpectrumDataFFTW;
    class KTProcessedTrackData;
    class KTTimeSeriesData;

    /*
     @class KTWarblet
     @author N. Buzinsky

     @brief Multiplies a time series by a warbling phase like exp( -i a*cos(2*pi*f_m *t) ) to demodulate signal

     @details
     This code is a sad ripoff of the KTQuadraticPhaseShift. It is meant as a proof of concept.
     I need to put in configuration where user can specify trap. Ie bathtub24 with a particular depth!!!

     Configuration name: "warblet"

     Available configuration values:
     - "pitch": double -- value of the pitch angle of the electron you are looking for
     - "axial_phase": double -- value of phase angle for doppler modulation itself

     Slots:
     - "ts-dewarble": void (Nymph::KTDataPtr) -- Multiplies a time series by the warbling phase; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing

     Signals:
     - "time-series": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData and KTSliceHeader
    */

    class KTWarblet : public Nymph::KTProcessor
    {
        public:
            KTWarblet(const std::string& name = "warblet");
            virtual ~KTWarblet();

            bool Configure(const scarab::param_node* node);


            double GetPitchAngle() const;
            void SetPitchAngle(double alpha);

            double GetAxialPhase() const;
            void SetAxialPhase(double alpha);

            std::string GetMethod() const;
            void SetMethod(std::string method);

        private:
            double fPitchAngle;
            double fAxialPhase;
            std::string fMethod;
        
            double fTimeBinWidth;
            double fFrequencyBinWidth;

        public:
            bool DewarbleTS( KTTimeSeriesData& tsData, KTSliceHeader& slice );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;
            Nymph::KTSignalData fFSSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionTS( Nymph::KTDataPtr data );
            void SlotFunctionFS( Nymph::KTDataPtr data );
            void SlotFunctionHeader( Nymph::KTDataPtr data );

    };

    inline double KTWarblet::GetPitchAngle() const
    {
        return fPitchAngle;
    }

    inline void KTWarblet::SetPitchAngle(double alpha)
    {
        fPitchAngle = alpha;
        return;
    }

    inline double KTWarblet::GetAxialPhase() const
    {
        return fAxialPhase;
    }

    inline void KTWarblet::SetAxialPhase(double alpha)
    {
        fAxialPhase = alpha;
        return;
    }

    inline std::string KTWarblet::GetMethod() const
    {
        return fMethod;
    }

    inline void KTWarblet::SetMethod(std::string method)
    {
        fMethod = method;
        return;
    }
    
}

#endif /* KTWARBLET_HH_ */
