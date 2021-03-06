/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details

This allows KTEggProcessor to read Matlab files generated by the Tektronix RSA.
It parses the header and produces slices, and then stores the header and data 
into Egg classes as if it was an Egg file, allowing for seamless processing of
the data.  The user can select this reader by setting "egg-reader" to "rsamat"
in the JSON configuration file.

It can read the MAT files produced by the RSA directly, or the Concatenated MAT
files generated by ConcatRSAMAT.m.  The concatenated MAT files are identified by
the presence of a structure fileinfo inside, with information about the
original MAT files.

To use the "rsamat" egg-reader, the user must have installed the Matlab 
Compiler Runtime (MCR) of version 2014a, and configured Katydid to use 
Matlab at compilation.  Earlier versions might work but have not been
tested.  R2013a is known to not work for Katydid on Macs, and R2012b
to not work with Katydid on Linux systems.

 @author: L de Viveiros
 @date: May 19, 2014
 */

#ifndef KTRSAMATREADER_HH_
#define KTRSAMATREADER_HH_

#include "matio.h"

#include "KTEggReader.hh"

#include <inttypes.h>
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
    
    class KTEggHeader;

    class KTRSAMatReader : public KTEggReader
    {
        public:
            KTRSAMatReader();
            virtual ~KTRSAMatReader();

        protected:
            unsigned fSliceSize;
            unsigned fStride;

        private:
            Nymph::KTDataPtr fHeaderPtr;
            KTEggHeader& fHeader;
            static const unsigned fMaxChannels = 2;
            double fSampleRateUnitsInHz;
            unsigned fRecordSize;
            double fBinWidth;
            uint64_t fSliceNumber;
            unsigned fRecordsRead;
            unsigned fSamplesRead;
            unsigned fSamplesPerFile;
            unsigned fRecordsPerFile;
            std::vector< double > fRecordsTimeStampSeconds;
            matvar_t *fTSArrayMat;
            mat_t *fMatFilePtr;
            unsigned fDataPrecision;


        public:
            bool Configure(const KTEggProcessor& eggProc);

            /// Opens the matlab file and returns a new copy of the header information.
            Nymph::KTDataPtr BreakEgg(const path_vec& filenames);
            /// Returns the next slice's time series data.
            Nymph::KTDataPtr HatchNextSlice();
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
            /// Returns the time within the current acquisition
            double GetTimeInAcq() const;

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
        // Time to the acquisition, relative to the start of the run
        // = fRecordsTimeStampSeconds[fRecordsRead]
        // Number of Samples within the record
        // = fSamplesRead - fRecordsRead*fRecordSize
        return GetTimeInAcq() + fRecordsTimeStampSeconds[fRecordsRead];
    }
    inline double KTRSAMatReader::GetTimeInAcq() const
    {
        // Number of Samples within the record
        // = fSamplesRead - fRecordsRead*fRecordSize
        return double(fSamplesRead - fRecordsRead*fRecordSize) * fBinWidth;
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
