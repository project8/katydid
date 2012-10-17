/**
 @file KTTimeSeriesDataReal.hh
 @brief Contains KTTimeSeriesDataReal
 @details Version of KTTimeSeriesData that contains a KTTimeSeriesReal.
 @author: N. S. Oblath
 @date: Oct 16, 2012
 */

#ifndef KTTIMESERIESDATAREAL_HH_
#define KTTIMESERIESDATAREAL_HH_

#include "KTTimeSeriesData.hh"


namespace Katydid
{
    class KTTimeSeriesDataReal : public KTTimeSeriesData
    {
        public:
            KTTimeSeriesDataReal(UInt_t nChannels=1);
            virtual ~KTTimeSeriesDataReal();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            void Accept(KTWriter* writer) const;

        private:
            static std::string fName;

    };

    inline const std::string& KTTimeSeriesDataReal::GetName() const
    {
        return fName;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESDATAREAL_HH_ */
