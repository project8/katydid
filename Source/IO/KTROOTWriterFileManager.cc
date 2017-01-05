/*
 * KTROOTWriterFileManager.cc
 *
 *  Created on: Jan 4, 2017
 *      Author: obla999
 */

#include "KTROOTWriterFileManager.hh"

#include "KTLogger.hh"
#include "KTWriter.hh"

#include "TMemFile.h"
#include "TFileMerger.h"

namespace Katydid
{
    KTLOGGER(publog, "KTROOTWriterFileManager");

    KTROOTWriterFileManager::KTROOTWriterFileManager() :
            fFiles(),
            fMutex()
    {
    }

    KTROOTWriterFileManager::~KTROOTWriterFileManager()
    {
        std::unique_lock< std::mutex > lock(fMutex);

        for (FileConstMapIt tIt = fFiles.begin(); tIt != fFiles.end(); ++tIt)
        {
            CloseFile(tIt);
        }

        lock.unlock();
    }

    TFile* KTROOTWriterFileManager::OpenFile(const Nymph::KTWriter* aParent, const std::string& aFilename, const std::string& aOption, bool aInMemory)
    {
        std::unique_lock< std::mutex > lock(fMutex);

        try
        {
            std::string option = aOption;
            std::transform(option.begin(), option.end(), option.begin(), ::toupper);

            // deal with / verify the file option
            if (option == "NEW" || option == "CREATE")
            {
                option = "CREATE";
                if (scarab::fs::exists(aFilename))
                {
                    KTERROR(publog, "File already exists and the option specified prevents overwriting: <" << aFilename << ">");
                    return nullptr;
                }
            }
            else if (option != "RECREATE" && option != "UPDATE")
            {
                KTERROR(publog, "Invalid file option: " << option);
                return nullptr;
            }

            // determine the absolute path and parent directory
            scarab::path absFilepath = scarab::fs::absolute(aFilename);
            scarab::path fileDir = absFilepath.parent_path();

            if (! scarab::fs::is_directory(fileDir))
            {
                KTERROR(publog, "Parent directory of output file <" << fileDir << "> does not exist or is not a directory");
                return nullptr;
            }

            KTDEBUG(publog, "Opening file <" << absFilepath << "> for parent <" << aParent << ">");

            // find/create the file construct
            FileConstMapIt tThisFileConst = fFiles.find(absFilepath.native());
            if (tThisFileConst == fFiles.end())
            {
                auto insertion = fFiles.insert(FileConstMap::value_type(absFilepath.native(), FileConstruct()));
                if (! insertion.second)
                {
                    KTERROR(publog, "Unable to add file construct for <" << absFilepath << ">");
                    return nullptr;
                }
                tThisFileConst = insertion.first;
                tThisFileConst->second.fOption = option;
                tThisFileConst->second.fDir = fileDir;
            }

            // construct the unique filename that will be used for this particular contribution to the eventual file
            scarab::path filePath = fileDir / scarab::fs::unique_path();
            filePath += ".root";
            KTDEBUG(publog, "Temporary file path for file <" << absFilepath << "> with parent <" << aParent << "> is <" << filePath << ">");

            // create the file object that will be written to
            TFile* newFile = nullptr;
            if (aInMemory)
            {
                newFile = new TMemFile(filePath.native().c_str(), "RECREATE");
            }
            else
            {
                newFile = new TFile(filePath.native().c_str(), "RECREATE");
            }

            if (newFile == nullptr)
            {
                KTERROR(publog, "File <" << absFilepath << "> was not created for parent <" << aParent << ">!");
                return nullptr;
            }

            // insert file object into the file construct
            tThisFileConst->second.fOpenFiles.insert(FileMap::value_type(aParent, newFile));

            KTINFO(publog, "Created file <" << absFilepath << "> for parent <" << aParent << ">");

            return newFile;
        }
        catch (std::exception& e)
        {
            KTERROR(publog, "Exception caught while trying to open file <" << aFilename << "> for parent <" << aParent << ">: " << e.what());
            return nullptr;
        }
    }

    bool KTROOTWriterFileManager::FinishFile(const Nymph::KTWriter* aParent, const std::string& aFilename)
    {
        std::unique_lock< std::mutex > lock(fMutex);

        try
        {
            // determine the absolute path and parent directory
            scarab::path absFilepath = scarab::fs::absolute(aFilename);
            KTDEBUG(publog, "Finishing file <" << absFilepath << "> for parent <" << aParent << ">");

            FileConstMapIt tThisFileConst = fFiles.find(absFilepath.native());
            if (tThisFileConst == fFiles.end())
            {
                KTWARN(publog, "Did not find file construct for <" << absFilepath << ">; no action taken");
                return true;
            }

            if (! MoveToFinished(tThisFileConst, aParent))
            {
                KTERROR(publog, "Unable to finish file <" << absFilepath << "> for parent <" << aParent << ">");
                return false;
            }

            if (tThisFileConst->second.fOpenFiles.size() == 0)
            {
                return CloseFile(tThisFileConst);
            }

            return true;
        }
        catch (std::exception& e)
        {
            KTERROR(publog, "Exception caught while trying to finish file <" << aFilename << "> for parent <" << aParent << ">: " << e.what());
            return false;
        }
    }

    bool KTROOTWriterFileManager::DiscardFile(const Nymph::KTWriter* aParent, const std::string& aFilename)
    {
        std::unique_lock< std::mutex > lock(fMutex);

        try
        {
            // determine the absolute path and parent directory
            scarab::path absFilepath = scarab::fs::absolute(aFilename);
            KTDEBUG(publog, "Discarding file <" << absFilepath << "> for parent <" << aParent << ">");

            FileConstMapIt tThisFileConst = fFiles.find(absFilepath.native());
            if (tThisFileConst == fFiles.end())
            {
                KTWARN(publog, "Did not find file construct for <" << absFilepath << ">; no action taken");
                return true;
            }

            FileMapIt tThisFileIt = tThisFileConst->second.fOpenFiles.find(aParent);
            if (tThisFileIt == tThisFileConst->second.fOpenFiles.end())
            {
                KTWARN(publog, "File <" << tThisFileConst->first << "> was not open for parent <" << aParent << ">; no action taken");
                return true;
            }

            delete tThisFileIt->second;
            tThisFileConst->second.fOpenFiles.erase(tThisFileIt);

            if (tThisFileConst->second.fOpenFiles.size() == 0)
            {
                return CloseFile(tThisFileConst);
            }

            return true;
        }
        catch (std::exception& e)
        {
            KTERROR(publog, "Exception caught while trying to discard file <" << aFilename << "> for parent <" << aParent << ">: " << e.what());
            return false;
        }
    }

    bool KTROOTWriterFileManager::MoveToFinished(const FileConstMapIt& aFileConst, const Nymph::KTWriter* aParent)
    {
        FileMapIt tThisFileIt = aFileConst->second.fOpenFiles.find(aParent);
        if (tThisFileIt == aFileConst->second.fOpenFiles.end())
        {
            KTWARN(publog, "File <" << aFileConst->first << "> was not open for parent <" << aParent << ">; no action taken");
            return true;
        }

        return MoveToFinished(aFileConst, tThisFileIt);
    }

    bool KTROOTWriterFileManager::MoveToFinished(const FileConstMapIt& aFileConst, const FileMapIt& aFileIt)
    {
        auto insertion = aFileConst->second.fFinishedFiles.insert(FileMap::value_type(aFileIt->first, aFileIt->second));
        if (! insertion.second)
        {
            KTERROR(publog, "Unable to move file <" << aFileConst->first << "> for parent <" << aFileIt->first << "> from \"open\" to \"finished\"");
            return false;
        }

        aFileConst->second.fOpenFiles.erase(aFileIt);

        KTDEBUG(publog, "File <" << aFileConst->first << "> for parent <" << insertion.first->first << "> transferred from \"open\" to \"finished\"");

        KTDEBUG(publog, "Writing contents of the ROOT file");
        insertion.first->second->Write();

        return true;

    }

    bool KTROOTWriterFileManager::CloseFile(FileConstMapIt& aFileConst)
    {
        KTINFO(publog, "Closing file <" << aFileConst->first << ">");

        if (! aFileConst->second.fOpenFiles.empty())
        {
            for (FileMapIt tFileIt = aFileConst->second.fOpenFiles.begin(); tFileIt != aFileConst->second.fOpenFiles.end(); ++tFileIt)
            {
                if (! MoveToFinished(aFileConst, tFileIt))
                {
                    // if the move failed, then tFileIt should still be valid
                    KTERROR(publog, "Due to an error moving a file to finished, file <" << aFileConst->first << "> will be incomplete!");
                    KTWARN(publog, "Writing partial file at <" << tFileIt->second->GetName() << ">");
                    tFileIt->second->Write();
                    delete tFileIt->second;
                    tFileIt = (aFileConst->second.fOpenFiles.erase(tFileIt))--;
                }
            }
        }

        // if we'll be adding to an existing file, add it to the finished-files map
        if (aFileConst->second.fOption == "UPDATE" && scarab::fs::exists(aFileConst->first))
        {
            scarab::path newPath = aFileConst->second.fDir / scarab::fs::unique_path();
            newPath += ".root";
            scarab::fs::rename(aFileConst->first, newPath);
            TFile* existingFile = new TFile(newPath.native().c_str(), "READ");
            Nymph::KTWriter* tempWriter = FindTempPointer(aFileConst->second.fFinishedFiles);
            aFileConst->second.fFinishedFiles.insert(FileMap::value_type(tempWriter, existingFile));
            aFileConst->second.fOption = "CREATE";
            KTDEBUG(publog, "Writing file in \"update\" mode, and file already exists; moving existing file to temporary location <" << newPath << "> and identifying it with temp writer pointer <" << tempWriter << ">");
        }

        // move/merge data from temporary file(s) to final destination
        if (aFileConst->second.fFinishedFiles.size() > 1)
        {
            KTDEBUG(publog, "Merging files into <" << aFileConst->first << ">");

            TFileMerger merger;
            if (! merger.OutputFile(aFileConst->first.c_str(), aFileConst->second.fOption.c_str()))
            {
                KTERROR(publog, "Output file did not open for <" << aFileConst->first << ">; unmerged files have not been affected");
                return false;
            }

            for (FileMapIt tFileIt = aFileConst->second.fFinishedFiles.begin(); tFileIt != aFileConst->second.fFinishedFiles.end(); ++tFileIt)
            {
                KTDEBUG(publog, "Adding file <" << tFileIt->second->GetName() << "> to the merge");
                if (! merger.AddFile(tFileIt->second))
                {
                    KTERROR(publog, "Unable to add file from parent <" << tFileIt->first << "> for <" << aFileConst->first << ">; output will be incomplete!");
                    KTWARN(publog, "Writing partial file at <" << tFileIt->second->GetName() << ">");
                    tFileIt->second->Write();
                    tFileIt->second->Close();
                    delete tFileIt->second;
                    tFileIt = (aFileConst->second.fOpenFiles.erase(tFileIt))--;
                }
            }

            KTDEBUG(publog, "Merging files now");
            if (! merger.Merge())
            {
                KTERROR(publog, "File merge failed for <" << aFileConst->first << ">. Output data was lost!");
                fFiles.erase(aFileConst);
                return false;
            }

            KTDEBUG(publog, "Removing temporary files");
            for (FileMapIt tFileIt = aFileConst->second.fFinishedFiles.begin(); tFileIt != aFileConst->second.fFinishedFiles.end(); ++tFileIt)
            {
                scarab::path tempFilePath = tFileIt->second->GetName();
                KTDEBUG(publog, "Removing file <" << tempFilePath << ">");
                tFileIt->second->Close();
                delete tFileIt->second;
                tFileIt->second = nullptr;
                if (! scarab::fs::remove(tempFilePath))
                {
                    KTWARN(publog, "Unable to remove temporary file <" << tempFilePath << ">");
                }
            }
            aFileConst->second.fFinishedFiles.clear();

        }
        else
        {
            KTDEBUG(publog, "Closing single file <" << aFileConst->first << ">");
            TFile* tFile = aFileConst->second.fFinishedFiles.begin()->second;
            scarab::path tempFilePath = tFile->GetName();
            tFile->Close();
            delete tFile;

            KTDEBUG(publog, "Moving file from <" << tempFilePath << "> to <" << aFileConst->first << ">");
            scarab::fs::rename(tempFilePath, aFileConst->first);
            aFileConst->second.fFinishedFiles.clear();
        }

        KTINFO(publog, "File <" << aFileConst->first << "> has been written and closed");

        fFiles.erase(aFileConst);

        return true;
    }

    Nymph::KTWriter* KTROOTWriterFileManager::FindTempPointer(const FileMap& aMap) const
    {
        Nymph::KTWriter* tempWriter = 0x0;
        while (aMap.find(tempWriter) != aMap.end())
        {
            tempWriter += 0x1;
        }
        KTDEBUG(publog, "Returning temp pointer <" << tempWriter << ">");
        return tempWriter;
    }

} /* namespace Katydid */
