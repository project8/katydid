/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces slices.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTRSAMATREADER_HH_
#define KTRSAMATREADER_HH_

#include "mat.h" 

#include "KTEggReader.hh"
#include "KTEggHeader.hh"

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif


namespace Katydid
{

    class KTRSAMatReader : public KTEggReader
    {
        public:
            KTRSAMatReader();
            virtual ~KTRSAMatReader();

        protected:
            unsigned fSliceSize;
            unsigned fStride;

        private:
            KTEggHeader fHeader;
            static const unsigned fMaxChannels = 2;
            double fSampleRateUnitsInHz;
            unsigned fRecordSize;
            double fBinWidth;
            uint64_t fSliceNumber;
            unsigned fRecordsRead;
            unsigned fSamplesRead;
            mxArray *ts_array_mat;
            MATFile *matfilep;


        public:
            /// Opens the egg file and returns a new copy of the header information.
            KTEggHeader* BreakEgg(const std::string& filename);
            /// Returns the next slice's time series data.
            KTDataPtr HatchNextSlice();
            /// Closes the file.
            bool CloseEgg();

            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);
            unsigned GetStride() const;
            void SetStride(unsigned stride);
            static unsigned GetMaxChannels();
            double GetSampleRateUnitsInHz() const;
            double GetFullVoltageScale() const;
            unsigned GetNADCLevels() const;
            unsigned GetRecordSize() const;
            double GetBinWidth() const;

            /// Returns the time since the run started in seconds
            double GetTimeInRun() const;
            /// Same as GetTimeInRun
            virtual double GetIntegratedTime() const;

            /// Returns the number of slices processed
            virtual unsigned GetNSlicesProcessed() const;

            /// Returns the number of records processed (including partial usage)
            virtual unsigned GetNRecordsProcessed() const;


    };

    inline unsigned KTRSAMatReader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTRSAMatReader::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline unsigned KTRSAMatReader::GetStride() const
    {
        return fStride;
    }

    inline void KTRSAMatReader::SetStride(unsigned stride)
    {
        fStride = stride;
        return;
    }

    inline double KTRSAMatReader::GetSampleRateUnitsInHz() const
    {
        return fSampleRateUnitsInHz;
    }

    inline unsigned KTRSAMatReader::GetRecordSize() const
    {
        return fRecordSize;
    }
    inline double KTRSAMatReader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline double KTRSAMatReader::GetIntegratedTime() const
    {
        return GetTimeInRun();
    }
    inline double KTRSAMatReader::GetTimeInRun() const
    {
        return double(fSamplesRead) * fBinWidth;
    }
    inline unsigned KTRSAMatReader::GetNSlicesProcessed() const
    {
        return (unsigned)fSliceNumber;
    }
    inline unsigned KTRSAMatReader::GetNRecordsProcessed() const
    {
        return fRecordsRead;
    }




} /* namespace Katydid */

#endif /* KTRSAMATREADER_HH_ */
