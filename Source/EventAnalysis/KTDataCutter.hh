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
    class KTEggHeader;

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
     - "min-track-id": unsigned -- minimum track ID to accept from KTProcessedTrackData
     - "max-track-id": unsigned -- maximum track ID to accept from KTProcessedTrackData
     - "cut-bogus-tracks": bool -- perform a cut on IsCut from KTProcessedTrackData
     - "is-cut": bool -- value of IsCut to accept from KTProcessedTrackData
     - "min-start-time-in-run-c": double -- minimum start time to accept from KTProcessedTrackData
     - "max-start-time-in-run-c": double -- maximum start time to accept from KTProcessedTrackData
     - "min-time-length": double -- minimum time length to accept from KTProcessedTrackData
     - "max-time-length": double -- maximum time length to accept from KTProcessedTrackData
     - "min-start-freq": double -- minimum starting frequency to accept from KTProcessedTrackData
     - "max-start-freq": double -- maximum starting frequency to accept from KTProcessedTrackData
     - "min-slope": double -- minimum slope to accept from KTProcessedTrackData
     - "max-slope": double -- maximum slope to accept from KTProcessedTrackData

     Slots:
     - "fit-result": void (Nymph::KTDataPtr) -- Cuts on a linear fit result; Requires KTLinearFitResult; Adds nothing
     - "track": void (Nymph::KTDataPtr) -- Cuts on a processed track; Requires KTProcessedTrackData; Adds nothing

     Signals:
     - "fit-result": void (Nymph::KTDataPtr) -- Emitted upon passing of a cut on KTLinearFitResult; Guarantees KTLinearFitResult
     - "track": void (Nymph::KTDataPtr) -- Emitted upon passing of a cut on KTProcessedTrackData; Guarantees KTProcessedTrackData
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

            unsigned GetMinTrackID() const;
            void SetMinTrackID(unsigned id);

            unsigned GetMaxTrackID() const;
            void SetMaxTrackID(unsigned id);

            bool GetCutBogusTracks() const;
            void SetCutBogusTracks(bool cut);

            bool GetIsCut() const;
            void SetIsCut(bool cut);

            double GetMinStartTimeInRunC() const;
            void SetMinStartTimeInRunC(double t);

            double GetMaxStartTimeInRunC() const;
            void SetMaxStartTimeInRunC(double t);

            double GetMinTimeLength() const;
            void SetMinTimeLength(double t);

            double GetMaxTimeLength() const;
            void SetMaxTimeLength(double t);

            double GetMinStartFreq() const;
            void SetMinStartFreq(double freq);

            double GetMaxStartFreq() const;
            void SetMaxStartFreq(double freq);

            double GetMinSlope() const;
            void SetMinSlope(double m);

            double GetMaxSlope() const;
            void SetMaxSlope(double m);

        private:
            double fMinSidebandSeparation;
            double fMaxSidebandSeparation;
            double fMinMagnetronFreq;
            double fMaxMagnetronFreq;
            unsigned fMinTrackID;
            unsigned fMaxTrackID;
            bool fCutBogusTracks;
            bool fIsCut;
            double fMinStartTimeInRunC;
            double fMaxStartTimeInRunC;
            double fMinTimeLength;
            double fMaxTimeLength;
            double fMinStartFreq;
            double fMaxStartFreq;
            double fMinSlope;
            double fMaxSlope;

        public:
            bool CutLinearFitResult( KTLinearFitResult& fitData );
            bool CutProcessedTrack( KTProcessedTrackData& trackData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fLinearFitSignal;
            Nymph::KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionFitResult( Nymph::KTDataPtr data );
            void SlotFunctionProcessedTrack( Nymph::KTDataPtr data );

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

    inline unsigned KTDataCutter::GetMinTrackID() const
    {
        return fMinTrackID;
    }

    inline void KTDataCutter::SetMinTrackID(unsigned id)
    {
        fMinTrackID = id;
        return;
    }

    inline unsigned KTDataCutter::GetMaxTrackID() const
    {
        return fMaxTrackID;
    }

    inline void KTDataCutter::SetMaxTrackID(unsigned id)
    {
        fMaxTrackID = id;
        return;
    }

    inline bool KTDataCutter::GetCutBogusTracks() const
    {
        return fCutBogusTracks;
    }

    inline void KTDataCutter::SetCutBogusTracks(bool cut)
    {
        fCutBogusTracks = cut;
        return;
    }

    inline bool KTDataCutter::GetIsCut() const
    {
        return fIsCut;
    }

    inline void KTDataCutter::SetIsCut(bool cut)
    {
        fIsCut = cut;
        return;
    }

    inline double KTDataCutter::GetMinStartTimeInRunC() const
    {
        return fMinStartTimeInRunC;
    }

    inline void KTDataCutter::SetMinStartTimeInRunC(double t)
    {
        fMinStartTimeInRunC = t;
        return;
    }

    inline double KTDataCutter::GetMaxStartTimeInRunC() const
    {
        return fMaxStartTimeInRunC;
    }

    inline void KTDataCutter::SetMaxStartTimeInRunC(double t)
    {
        fMaxStartTimeInRunC = t;
        return;
    }

    inline double KTDataCutter::GetMinTimeLength() const
    {
        return fMinTimeLength;
    }

    inline void KTDataCutter::SetMinTimeLength(double t)
    {
        fMinTimeLength = t;
        return;
    }

    inline double KTDataCutter::GetMaxTimeLength() const
    {
        return fMaxTimeLength;
    }

    inline void KTDataCutter::SetMaxTimeLength(double t)
    {
        fMaxTimeLength = t;
        return;
    }

    inline double KTDataCutter::GetMinStartFreq() const
    {
        return fMinStartFreq;
    }

    inline void KTDataCutter::SetMinStartFreq(double freq)
    {
        fMinStartFreq = freq;
        return;
    }

    inline double KTDataCutter::GetMaxStartFreq() const
    {
        return fMaxStartFreq;
    }

    inline void KTDataCutter::SetMaxStartFreq(double freq)
    {
        fMaxStartFreq = freq;
        return;
    }

    inline double KTDataCutter::GetMinSlope() const
    {
        return fMinSlope;
    }

    inline void KTDataCutter::SetMinSlope(double m)
    {
        fMinSlope = m;
        return;
    }

    inline double KTDataCutter::GetMaxSlope() const
    {
        return fMaxSlope;
    }

    inline void KTDataCutter::SetMaxSlope(double m)
    {
        fMaxSlope = m;
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
        KTINFO(avlog_hh, "Linear fit passed cut; emitting signal");
        fLinearFitSignal( data );
    
        return;
    }

    void KTDataCutter::SlotFunctionProcessedTrack( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        // Call function
        if( !CutProcessedTrack( data->Of< KTProcessedTrackData >() ) )
        {
            KTINFO(avlog_hh, "Processed track failed cut; suppressing signal");
            return;
        }

        // Emit signal
        KTINFO(avlog_hh, "Processed track passed cut; emitting signal");
        fTrackSignal( data );
    
        return;
    }
}

#endif /* KTDATACUTTER_HH_ */
