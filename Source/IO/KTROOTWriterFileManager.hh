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

    /*!
       @class KTROOTWriterFileManager
       @author N. S. Oblath

       @brief Handles management of ROOT files for ROOT-based Writers

       @details
       This class handles the opening, closing, and merging of ROOT files via three interface functions: OpenFile(), FinishFile(), and DiscardFile().

       A parent writer calls OpenFile() to receive a TFile pointer to which it can write all its data.  Once writing is complete, FinishFile() should be called.  DiscardFile() is used to remove a file without writing it to disk.

       The TFile object returned by OpenFile() is a temporary file for reasons described in "Behind the scenes . . ." below.
       When file writing is complete from all parents of a given filename, the final file has the given filename and the temporary file(s) are removed.

       Multiple writers can write to the same file by providing the same filename, but a unique "parent" pointer.  Typically the writer itself is the parent, and it provides its "this" pointer for that argument.

       File writing and the interface functions of this class are thread-safe.  Multiple parents can write to the same filename from different threads.

       Behind the scenes, any every parent is writing to a temporary file.  If only a single parent is writing to a given filename, then the temporary file is copied over to the final filename.
       If multiple parents are writing to the same filename, then all of the temporary files are merged into one having the final filename.
       The motivation for using the temporary-file scheme is thread safety.  TFile is not (as of this writing) inherently thread safe, in that multiple files cannot concurrently write to the same TFile.

       The pointer to the parent is _never_ dereferenced; it's only used to distinguish who is writing to which file.
       Therefore a user can supply any pointer value, even if it's not to a valid KTWriter.  This is useful when the parent is not a Writer (see, for example, TestROOTWriterFileManager.cc.

       The acceptable Option parameters are the typical ROOT file options for writable files:
         - CREATE or NEW -- creates a new file; file opening will fail if the file already exists
         - RECREATE -- overwrites a file if it already exists; creates a new file if the file doesn't already exist
         - UPDATE -- adds to an existing file if it already exists
       Option arguments are not case-sensitive.
      */
    class KTROOTWriterFileManager : public scarab::singleton< KTROOTWriterFileManager >
    {
        private:
            KTROOTWriterFileManager();
            virtual ~KTROOTWriterFileManager();

        public:
            TFile* OpenFile(const Nymph::KTWriter* aParent, const std::string& aFilename, const std::string& aOption = "CREATE", bool aInMemory = false);

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
