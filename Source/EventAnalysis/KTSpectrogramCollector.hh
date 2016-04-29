/*
 * KTSpectrogramCollector.hh
 *
 *  Created on: Oct 9, 2015
 *      Author: nsoblath
 */

#ifndef KTSPECTROGRAMCOLLECTOR_HH_
#define KTSPECTROGRAMCOLLECTOR_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"

#include "KTSpectrumCollectionData.hh"

#include <set>


namespace Katydid
{
    using namespace Nymph;
    class KTPowerSpectrumData;
    class KTProcessedTrackData;
    class KTSliceHeader;
    //class KTDataPtr;

    /*!
     @class KTSpectrogramCollector
     @author N.S. Oblath

     @brief Collects the spectra that pertain to a particular track or event

     @details
     [detailed class description]

     Configuration name: "spectrogram-collector"

     Available configuration values:
     - "min-frequency": double -- minimum frequency
     - "max-frequency": double -- maximum frequency
     - "min-bin": unsigned -- minimum frequency by bin
     - "max-bin": unsigned -- maximum frequency by bin
     - "lead-time": double -- time to collect before the beginning of the track
     - "trail-time": double -- time to collect after the end of the track

     Slots:
     - "track": void (KTDataPtr) -- Adds a track to the list of active spectrogram collections; Requires KTProcessedTrackData; Adds nothing
     - "ps": void (KTDataPtr) -- Adds a power spectrum to the appropriate spectrogram(s), if any; Requires KTPowerSpectrumData and KTSliceHeader; Adds nothing

     Signals:
     - "waterfall": void (KTDataPtr) -- Emitted upon completion of a spectrogram (waterfall plot); Guarantees KTPSCollectionData
    */

    class KTSpectrogramCollector : public KTProcessor
    {
        public:
            KTSpectrogramCollector(const std::string& name = "spectrogram-collector");
            virtual ~KTSpectrogramCollector();

            bool Configure(const KTParamNode* node);

            double GetMinFrequency() const;
            void SetMinFrequency(double freq);

            double GetMaxFrequency() const;
            void SetMaxFrequency(double freq);

            unsigned GetMinBin() const;
            void SetMinBin(unsigned bin);

            unsigned GetMaxBin() const;
            void SetMaxBin(unsigned bin);

            double GetLeadTime() const;
            void SetLeadTime(double t);

            double GetTrailTime() const;
            void SetTrailTime(double t);

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            double fLeadTime;
            double fTrailTime;

        public:
            bool AddTrack(KTProcessedTrackData& trackData, unsigned component);
            bool ConsiderSpectrum(KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component);
            bool ReceiveTrack(KTProcessedTrackData& data);
            bool ReceiveSpectrum(KTPowerSpectrumData& data, KTSliceHeader& sliceData);
            void FinishSC( KTDataPtr data );

            struct KTTrackCompare
            {
                bool operator() (const std::pair< KTDataPtr, KTPSCollectionData* > lhs, const std::pair< KTDataPtr, KTPSCollectionData* > rhs) const
                {
                    return lhs.second->GetStartTime() < rhs.second->GetStartTime();
                }
            };

            std::vector< std::set< std::pair< KTDataPtr, KTPSCollectionData* >, KTTrackCompare > > fWaterfallSets;

        private:
            // Perhaps there are some non-public helper functions?

            // And then any non-configurable member variables

            //***************
            // Signals
            //***************

        private:
            KTSignalData fWaterfallSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< KTProcessedTrackData > fTrackSlot;
            KTSlotDataTwoTypes< KTPowerSpectrumData, KTSliceHeader > fPSSlot;

    };

    inline double KTSpectrogramCollector::GetMinFrequency() const
    {
        return fMinFrequency;
    }

    inline void KTSpectrogramCollector::SetMinFrequency(double freq)
    {
        fMinFrequency = freq;
        fCalculateMinBin = true;
        return;
    }

    inline double KTSpectrogramCollector::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTSpectrogramCollector::SetMaxFrequency(double freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline unsigned KTSpectrogramCollector::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTSpectrogramCollector::SetMinBin(unsigned bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline unsigned KTSpectrogramCollector::GetMaxBin() const
    {
        return fMaxBin;
    }

    inline void KTSpectrogramCollector::SetMaxBin(unsigned bin)
    {
        fMaxBin = bin;
        fCalculateMaxBin = false;
        return;
    }

    inline double KTSpectrogramCollector::GetLeadTime() const
    {
        return fLeadTime;
    }

    inline void KTSpectrogramCollector::SetLeadTime(double t)
    {
        fLeadTime = t;
        return;
    }

    inline double KTSpectrogramCollector::GetTrailTime() const
    {
        return fTrailTime;
    }

    inline void KTSpectrogramCollector::SetTrailTime(double t)
    {
        fTrailTime = t;
        return;
    }
}

#endif /* KTTRACKSPECTROGRAMCOLLECTOR_HH_ */
