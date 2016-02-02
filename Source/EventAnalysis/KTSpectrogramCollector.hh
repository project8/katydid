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



namespace Nymph
{
    using namespace Nymph;
    class KTParamNode;
    class KTPowerSpectrumData;
    class KTProcessedTrackData;

    /*!
     @class KTSpectrogramCollector
     @author N.S. Oblath

     @brief Collects the spectra that pertain to a particular track or event

     @details
     [detailed class description]

     Configuration name: "spectrogram-collector"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "[slot-name]": void (KTDataPtr) -- [what it does]; Requires [input data type]; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "[signal-name]": void (KTDataPtr) -- Emitted upon [whatever was done]; Guarantees [output data type].
    */

    class KTSpectrogramCollector : public KTProcessor
    {
        public:
            KTSpectrogramCollector(const std::string& name = "[config-name]");
            virtual ~KTSpectrogramCollector();

            bool Configure(const KTParamNode* node);

            // Getters and setters for configurable parameters go here
        private:
            // configurable member parameters go here

        public:
            bool AddTrack(KTProcessedTrackData& trackData);

            bool ConsiderSpectrum(KTPowerSpectrumData& psData);

        private:
            // Perhaps there are some non-public helper functions?

            // And then any non-configurable member variables
            std::vector<std::set<KTPSCollectionData>> waterfallSets;
            struct TrackTimeInfo
            {
                double fStartTime;
                double fEndTime;
                TrackTimeInfo( double startTime, double endTime ) : fStartTime( startTime ), fEndTime( endTime ) {}
            };
            struct KTTrackCompare
            {
                bool operator() (const KTPSCollectionData lhs, const KTPSCollectionData rhs) const
                {
                    return lhs.fStartTime < rhs.fStartTime;
                }
            };

            //***************
            // Signals
            //***************

        private:
            KTSignalData f[SomeName]Signal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataOneType< [Input Data Type] > f[SomeName]Slot;

    };
}

#endif /* KTTRACKSPECTROGRAMCOLLECTOR_HH_ */
