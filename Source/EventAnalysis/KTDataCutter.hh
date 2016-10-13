/*
 * KTDataCutter.hh
 *
 *  Created on: Oct 13, 2016
 *      Author: ezayas
 */

#ifndef KTDATACUTTER_HH_
#define KTDATACUTTER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTDataCutter.hh");

    class KTLinearFitResult;
    class KTProcessedTrackData;

    /*
     @class KTDataCutter
     @author E. Zayas

     @brief Makes cuts on data objects based on member variables

     @details
     This processor performs cuts on a data object according to configurable ranges of member variables which are to be accepted. If the object passes
     the cut, it will emit a signal containing the same KTDataPtr it was passed. If it does not, no signal will be emitted. Note that the list of
     configuration values contains member variables associated to any data object which a slot can accept.

     Configuration name: "data-cutter"

     Available configuration values:
     - "min-sideband-separation": double -- minimum sideband separation to accept from KTLinearFitResult
     - "max-sideband-separation": double -- maximum sideband separation to accept from KTLinearFitResult
     - "min-magnetron-freq": double -- minimum magnetron frequency to accept from KTLinearFitResult
     - "max-magnetron-freq": double -- maximum magnetron frequency to accept from KTLinearFitResult

     Slots:
     - "fit-result": void (Nymph::KTDataPtr) -- Cuts on a linear fit result; Requires KTLinearFitResult; Adds nothing

     Signals:
     - "fit-result": void (Nymph::KTDataPtr) -- Emitted upon passing of a cut on KTLinearFitResult; Guarantees KTLinearFitResult
    */

    class KTDataCutter : public Nymph::KTProcessor
    {
        public:
            KTDataCutter(const std::string& name = "data-cutter");
            virtual ~KTDataCutter();

            bool Configure(const scarab::param_node* node);

            double GetMinSidebandSeparation() const;
            void SetMinSidebandSeparation(double freq);

            double GetMaxSidebandSeparation() const;
            void SetMaxSidebandSeparation(double freq);

            double GetMinMagnetronFreq() const;
            void SetMinMagnetronFreq(double freq);

            double GetMaxMagnetronFreq() const;
            void SetMaxMagnetronFreq(double freq);

        private:
            double fMinSidebandSeparation;
            double fMaxSidebandSeparation;
            double fMinMagnetronFreq;
            double fMaxMagnetronFreq;

        public:
            bool CutLinearFitResult( KTLinearFitResult& fitData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLinearFitSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionFitResult( Nymph::KTDataPtr data );

    };

    inline double KTDataCutter::GetMinSidebandSeparation() const
    {
        return fMinSidebandSeparation;
    }

    inline void KTDataCutter::SetMinSidebandSeparation(double freq)
    {
        fMinSidebandSeparation = freq;
        return;
    }

    inline double KTDataCutter::GetMaxSidebandSeparation() const
    {
        return fMaxSidebandSeparation;
    }

    inline void KTDataCutter::SetMaxSidebandSeparation(double freq)
    {
        fMaxSidebandSeparation = freq;
        return;
    }

    inline double KTDataCutter::GetMinMagnetronFreq() const
    {
        return fMinMagnetronFreq;
    }

    inline void KTDataCutter::SetMinMagnetronFreq(double freq)
    {
        fMinMagnetronFreq = freq;
        return;
    }

    inline double KTDataCutter::GetMaxMagnetronFreq() const
    {
        return fMaxMagnetronFreq;
    }

    inline void KTDataCutter::SetMaxMagnetronFreq(double freq)
    {
        fMaxMagnetronFreq = freq;
        return;
    }

    void KTDataCutter::SlotFunctionFitResult( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTLinearFitResult >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTLinearFitResult >!");
            return;
        }

        // Call function
        if( !CutLinearFitResult( data->Of< KTLinearFitResult >() ) )
        {
            KTINFO(avlog_hh, "Linear fit failed cut; suppressing signal");
            return;
        }

        // Emit signal
        fLinearFitSignal( data );
    
        return;
    }
}

#endif /* KTDATACUTTER_HH_ */
