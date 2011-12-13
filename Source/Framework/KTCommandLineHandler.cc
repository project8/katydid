/*
 * KTCommandLineHandler.cxx
 *
 *  Created on: Nov 21, 2011
 *      Author: nsoblath
 */

#include "KTCommandLineHandler.hh"

#include "KTCommandLineUser.hh"

#include "KatydidConfig.hh"

#include "KTIOMessage.hh"

#ifndef PACKAGE_STRING
#define PACKAGE_STRING Katydid (unknown version)
#endif
#define STRINGIFY_1(x) #x
#define STRINGIFY(x) STRINGIFY_1(x)

namespace Katydid
{
    KTCommandLineHandler* KTCommandLineHandler::fInstance = NULL;
    KTDestroyer< KTCommandLineHandler > KTCommandLineHandler::fCLHandlerDestroyer;

    KTCommandLineHandler* KTCommandLineHandler::GetInstance()
    {
        if (fInstance == NULL)
        {
            fInstance = new KTCommandLineHandler();
            fCLHandlerDestroyer.SetDoomed(fInstance);
        }
        return fInstance;
    }

    KTCommandLineHandler::KTCommandLineHandler() :
            fExecutableName("NONE"),
            fPackageString(STRINGIFY(PACKAGE_STRING)),
            fNArgs(0),
            fArgV(NULL),
            fArgumentsTaken(kFALSE),
            fCommandLineOptions(),
            fPrintHelpOptions(),
            fCommandLineParseLater(),
            fCommandLineVarMap(),
            fPrintHelpMessage(kFALSE),
            fPrintVersion(kFALSE),
            fIOConfigFileName("NONE")
    {
    }

    KTCommandLineHandler::~KTCommandLineHandler()
    {
        while (! fProposedGroups.empty())
        {
            OptDescMapIt tIter = fProposedGroups.begin();
            delete tIter->second;
            fProposedGroups.erase(tIter);
        }
        cout << "erasing the command line users: " << fUsers.size() << endl;
        while (! fUsers.empty())
        {
            set< KTCommandLineUser* >::iterator tIter = fUsers.begin();
            (*tIter)->fCLHandler = NULL;
            fUsers.erase(tIter);
        }
    }

    Bool_t KTCommandLineHandler::TakeArguments(Int_t argC, Char_t**argV)
    {
        if (fArgumentsTaken) return kFALSE;

        fNArgs = argC;
        fArgV = argV;
        fArgumentsTaken = kTRUE;

        InitialCommandLineProcessing();

        return kTRUE;
    }

    //**************

    Bool_t KTCommandLineHandler::GetArgumentsTaken()
    {
        return fArgumentsTaken;
    }

    Int_t KTCommandLineHandler::GetNArgs()
    {
        return fNArgs;
    }

    Char_t** KTCommandLineHandler::GetArgV()
    {
        return fArgV;
    }

    //**************

    Bool_t KTCommandLineHandler::ProposeNewOptionGroup(const string& aKey, const string& aTitle)
    {
        if (fAllGroupKeys.find(aKey) != fAllGroupKeys.end())
        {
            iomsg < "KTCommandLineHandler::ProposeNewOptionGroup";
            iomsg(eWarning) << "There is already an option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }
        if (fProposedGroups.find(aKey) != fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::ProposeNewOptionGroup";
            iomsg(eWarning) << "There is already a proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }

        po::options_description* tNewOpts = new po::options_description(aTitle);
        fProposedGroups.insert(OptDescMap::value_type(aKey, tNewOpts));

        return kTRUE;
    }

    Bool_t KTCommandLineHandler::AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt, Char_t aShortOpt)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There is already an option called <" << aLongOpt << ">" << eom;
            return kFALSE;
        }
        if (aShortOpt != '#')
        {
            if (fAllOptionsShort.find(aShortOpt) != fAllOptionsShort.end())
            {
                iomsg < "KTCommandLineHandler::AddOption";
                iomsg(eWarning) << "There is already a short option called <" << aShortOpt << ">" << eom;
                return kFALSE;
            }
        }

        // option is okay at this point

        string tOptionName = aLongOpt;
        fAllOptionsLong.insert(aLongOpt);
        if (aShortOpt != '#')
        {
            tOptionName += "," + string(&aShortOpt);
            fAllOptionsShort.insert(aShortOpt);
        }
        tIter->second->add_options()(tOptionName.c_str(), aHelpMsg.c_str());

        return kTRUE;
    }

    Bool_t KTCommandLineHandler::AddOption(const string& aKey, const string& aHelpMsg, const string& aLongOpt)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There is already an option called <" << aLongOpt << ">" << eom;
            return kFALSE;
        }

        // option is okay at this point

        fAllOptionsLong.insert(aLongOpt);
        tIter->second->add_options()(aLongOpt.c_str(), aHelpMsg.c_str());

        return kTRUE;
    }

    po::options_description* KTCommandLineHandler::GetOptionsDescription(const string& aKey)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::AddOption";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return NULL;
        }
        return tIter->second;
    }

    Bool_t KTCommandLineHandler::FinalizeNewOptionGroup(const string& aKey)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if ( tIter == fProposedGroups.end())
        {
            iomsg < "KTCommandLineHandler::FinalizeNewOptionGroup";
            iomsg(eWarning) << "There no proposed option group with key <" << aKey << ">" << eom;
            return kFALSE;
        }

        if (! AddCommandLineOptions(*(tIter->second))) return kFALSE;
        delete tIter->second;
        fProposedGroups.erase(tIter);

        return kTRUE;
    }

    //**************

    Bool_t KTCommandLineHandler::AddCommandLineOptions(const po::options_description& aSetOfOpts)
    {
        try
        {
            fCommandLineOptions.add(aSetOfOpts);
            fPrintHelpOptions.add(aSetOfOpts);
            return kTRUE;
        }
        catch (std::exception& e)
        {
            iomsg < "KTCommandLineHandler::AddCommandLineOptions";
            iomsg(eWarning) << "Exception thrown while adding options: " << e.what() << eom;
            return kFALSE;
        }
        catch (...)
        {
            iomsg < "KTCommandLineHandler::AddCommandLineOptions";
            iomsg(eWarning) << "Exception was thrown, but caught in a generic way!" << eom;
            return kFALSE;
        }
        return kFALSE;
    }

    //**************

    Bool_t KTCommandLineHandler::IsCommandLineOptSet(const string& aCLOption)
    {
        return fCommandLineVarMap.count(aCLOption);
    }

    //**************

    void KTCommandLineHandler::ProcessCommandLine()
    {
        // Get options from all command line users
        AddOptionsFromAllUsers();

        // Parse the command line options that remain after the initial parsing
        po::parsed_options tParsedOpts = po::command_line_parser(fCommandLineParseLater).options(fCommandLineOptions).run();

        // Create the variable map from the parse options
        po::store(tParsedOpts, fCommandLineVarMap);
        po::notify(fCommandLineVarMap);

        this->NotifyUsersOfParsing();

        return;
    }

    void KTCommandLineHandler::InitialCommandLineProcessing()
    {
        if (fNArgs <= 0 || fArgV == NULL)
        {
            fNArgs = 0;
            fArgV = NULL;
            return;
        }

        // Get the executable name
        if (fNArgs >= 1) fExecutableName = string(fArgV[0]);

        // Define general options, and add them to the complete option list
        po::options_description tGeneralOpts("General options");
        tGeneralOpts.add_options()("help,h", "Print help message")("version,v", "Print version information");
        // We want to have the general options printed if --help is used
        fPrintHelpOptions.add(tGeneralOpts);

        // Fill in the duplication-checking sets
        fAllGroupKeys.insert("General");
        fAllOptionsLong.insert("help");
        fAllOptionsShort.insert('h');
        fAllOptionsLong.insert("version");
        fAllOptionsShort.insert('v');

        // Define the option for the user configuration file; this does not get printed in list of options when --help is used
        po::options_description tHiddenOpts("Hidden options");
        tHiddenOpts.add_options()("user-config", po::value< string >(), "Assign the UserConfiguration file");

        // Add together any options that will be parsed here, in the initial command-line processing
        po::options_description tInitialOptions("Initial options");
        tInitialOptions.add(tGeneralOpts).add(tHiddenOpts);

        // Allow the UserConfiguration file to be specified with the only positional option
        po::positional_options_description tPositionOpt;
        tPositionOpt.add("user-config", 1);

        // Command line style
        po::command_line_style::style_t pstyle = po::command_line_style::unix_style;
        /*
        po::command_line_style::style_t pstyle = po::command_line_style::style_t(
                po::command_line_style::allow_long |
                po::command_line_style::allow_short |
                po::command_line_style::allow_dash_for_short |
                po::command_line_style::long_allow_next |
                po::command_line_style::long_allow_adjacent |
                po::command_line_style::short_allow_next |
                po::command_line_style::short_allow_adjacent |
                po::command_line_style::allow_guessing |
                po::command_line_style::allow_sticky);
        */

        // Parse the command line looking only for the general options
        po::parsed_options tParsedOpts = po::command_line_parser(fNArgs, fArgV).style(pstyle).options(tInitialOptions).positional(tPositionOpt).allow_unregistered().run();
        // Save the remaining command-line options for later parsing (after the full option list has been populated)
        fCommandLineParseLater = po::collect_unrecognized(tParsedOpts.options, po::exclude_positional);
        /* some debugging couts
        std::cout << "there are " << fCommandLineParseLater.size() << " tokens to parse later." << std::endl;
        for (UInt_t i = 0; i < fCommandLineParseLater.size(); i++)
        {
            std::cout << "   " << fCommandLineParseLater[i] << std::endl;
        }
        */

        // Create the variable map from the general options
        po::variables_map tGeneralOptsVarMap;
        po::store(tParsedOpts, tGeneralOptsVarMap);
        po::notify(tGeneralOptsVarMap);

        // Use the general options information
        if (tGeneralOptsVarMap.count("help")) fPrintHelpMessage = kTRUE;
        if (tGeneralOptsVarMap.count("version")) fPrintVersion = kTRUE;
        if (tGeneralOptsVarMap.count("user-config"))
        {
            fIOConfigFileName = tGeneralOptsVarMap["user-config"].as< string >();
        }
    }

    //**************

    void KTCommandLineHandler::RegisterUser(KTCommandLineUser* aUser)
    {
        fUsers.insert(aUser);
        return;
    }

    void KTCommandLineHandler::RemoveUser(KTCommandLineUser* aUser)
    {
        set< KTCommandLineUser* >::iterator tIter=fUsers.find(aUser);
        if (tIter != fUsers.end()) fUsers.erase(tIter);
        return;
    }

    void KTCommandLineHandler::AddOptionsFromAllUsers()
    {
        for (set< KTCommandLineUser* >::iterator tIter=fUsers.begin(); tIter != fUsers.end(); tIter++)
        {
            if (! (*tIter)->fCommandLineOptionsAdded)
            {
                (*tIter)->AddCommandLineOptions();
                (*tIter)->fCommandLineOptionsAdded = kTRUE;
            }
        }
        return;
    }

    void KTCommandLineHandler::NotifyUsersOfParsing()
    {
        for (set< KTCommandLineUser* >::iterator tIter=fUsers.begin(); tIter != fUsers.end(); tIter++)
        {
            (*tIter)->UseParsedCommandLineImmediately();
            (*tIter)->fCommandLineIsParsed = kTRUE;
        }
        return;
    }

    //**************

    void KTCommandLineHandler::PrintHelpMessageAndExit(const string& aApplicationType)
    {
        this->AddOptionsFromAllUsers();

        string tLocation;
        if (aApplicationType == string("")) tLocation = string("KTCommandLineHandler -- Version Information");
        else tLocation = aApplicationType + string(" -- Version Information");
        iomsg < tLocation;
        iomsg(eNormal) << "Usage: " << fExecutableName << " [user-config file] [options]" << ret << ret;
        iomsg << "   user-config file: The relative or absolute path for the desired user configuration file" << ret;

        iomsg << fPrintHelpOptions << eom;

        exit(0);
    }

    void KTCommandLineHandler::PrintVersionMessageAndExit(const string& aApplicationType, const string& aApplicationString)
    {
        string tLocation;
        if (aApplicationType == string("")) tLocation = string("KTCommandLineHandler -- Version Information");
        else tLocation = aApplicationType + string(" -- Version Information");
        iomsg < tLocation;
        iomsg(eNormal) << "Executable: " << fExecutableName << ret;
        if (aApplicationString != string(""))
            iomsg << "Application: " << aApplicationString << ret;
        iomsg << "Built with: " << fPackageString << eom;
        exit(0);
    }



} /* namespace Katydid */
