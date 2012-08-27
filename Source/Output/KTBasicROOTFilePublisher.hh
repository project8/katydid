/*
 * KTBasicROOTFilePublisher.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTFILEPUBLISHER_HH_
#define KTBASICROOTFILEPUBLISHER_HH_

#include "KTPublisher.hh"
#include "KTFrequencySpectrumData.hh"

#include "TFile.h"

#include <iostream>

namespace Katydid
{

    class KTBasicROOTFilePublisher : public KTPublisher
    {
        public:
            KTBasicROOTFilePublisher();
            virtual ~KTBasicROOTFilePublisher();

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

            void Publish(KTWriteableData* data);

            void Write(KTWriteableData* data);

            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void Write(KTFrequencySpectrumData* data);

    };

    inline TFile* KTBasicROOTFilePublisher::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }
    inline void KTBasicROOTFilePublisher::CloseFile()
    {
        if (fFile != NULL)
        {
            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    inline const std::string& KTBasicROOTFilePublisher::GetFilename() const
    {
        return fFilename;
    }
    inline void KTBasicROOTFilePublisher::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline const std::string& KTBasicROOTFilePublisher::GetFileFlag() const
    {
        return fFileFlag;
    }
    inline void KTBasicROOTFilePublisher::SetFileFlag(const std::string& flag)
    {
        fFileFlag = flag;
        return;
    }

    inline TFile* KTBasicROOTFilePublisher::GetFile()
    {
        return fFile;
    }


} /* namespace Katydid */
#endif /* KTBASICROOTFILEPUBLISHER_HH_ */
