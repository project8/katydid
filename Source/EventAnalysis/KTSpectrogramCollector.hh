/*
 * KTSpectrogramCollector.hh
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#ifndef KTSPECTROGRAMCOLLECTOR_HH_
#define KTSPECTROGRAMCOLLECTOR_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"

#include "KTSpectrumCollectionData.hh"
#include "KTSliceHeader.hh"

#include <set>


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTDataAccumulator.hh");

    class KTPowerSpectrumData;
    class KTProcessedTrackData;
    class KTMultiPeakTrackData;
    class KTMultiTrackEventData;

    /*
     @class KTSpectrogramCollector
     @author E. Zayas
     @brief Collects the spectra that pertain to a particular track or event
     @details
     Supports an arbitrary number of tracks to collect simultaneously. Collection begins when a spectrum is received which matches the timestamp
     of the beginning of a track. A signal is emitted when the spectrum matches the end time.
     Configuration name: "spectrogram-collector"
     
     Available configuration values:
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency by bin
     - "max-bin": unsigned -- maximum frequency by bin
     - "lead-time": double -- time to collect before the beginning of the track
     - "trail-time": double -- time to collect after the end of the track
     - "lead-freq": double -- frequency below the track to begin collection
     - "trail-freq": double -- frequency above the track to end collection
     - "use-track-freqs": bool -- if true, the min/max frequencies are calculated from the track and the lead/trail frequencies; if false, min/max-frequency is used
     - "full-event": bool -- if true, collect the full spectrogram of an MP-event. If false, collect only the first track grouping (fEventSequenceID==0)
     
     Slots:
     - "track": void (Nymph::KTDataPtr) -- Adds a track to the list of active spectrogram collections; Requires KTProcessedTrackData; Adds nothing
     - "mp-track": void (Nymph::KTDataPtr) -- Adds a multi-peak track to the list of active spectrogram collections; Requires KTMultiPeakTrackData; Adds nothing
     - "mp-event": void (Nymph::KTDatPtr) -- Adds a multi-peak event to the list of active spectrogram collections; Requires KTMultiTrackEventData; Adds nothing
     - "ps": void (Nymph::KTDataPtr) -- Adds a power spectrum to the appropriate spectrogram(s), if any; Requires KTPowerSpectrumData and KTSliceHeader; Adds nothing
     
     Signals:
     - "ps-coll": void (Nymph::KTDataPtr) -- Emitted upon completion of a spectrogram (waterfall plot); Guarantees KTPSCollectionData
    */

    class KTSpectrogramCollector : public Nymph::KTProcessor
    {
        public:
            KTSpectrogramCollector(const std::string& name = "spectrogram-collector");
            virtual ~KTSpectrogramCollector();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE_NOSET(double, MinFrequency);
            MEMBERVARIABLE_NOSET(double, MaxFrequency);
            MEMBERVARIABLE_NOSET(unsigned, MinBin);
            MEMBERVARIABLE_NOSET(unsigned, MaxBin);
            MEMBERVARIABLE(double, LeadTime);
            MEMBERVARIABLE(double, TrailTime);
            MEMBERVARIABLE(double, LeadFreq);
            MEMBERVARIABLE(double, TrailFreq);
            MEMBERVARIABLE(bool, UseTrackFreqs);
            MEMBERVARIABLE(bool, FullEvent);
            MEMBERVARIABLE(double, PrevSliceTimeInRun);
            MEMBERVARIABLE(double, PrevSliceTimeInAcq);

        private:
            bool fCalculateMinBin;
            bool fCalculateMaxBin;

        public:
            bool AddTrack(KTProcessedTrackData& trackData, unsigned component);
            bool AddMPTrack(KTMultiPeakTrackData& mpTrackData, unsigned component);
            bool AddMPEvent(KTMultiTrackEventData& mpEventData, unsigned component);
            bool ConsiderSpectrum(KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component, bool forceEmit = false);
            bool ReceiveTrack(KTProcessedTrackData& data);
            bool ReceiveMPTrack(KTMultiPeakTrackData& data);
            bool ReceiveMPEvent(KTMultiTrackEventData& data);
            bool ReceiveSpectrum(KTPowerSpectrumData& data, KTSliceHeader& sliceData, bool forceEmit = false);
            void FinishSC( Nymph::KTDataPtr data );

            struct KTTrackCompare
            {
                bool operator() (const std::pair< Nymph::KTDataPtr, KTPSCollectionData* > lhs, const std::pair< Nymph::KTDataPtr, KTPSCollectionData* > rhs) const
                {
                    return lhs.second->GetStartTime() < rhs.second->GetStartTime();
                }
            };

            // The spectrograms are stored in a vector of sets of pairs of Nymph::KTDataPtr and KTPSCollectionData. The levels to this hierarchy are:
            //      Vector - each element corresponds to a component
            //      Set    - each element corresponds to a track
            //      Pair   - the Nymph::KTDataPtr which contains the spectrogram, and a pointer to the spectrogram
            // It is necessary to store the Nymph::KTDataPtr because the signal must contain this object when it emits, and each spectrogram must have a
            // unique associated Nymph::KTDataPtr

            std::vector< std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare > > fWaterfallSets;

        private:

            void SetMinFrequency( double freq );
            void SetMaxFrequency( double freq );
            void SetMinBin( unsigned bin );
            void SetMaxBin( unsigned bin );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fWaterfallSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTrackSlot;
            Nymph::KTSlotDataOneType< KTMultiPeakTrackData > fMPTrackSlot;
            Nymph::KTSlotDataOneType< KTMultiTrackEventData > fMPEventSlot;
            void SlotFunctionPSData( Nymph::KTDataPtr data );

    };

    void KTSpectrogramCollector::SlotFunctionPSData( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTPowerSpectrumData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTPowerSpectrumData >!");
            return;
        }
        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTSliceHeader >!");
            return;
        }

        // If the slice is the last, set a flag to force a signal emit
        bool force = data->GetLastData() || data->Of< KTSliceHeader >().GetIsNewAcquisition();
        if (force)
        {
            KTDEBUG(avlog_hh, "Reached last-data, forcing emit");
        }

        // Call the function
        if (! ReceiveSpectrum(data->Of< KTPowerSpectrumData >(), data->Of< KTSliceHeader >(), force))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing data with type < KTPSCollectionData >");
            return;
        }
    
        return;
    }

    inline void KTSpectrogramCollector::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline void KTSpectrogramCollector::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline void KTSpectrogramCollector::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline void KTSpectrogramCollector::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }
}

#endif /* KTSPECTROGRAMCOLLECTOR_HH_ */
