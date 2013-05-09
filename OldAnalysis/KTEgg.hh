/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces slices.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include <boost/shared_ptr.hpp>

#include <string>

namespace Katydid
{
    class KTEggReader;
    class KTEggHeader;
    class KTData;

    class KTEgg
    {
        public:
            KTEgg();
            virtual ~KTEgg();

            bool BreakEgg(const std::string& filename);
            boost::shared_ptr<KTData> HatchNextSlice();
            bool CloseEgg();

            /// Assumes ownership of the egg reader
            void SetReader(KTEggReader* reader);
            const KTEggReader* GetReader() const;

            void SetHeader(KTEggHeader* header);
            const KTEggHeader* GetHeader() const;

            void SetSliceCounter(int count);
            int GetSliceCounter() const;

        private:
            KTEggReader* fReader;
            KTEggHeader* fHeader;

            int fSliceCounter;

    };

    inline const KTEggReader* KTEgg::GetReader() const
    {
        return fReader;
    }

    inline const KTEggHeader* KTEgg::GetHeader() const
    {
        return fHeader;
    }

    inline void KTEgg::SetSliceCounter(int count)
    {
        fSliceCounter = count;
        return;
    }

    inline int KTEgg::GetSliceCounter() const
    {
        return fSliceCounter;
    }

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
