/*
 * KTSpectrogramCollector.hh
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#ifndef KTSPECTROGRAMCOLLECTOR_HH_
#define KTSPECTROGRAMCOLLECTOR_HH_

#include "KTProcessor.hh"

#include "KTData.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTProcessedTrackData.hh"
#include "KTSlot.hh"

#include "KTSpectrumCollectionData.hh"

#include <set>
#include <map>
#include <typeinfo>
#include <vector>


namespace Katydid
{
    using namespace Nymph;
    KTLOGGER(avlog_hh, "KTSpectrogramCollector.hh");

    class KTPowerSpectrumData;
    class KTSliceHeader;

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

     Slots:
     - "track": void (KTDataPtr) -- Adds a track to the list of active spectrogram collections; Requires KTProcessedTrackData; Adds nothing
     - "ps": void (KTDataPtr) -- Adds a power spectrum to the appropriate spectrogram(s), if any; Requires KTPowerSpectrumData and KTSliceHeader; Adds nothing

     Signals:
     - "waterfall": void (KTDataPtr) -- Emitted upon completion of a spectrogram (waterfall plot); Guarantees KTPSCollectionData
    */

    class KTSpectrogramCollector : public KTProcessor
    {
        public:
            struct CompareTypeInfo
            {
                bool operator() (const std::type_info* lhs, const std::type_info* rhs)
                {
                    return lhs->before(*rhs);
                }
            };
            struct SpectrogramCollector
            {
                KTDataPtr fData;
                KTSliceHeader& fSliceHeader;

                SpectrogramCollector() : fData(new KTData()), fSliceHeader(fData->Of<KTSliceHeader>())
                {
                }
                /*unsigned GetSliceNumber() const
                {
                    return fSliceHeader.GetSliceNumber();
                }
                void BumpSliceNumber()
                {
                    fSliceHeader.SetSliceNumber(fSliceHeader.GetSliceNumber() + 1);
                    return;
                }*/
            };

            typedef std::map< const std::type_info*, SpectrogramCollector > SpectrogramSetMap;
            typedef SpectrogramSetMap::iterator SpectrogramSetMapIt;

            struct SpectrogramCollectorTrackData
            {
                SpectrogramSetMap fDataMap;
                KTProcessedTrackData fTrackData;

                SpectrogramCollectorTrackData( SpectrogramSetMap dm, KTProcessedTrackData td ) : fDataMap( dm ), fTrackData( td )
                {
                }
            };
            struct KTTrackCompare
            {
                bool operator() (const SpectrogramCollectorTrackData lhs, const SpectrogramCollectorTrackData rhs) const
                {
                    return lhs.fTrackData.GetStartTimeInRunC() < rhs.fTrackData.GetStartTimeInRunC();
                }
            };
            
            std::vector< std::set< SpectrogramCollectorTrackData, KTTrackCompare > > fWaterfallSets;

            typedef std::set< SpectrogramCollectorTrackData, KTTrackCompare >::iterator SpectrogramCollectorTrackDataSetIt;

            struct SignalSet
            {
                    unsigned fSignalCount;
                    KTSignalData* fCollectingSignal;
                    KTSignalData* fFinishedSignal;
                    SignalSet(KTSignalData* accSig, KTSignalData* finishedSig) :
                        fSignalCount(0),
                        fCollectingSignal(accSig),
                        fFinishedSignal(finishedSig)
                    {}
            };
            typedef std::map< const std::type_info*, SignalSet > SignalMap;
            typedef SignalMap::iterator SignalMapIt;
            typedef SignalMap::value_type SignalMapValue;

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

            unsigned GetSignalInterval() const;
            void SetSignalInterval(unsigned interval);

        private:
            double fMinFrequency;
            double fMaxFrequency;
            unsigned fMinBin;
            unsigned fMaxBin;
            bool fCalculateMinBin;
            bool fCalculateMaxBin;
            double fLeadTime;
            double fTrailTime;
            double fSignalInterval;

        public:
            bool AddTrack(KTProcessedTrackData& trackData, unsigned component);
            bool ConsiderSpectrum(KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component);
            bool ReceiveTrack(KTProcessedTrackData& data);
            bool ReceiveSpectrum(KTPowerSpectrumData& data, KTSliceHeader& sliceData);
            bool ReceiveSpectrumCore(KTPowerSpectrumData& data, SpectrogramCollector& scDataStruct, KTPSCollectionData& psCollData, KTSliceHeader& sliceData, unsigned iComponent);
            void FinishSC( KTDataPtr data );

            const std::vector< std::set< SpectrogramCollectorTrackData, KTTrackCompare > > GetWaterfallSets() const;

            template< class XDataType >
            const SpectrogramCollector& GetSpectrogramSet( unsigned component, SpectrogramCollectorTrackData trackDataStruct ) const;

            template< class XDataType >
            SpectrogramCollector& GetOrCreateSpectrogramSet( unsigned component, SpectrogramCollectorTrackDataSetIt trackDataStruct );

            // The spectrograms are stored in a vector of sets of pairs of KTDataPtr and KTPSCollectionData. The levels to this hierarchy are:
            //      Vector - each element corresponds to a component
            //      Set    - each element corresponds to a track
            //      Pair   - the KTDataPtr which contains the spectrogram, and a pointer to the spectrogram
            // It is necessary to store the KTDataPtr because the signal must contain this object when it emits, and each spectrogram must have a
            // unique associated KTDataPtr
           
        private:
            mutable SpectrogramCollector* fLastSpectrogramCollectorPtr;

            //***************
            // Signals
            //***************

        private:
            KTSignalData fWaterfallSignal;

            KTSignalData fWaterfallFinishedSignal;

            SignalMap fSignalMap;

            //***************
            // Slots
            //***************

        private:
            //KTSlotDataOneType< KTProcessedTrackData > fTrackSlot;
            //KTSlotDataTwoTypes< KTPowerSpectrumData, KTSliceHeader > fPSSlot;
            template< class XDataType >
            void SlotFunction( KTDataPtr data );

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

    inline unsigned KTSpectrogramCollector::GetSignalInterval() const
    {
        return fSignalInterval;
    }

    inline void KTSpectrogramCollector::SetSignalInterval(unsigned interval)
    {
        fSignalInterval = interval;
        return;
    }

    inline const std::vector< std::set< KTSpectrogramCollector::SpectrogramCollectorTrackData, KTSpectrogramCollector::KTTrackCompare > > KTSpectrogramCollector::GetWaterfallSets() const
    {
        return fWaterfallSets;
    }

    template< class XDataType >
    const KTSpectrogramCollector::SpectrogramCollector& KTSpectrogramCollector::GetSpectrogramSet( unsigned component, SpectrogramCollectorTrackData trackDataStruct ) const
    {
        fLastSpectrogramCollectorPtr = const_cast< SpectrogramCollector* >(&fWaterfallSets[component].find( trackDataStruct )->fDataMap.at( &typeid( XDataType ) ));
        return *fLastSpectrogramCollectorPtr;
    }

    template< class XDataType >
    KTSpectrogramCollector::SpectrogramCollector& KTSpectrogramCollector::GetOrCreateSpectrogramSet( unsigned component, SpectrogramCollectorTrackDataSetIt trackDataStruct )
    {
        fLastSpectrogramCollectorPtr = trackDataStruct->fDataMap[&typeid( XDataType )];
        return *fLastSpectrogramCollectorPtr;
    }

    template< class XDataType >
    void KTSpectrogramCollector::SlotFunction(KTDataPtr data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< XDataType >())
        {
            KTERROR(avlog_hh, "Data not found with type <" << typeid(XDataType).name() << ">");
            return;
        }
        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(avlog_hh, "Data does not contain a slice header!");
        }

        // Call the function
        if (! ReceiveSpectrum(data->Of< XDataType >(), data->Of< KTSliceHeader >()))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing data with type <" << typeid(XDataType).name() << ">");
            return;
        }
        // If there's a signal pointer, emit the signal
        SignalMapIt sigIt = fSignalMap.find(&typeid(XDataType));
        if (sigIt != fSignalMap.end())
        {
            sigIt->second.fSignalCount++;
            unsigned sigCount = sigIt->second.fSignalCount;
            if (sigCount == fSignalInterval)
            {
                (*sigIt->second.fCollectingSignal)(fLastSpectrogramCollectorPtr->fData);
                sigIt->second.fSignalCount = 0;
            }
            if (data->GetLastData())
            {
                KTDEBUG(avlog_hh, "Emitting last-data signal");
                (*sigIt->second.fFinishedSignal)(fLastSpectrogramCollectorPtr->fData);
            }
        }
        return;
    }
}

#endif /* KTTRACKSPECTROGRAMCOLLECTOR_HH_ */
