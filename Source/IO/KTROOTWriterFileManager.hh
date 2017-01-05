/*
 * KTROOTWriterFileManager.hh
 *
 *  Created on: Jan 4, 2017
 *      Author: obla999
 */

#ifndef KATYDID_KTROOTWRITERFILEMANAGER_HH_
#define KATYDID_KTROOTWRITERFILEMANAGER_HH_

#include "singleton.hh"

#include "path.hh"

#include <map>
#include <mutex>
#include <string>

namespace Nymph
{
    class KTWriter;
}

class TFile;

namespace Katydid
{

    class KTROOTWriterFileManager : public scarab::singleton< KTROOTWriterFileManager >
    {
        private:
            KTROOTWriterFileManager();
            virtual ~KTROOTWriterFileManager();

        public:
            TFile* OpenFile(const Nymph::KTWriter* aParent, const std::string& aFilename, const std::string& aOption = "", bool aInMemory = false);

            bool FinishFile(const Nymph::KTWriter* aParent, const std::string& aFilename);

            bool DiscardFile(const Nymph::KTWriter* aParent, const std::string& aFilename);

        private:
            friend class scarab::singleton< KTROOTWriterFileManager >;
            friend class scarab::destroyer< KTROOTWriterFileManager >;

            typedef std::map< const Nymph::KTWriter*, TFile* > FileMap;
            typedef FileMap::iterator FileMapIt;
            typedef FileMap::const_iterator FileMapCIt;

            struct FileConstruct
            {
                FileMap fOpenFiles;
                FileMap fFinishedFiles;
                std::string fOption;
                scarab::path fDir;
            };

            typedef std::map< std::string, FileConstruct > FileConstMap;
            typedef FileConstMap::iterator FileConstMapIt;
            typedef FileConstMap::const_iterator FileConstMapCIt;

            bool MoveToFinished(const FileConstMapIt& aFileConst, const Nymph::KTWriter* aParent);   // not thread-safe
            bool MoveToFinished(const FileConstMapIt& aFileConst, const FileMapIt& aFile);   // not thread-safe
            bool CloseFile(FileConstMapIt& aFileConst);   // not thread-safe

            Nymph::KTWriter* FindTempPointer(const FileMap& aMap) const;

            FileConstMap fFiles;

            std::mutex fMutex;
    };

} /* namespace Katydid */

#endif /* KATYDID_KTROOTWRITERFILEMANAGER_HH_ */
