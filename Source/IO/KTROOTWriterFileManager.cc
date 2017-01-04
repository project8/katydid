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

    bool KTROOTWriterFileManager::MoveToFinished(const FileConstMapIt& aFileConst, const Nymph::KTWriter* aParent)
    {
        FileMapIt tThisFile = aFileConst->second.fOpenFiles.find(aParent);
        if (tThisFile == aFileConst->second.fOpenFiles.end())
        {
            KTWARN(publog, "File <" << aFileConst->first << "> was not open for parent <" << aParent << ">; no action taken");
            return true;
        }

        return MoveToFinished(aFileConst, tThisFile);
    }

    bool KTROOTWriterFileManager::MoveToFinished(const FileConstMapIt& aFileConst, const FileMapIt& aFile)
    {
        auto insertion = aFileConst->second.fFinishedFiles.insert(FileMap::value_type(aFile->first, aFile->second));
        if (! insertion.second)
        {
            KTERROR(publog, "Unable to move file <" << aFileConst->first << "> for parent <" << aFile->first << "> from \"open\" to \"finished\"");
            return false;
        }

        aFileConst->second.fOpenFiles.erase(aFile);

        KTDEBUG(publog, "File <" << aFileConst->first << "> for parent <" << aFile->first << "> transferred from \"open\" to \"finished\"");

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
                    KTERROR(publog, "Due to an error moving a file to finished, file <" << aFileConst->first << "> will be incomplete!");
                    KTWARN(publog, "Writing partial file at <" << tFileIt->second->GetName() << ">");
                    tFileIt->second->Write();
                    delete tFileIt->second;
                    tFileIt = (aFileConst->second.fOpenFiles.erase(tFileIt))--;
                }
            }
        }

        // if we'll be adding to an existing file, add it to the finished-files map
        if (aFileConst->second.fOption == "UPDATE" && boost::filesystem::exists(aFileConst->first))
        {
            boost::filesystem::path newPath = aFileConst->second.fDir / boost::filesystem::unique_path();
            boost::filesystem::rename(aFileConst->first, newPath);
            TFile* existingFile = new TFile(newPath.native().c_str(), "UPDATE");
            aFileConst->second.fFinishedFiles.insert(FileMap::value_type(FindTempPointer(aFileConst->second.fFinishedFiles), existingFile));
            aFileConst->second.fOption = "CREATE";
        }

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
                if (! merger.AddAdoptFile(tFileIt->second))
                {
                    KTERROR(publog, "Unable to add file from parent <" << tFileIt->first << "> for <" << aFileConst->first << ">; output will be incomplete!");
                    KTWARN(publog, "Writing partial file at <" << tFileIt->second->GetName() << ">");
                    tFileIt->second->Write();
                    delete tFileIt->second;
                    tFileIt = (aFileConst->second.fOpenFiles.erase(tFileIt))--;
                }
            }
            aFileConst->second.fFinishedFiles.clear();

            if (! merger.Merge())
            {
                KTERROR(publog, "File merge failed for <" << aFileConst->first << ">. Output data was lost!");
                fFiles.erase(aFileConst);
                return false;
            }
        }
        else
        {
            KTDEBUG(publog, "Closing single file <" << aFileConst->first << ">");
            TFile* tFile = aFileConst->second.fFinishedFiles.begin()->second;
            tFile->Write();

            KTDEBUG(publog, "Moving file from <" << tFile->GetName() << "> to <" << aFileConst->first << ">");
            boost::filesystem::rename(tFile->GetName(), aFileConst->first);
            delete tFile;
            aFileConst->second.fFinishedFiles.clear();
        }

        fFiles.erase(aFileConst);

        KTINFO(publog, "File <" << aFileConst->first << "> has been written and closed");

        return true;
    }

    bool KTROOTWriterFileManager::FinishFile(const Nymph::KTWriter* aParent, const std::string& aFilename)
    {
        std::unique_lock< std::mutex > lock(fMutex);

        FileConstMapIt tThisFileConst = fFiles.find(aFilename);
        if (tThisFileConst == fFiles.end())
        {
            KTWARN(publog, "Did not find file construct for <" << aFilename << ">; no action taken");
            return true;
        }

        if (! MoveToFinished(tThisFileConst, aParent))
        {
            KTERROR(publog, "Unable to finish file <" << aFilename << "> for parent <" << aParent << ">");
            return false;
        }

        if (tThisFileConst->second.fOpenFiles.size() == 0)
        {
            return CloseFile(tThisFileConst);
        }

        return true;
    }


    TFile* KTROOTWriterFileManager::OpenFile(const Nymph::KTWriter* aParent, const std::string& aFilename, const std::string& aOption, bool aInMemory)
    {
        std::unique_lock< std::mutex > lock(fMutex);

        std::string option = aOption;

        // deal with / verify the file option
        if (option.empty() || option == "NEW" || option == "CREATE")
        {
            option = "CREATE";
            if (boost::filesystem::exists(aFilename))
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
        boost::filesystem::path absFilepath = boost::filesystem::absolute(aFilename);
        boost::filesystem::path fileDir = absFilepath.parent_path();
        KTDEBUG(publog, "Opening file <" << absFilepath << "> for parent <" << aParent << ">");

        // find/create the file construct
        FileConstMapIt tThisFileConst = fFiles.find(aFilename);
        if (tThisFileConst == fFiles.end())
        {
            auto insertion = fFiles.insert(FileConstMap::value_type(absFilepath.native(), FileConstruct()));
            if (! insertion.second)
            {
                KTERROR(publog, "Unable to add file construct for <" << aFilename << ">");
                return nullptr;
            }
            tThisFileConst = insertion.first;
            tThisFileConst->second.fOption = option;
            tThisFileConst->second.fDir = fileDir;
        }

        // construct the unique filename that will be used for this particular contribution to the eventual file
        boost::filesystem::path filePath = fileDir / boost::filesystem::unique_path();
        KTDEBUG(publog, "Temporary file path for file <" << aFilename << "> with parent <" << aParent << "> is <" << filePath << ">");

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
            KTERROR(publog, "File <" << aFilename << "> was not created for parent <" << aParent << ">!");
            return nullptr;
        }

        // insert file object into the file construct
        tThisFileConst->second.fOpenFiles.insert(FileMap::value_type(aParent, newFile));

        KTINFO(publog, "Created file <" << aFilename << "> for parent <" << aParent << ">");

        return newFile;
    }

    Nymph::KTWriter* KTROOTWriterFileManager::FindTempPointer(const FileMap& aMap) const
    {
        Nymph::KTWriter* tempWriter = 0x0;
        while (aMap.find(tempWriter) != aMap.end())
        {
            ++tempWriter;
        }
        return tempWriter;
    }

} /* namespace Katydid */
