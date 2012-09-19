/**
 @file KTAdvancedEggProcessor.hh
 @brief Contains KTAdvancedEggProcessor
 @details Iterates over events in an Egg file.
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTADVANCEDEGGPROCESSOR_HH_
#define KTADVANCEDEGGPROCESSOR_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

#include "KTEgg.hh"

namespace Katydid
{
    class KTTimeSeriesData;
    class KTPStoreNode;

    /*!
     @class KTAdvancedEggProcessor
     @author N. S. Oblath

     @brief Iterates over the events in an Egg file.

     @details
     Iterates over events in an egg file; events are extracted until fNEvents is reached.

     Available configuration options:
     \li \c "number-of-events": UInt_t -- Number of events to process
     \li \c "filename": string -- Egg filename to use
     \li \c "egg-reader": string -- Egg reader to use (options: monarch [default], 2011)

     Command-line options defined
     \li \c -n (n-events): Number of events to process
     \li \c -e (egg-file): Egg filename to use
     \li \c -z (--use-2011-egg-reader): Use the 2011 egg reader

     Signals:
     \li \c void (KTEgg::HeaderInfo info) emitted when the file header is parsed.
     \li \c void (UInt_t iEvent, const KTEvent* eventPtr) emitted when an event is read from the file.
     \li \c void () emitted when a file is finished.
    */
    class KTAdvancedEggProcessor : public KTProcessor, public KTConfigurable
    {
        public:
            typedef KTSignal< void (const KTEggHeader*) >::signal HeaderSignal;
            typedef KTSignal< void (KTEvent*) >::signal EventSignal;
            typedef KTSignal< void () >::signal EggDoneSignal;

        public:
            enum EggReaderType
            {
                k2011EggReader,
                kMonarchEggReader
            };

        public:
            KTAdvancedEggProcessor();
            virtual ~KTAdvancedEggProcessor();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t ProcessEgg();

            UInt_t GetNEvents() const;
            const std::string& GetFilename() const;
            EggReaderType GetEggReaderType() const;

            void SetNEvents(UInt_t nEvents);
            void SetFilename(const std::string& filename);
            void SetEggReaderType(EggReaderType type);

        private:
            UInt_t fNEvents;

            std::string fFilename;

            EggReaderType fEggReaderType;

            //***************
            // Signals
            //***************

        private:
            HeaderSignal fHeaderSignal;
            EventSignal fEventSignal;
            EggDoneSignal fEggDoneSignal;

    };

    inline UInt_t KTAdvancedEggProcessor::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTAdvancedEggProcessor::SetNEvents(UInt_t nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline const std::string& KTAdvancedEggProcessor::GetFilename() const
    {
        return fFilename;
    }

    inline void KTAdvancedEggProcessor::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline KTAdvancedEggProcessor::EggReaderType KTAdvancedEggProcessor::GetEggReaderType() const
    {
        return fEggReaderType;
    }

    inline void KTAdvancedEggProcessor::SetEggReaderType(EggReaderType type)
    {
        fEggReaderType = type;
        return;
    }

} /* namespace Katydid */

#endif /* KTADVANCEDEGGPROCESSOR_HH_ */
