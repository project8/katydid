/*
 * KTBasicROOTFileWriter.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTFILEWRITER_HH_
#define KTBASICROOTFILEWRITER_HH_

#include "KTWriter.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTCorrelationData.hh"
#include "KTHoughData.hh"

#include "TFile.h"

namespace Katydid
{
    class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;

    class KTBasicROOTFileWriter : public KTWriter
    {
        public:
            KTBasicROOTFileWriter();
            virtual ~KTBasicROOTFileWriter();

            Bool_t Configure(const KTPStoreNode* node);

            //***************************
           // ROOT-file-specific members
            //***************************
        public:
            TFile* OpenFile(const std::string& filename, const std::string& flag);
            void CloseFile();

            const std::string& GetFilename() const;
            void SetFilename(const std::string& filename);

            const std::string& GetFileFlag() const;
            void SetFileFlag(const std::string& flag);

            TFile* GetFile();

        protected:
            Bool_t OpenAndVerifyFile();

        protected:
            std::string fFilename;
            std::string fFileFlag;

            TFile* fFile;

            //************************
            // Basic Publish and Write
            //************************
        public:

            void Publish(const KTWriteableData* data);

            void Write(const KTWriteableData* data);

            //************************
            // Time Series Data
            //************************
        public:
            void Write(const KTTimeSeriesData* data);

            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void WriteFrequencySpectrumData(const KTFrequencySpectrumData* data);
            void WriteFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data);
            void Write(const KTFrequencySpectrumData* data);
            void Write(const KTFrequencySpectrumDataFFTW* data);

            //************************
            // Correlation Data
            //************************
        public:
            void WriteCorrelationData(const KTCorrelationData* data);
            void Write(const KTCorrelationData* data);

            //************************
            // Sliding Window Data
            //************************
        public:
            void WriteSlidingWindowFSData(const KTSlidingWindowFSData* data);
            void WriteSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data);
            void Write(const KTSlidingWindowFSData* data);
            void Write(const KTSlidingWindowFSDataFFTW* data);

            //************************
            // Hough Transform Data
            //************************
        public:
            void Write(const KTHoughData* data);

    };

    inline TFile* KTBasicROOTFileWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTBasicROOTFileWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline const std::string& KTBasicROOTFileWriter::GetFilename() const
    {
        return fFilename;
    }
    inline void KTBasicROOTFileWriter::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTBasicROOTFileWriter::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTBasicROOTFileWriter::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }

    inline TFile* KTBasicROOTFileWriter::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTBASICROOTFILEWRITER_HH_ */
