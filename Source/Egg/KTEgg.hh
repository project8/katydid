/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include <string>

namespace Katydid
{
    class KTEggReader;
    class KTEggHeader;
    class KTEvent;

    class KTEgg
    {
        public:
            KTEgg();
            virtual ~KTEgg();

            bool BreakEgg(const std::string& filename);
            KTEvent* HatchNextEvent();
            bool CloseEgg();

            void SetReader(KTEggReader* reader);
            const KTEggReader* GetReader() const;

            void SetHeader(KTEggHeader* header);
            const KTEggHeader* GetHeader() const;

            void SetEventCounter(int count);
            int GetEventCounter() const;

        private:
            KTEggReader* fReader;
            KTEggHeader* fHeader;

            int fEventCounter;

    };

    inline const KTEggReader* KTEgg::GetReader() const
    {
        return fReader;
    }

    inline const KTEggHeader* KTEgg::GetHeader() const
    {
        return fHeader;
    }

    inline void KTEgg::SetEventCounter(int count)
    {
        fEventCounter = count;
        return;
    }

    inline int KTEgg::GetEventCounter() const
    {
        return fEventCounter;
    }

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
