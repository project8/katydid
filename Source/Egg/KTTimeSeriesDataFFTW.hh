/**
 @file KTTimeSeriesDataFFTW.hh
 @brief Contains KTTimeSeriesDataFFTW
 @details Version of KTTimeSeriesData containing a KTTimeSeriesFFTW.
 @author: N. S. Oblath
 @date: Oct 16, 2012
 */

#ifndef KTTIMESERIESDATAFFTW_HH_
#define KTTIMESERIESDATAFFTW_HH_

#include "KTTimeSeriesData.hh"


namespace Katydid
{
    class KTTimeSeriesDataFFTW : public KTTimeSeriesData
    {
        public:
            KTTimeSeriesDataFFTW(UInt_t nChannels=1);
            virtual ~KTTimeSeriesDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();


        private:
            static std::string fName;

    };

    inline const std::string& KTTimeSeriesDataFFTW::GetName() const
    {
        return fName;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESDATAFFTW_HH_ */
