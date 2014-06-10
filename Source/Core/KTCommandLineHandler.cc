/*
 * KTCommandLineHandler.cxx
 *
 *  Created on: Nov 21, 2011
 *      Author: nsoblath
 */

#include "KTCommandLineHandler.hh"

#include "KTCommandLineOption.hh"

#include "KatydidConfig.hh"

#include <sstream>

#ifndef PACKAGE_STRING
#define PACKAGE_STRING Katydid (unknown version)
#endif
#define STRINGIFY_1(x) #x
#define STRINGIFY_2(x) STRINGIFY_1(x)

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(utillog, "KTCommandLineHandler");

    CommandLineHandlerException::CommandLineHandlerException (std::string const& why)
      : std::logic_error(why)
    {}

    KTCommandLineHandler::KTCommandLineHandler() :
            fExecutableName("NONE"),
            fPackageString(STRINGIFY_2(PACKAGE_STRING)),
            fNArgs(0),
            fArgV(NULL),
            fArgumentsTaken(false),
            fCommandLineOptions(),
            fPrintHelpOptions(),
            fCommandLineParseLater(),
            fParsedOptions(NULL),
            fCommandLineVarMap(),
            fConfigOverrideValues(),
            fPrintHelpMessage(false),
            fPrintVersionMessage(false),
            fPrintHelpMessageAfterConfig(false),
            fConfigFilename(),
            fCommandLineJSON()
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
    }

    bool KTCommandLineHandler::TakeArguments(int argC, char** argV)
    {
        if (fArgumentsTaken) return false;

        fNArgs = argC;
        fArgV = argV;
        fArgumentsTaken = true;

        ProcessCommandLine();

        return true;
    }

    bool KTCommandLineHandler::ProcessCommandLine()
    {
        InitialCommandLineProcessing();

        return DelayedCommandLineProcessing();
    }

    //**************

    bool KTCommandLineHandler::GetArgumentsTaken()
    {
        return fArgumentsTaken;
    }

    int KTCommandLineHandler::GetNArgs()
    {
        return fNArgs;
    }

    char** KTCommandLineHandler::GetArgV()
    {
        return fArgV;
    }

    //**************

    KTCommandLineHandler::OptDescMapIt KTCommandLineHandler::CreateNewOptionGroup(const string& aTitle)
    {
        po::options_description* tNewOpts = new po::options_description(aTitle);
        std::pair< OptDescMapIt, bool > result = fProposedGroups.insert(OptDescMap::value_type(aTitle, tNewOpts));
        if (! result.second)
        {
            KTWARN(utillog, "There is already an option group with title <" << aTitle << ">");
            delete tNewOpts;
        }

        return result.first;
    }

    bool KTCommandLineHandler::AddOption(const string& aTitle, const string& aHelpMsg, const string& aLongOpt, char aShortOpt, bool aWarnOnDuplicate)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            if (aWarnOnDuplicate)
                KTWARN(utillog, "There is already an option called <" << aLongOpt << ">");
            return false;
        }
        if (aShortOpt != '#')
        {
            if (fAllOptionsShort.find(aShortOpt) != fAllOptionsShort.end())
            {
                if (aWarnOnDuplicate)
                        KTWARN(utillog, "There is already a short option called <" << aShortOpt << ">");
                return false;
            }
        }

        // option is okay at this point

        OptDescMapIt tIter = fProposedGroups.find(aTitle);
        if (tIter == fProposedGroups.end())
        {
            tIter = CreateNewOptionGroup(aTitle);
        }

        string tOptionName = aLongOpt;
        fAllOptionsLong.insert(aLongOpt);
        if (aShortOpt != '#')
        {
            tOptionName += "," + string(&aShortOpt);
            fAllOptionsShort.insert(aShortOpt);
        }
        tIter->second->add_options()(tOptionName.c_str(), aHelpMsg.c_str());

        return true;
    }

    bool KTCommandLineHandler::AddOption(const string& aTitle, const string& aHelpMsg, const string& aLongOpt, bool aWarnOnDuplicate)
    {
        if (fAllOptionsLong.find(aLongOpt) != fAllOptionsLong.end())
        {
            if (aWarnOnDuplicate)
                KTWARN(utillog, "There is already an option called <" << aLongOpt << ">");
            return false;
        }

        // option is okay at this point

        OptDescMapIt tIter = fProposedGroups.find(aTitle);
        if (tIter == fProposedGroups.end())
        {
            tIter = CreateNewOptionGroup(aTitle);
        }

        fAllOptionsLong.insert(aLongOpt);
        tIter->second->add_options()(aLongOpt.c_str(), aHelpMsg.c_str());

        return true;
    }

    po::options_description* KTCommandLineHandler::GetOptionsDescription(const string& aKey)
    {
        OptDescMapIt tIter = fProposedGroups.find(aKey);
        if (tIter == fProposedGroups.end())
        {
            KTWARN(utillog, "There no proposed option group with key <" << aKey << ">");
            return NULL;
        }
        return tIter->second;
    }

    //**************

   bool KTCommandLineHandler::FinalizeNewOptionGroups()
    {
        for (OptDescMapIt tIter = fProposedGroups.begin(); tIter != fProposedGroups.end(); tIter++)
        {
            if (! AddCommandLineOptions(*(tIter->second)))
            {
                return false;
            }
            delete tIter->second;
        }
        fProposedGroups.clear();

        return true;
    }

    bool KTCommandLineHandler::AddCommandLineOptions(const po::options_description& aSetOfOpts)
    {
        try
        {
            fCommandLineOptions.add(aSetOfOpts);
            fPrintHelpOptions.add(aSetOfOpts);
        }
        catch (std::exception& e)
        {
            KTERROR(utillog, "Exception thrown while adding options: " << e.what());
            return false;
        }
        catch (...)
        {
            KTERROR(utillog, "Exception was thrown, but caught in a generic way!");
            return false;
        }
        return true;
    }

    //**************

    bool KTCommandLineHandler::IsCommandLineOptSet(const string& aCLOption)
    {
        return fCommandLineVarMap.count(aCLOption) != 0;
    }

    //**************

    bool KTCommandLineHandler::DelayedCommandLineProcessing()
    {
        if (! fProposedGroups.empty())
        {
            if (! this->FinalizeNewOptionGroups())
            {
                KTERROR(utillog, "An error occurred while adding the proposed option groups\n" <<
                        "Command-line options were not parsed");
                return false;
            }
        }

        // Parse the command line options that remain after the initial parsing
        try
        {
            fParsedOptions = po::command_line_parser(fCommandLineParseLater).options(fCommandLineOptions).allow_unregistered().run();
        }
        catch (std::exception& e)
        {
            KTERROR(utillog, "An error occurred while boost was parsing the command line options:\n" << e.what());
            return false;
        }

        // these will be the unregistered items, which are assumed to be intended to edit the config file
        vector< string > tRemainingToParse = po::collect_unrecognized(fParsedOptions.options, po::include_positional);

        // Create the variable map from the parse options
        po::store(fParsedOptions, fCommandLineVarMap);
        po::notify(fCommandLineVarMap);

        // now parse the remaining items into the config override param node
        for (vector< string >::const_iterator tokenIt = tRemainingToParse.begin(); tokenIt != tRemainingToParse.end(); ++tokenIt)
        {
            string argument(*tokenIt);
            size_t t_name_pos = argument.find_first_not_of( fDash );
            size_t t_val_pos = argument.find_first_of( fSeparator );
            // the name should have 2 dashes before it, and there should be a separator
            if( t_name_pos == 2 && t_val_pos != string::npos )
            {
                string t_full_name(argument.substr( t_name_pos, t_val_pos-2 ));

                size_t t_node_start_pos = 0;
                size_t t_node_sep_pos = t_full_name.find_first_of( fNodeSeparator );
                KTParamNode* parentNode = &fConfigOverrideValues;
                while (t_node_sep_pos != string::npos)
                {
                    string nodeName(t_full_name.substr(t_node_start_pos, t_node_sep_pos));
                    if (parentNode->Has(nodeName))
                    {
                        parentNode = parentNode->NodeAt(nodeName);
                    }
                    else
                    {
                        KTParamNode* newChildNode = new KTParamNode();
                        parentNode->Add(nodeName, newChildNode);
                        parentNode = newChildNode;
                    }
                    t_node_start_pos = t_node_sep_pos + 1;
                    t_node_sep_pos = t_full_name.find_first_of(fNodeSeparator, t_node_start_pos);
                }

                string valueName(t_full_name.substr(t_node_start_pos, t_val_pos));

                KTParamValue* new_value = new KTParamValue();
                *new_value << argument.substr( t_val_pos + 1 );

                //std::cout << "(parser) adding < " << t_name << "<" << t_type << "> > = <" << new_value.value() << ">" << std::endl;

                parentNode->Replace( valueName, new_value );

                continue;
            }

            KTERROR(utillog, "Argument <" << argument << "> does not match --<name>=<value> pattern");
            return false;
        }

        return true;
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

        // If no arguments were given, just return now
        if (fNArgs == 1) return;

        // Define general options, and add them to the complete option list
        po::options_description tGeneralOpts("General options");
        tGeneralOpts.add_options()("help,h", "Print help message")("help-config", "Print help message after reading config file")("version,v", "Print version information");
        /* WHEN NOT USING POSITIONAL CONFIG FILE ARGUMENT */
        tGeneralOpts.add_options()("config,c", po::value< string >(), "Configuration file");
        tGeneralOpts.add_options()("json,j", po::value< string >(), "Command-Line JSON");
        /**/
        // We want to have the general options printed if --help is used
        fPrintHelpOptions.add(tGeneralOpts);

        // Fill in the duplication-checking sets
        fAllGroupKeys.insert("General");
        fAllOptionsLong.insert("help");
        fAllOptionsShort.insert('h');
        fAllOptionsLong.insert("help-config");
        fAllOptionsLong.insert("version");
        fAllOptionsShort.insert('v');

        // Define the option for the user configuration file; this does not get printed in list of options when --help is used
        po::options_description tHiddenOpts("Hidden options");
        /* WHEN USING POSITIONAL CONFIG FILE ARGUMENT
        tHiddenOpts.add_options()("config-file", po::value< string >(), "Configuration file");
        */
        // Add together any options that will be parsed here, in the initial command-line processing
        po::options_description tInitialOptions("Initial options");
        tInitialOptions.add(tGeneralOpts).add(tHiddenOpts);

        // Allow the UserConfiguration file to be specified with the only positional option
        /* WHEN USING POSITIONAL CONFIG FILE ARGUMENT
        po::positional_options_description tPositionOpt;
        tPositionOpt.add("config-file", 1);
        */

        // Add contributions of other options from elsewhere in Katydid
        FinalizeNewOptionGroups();

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

        po::parsed_options tParsedOpts(NULL);
        // Parse the command line looking only for the general options
        try
        {
            tParsedOpts = po::command_line_parser(fNArgs, fArgV).style(pstyle).options(tInitialOptions).allow_unregistered().run();
            /* WHEN USING POSITIONAL CONFIG FILE ARGUMENT
            tParsedOpts = po::command_line_parser(fNArgs, fArgV).style(pstyle).options(tInitialOptions).positional(tPositionOpt).allow_unregistered().run();
            */
        }
        catch (std::exception& e)
        {
            KTERROR(utillog, "Exception caught while performing initial CL parsing:\n"
                    << '\t' << e.what());
            throw std::logic_error(e.what());
        }
        // Save the remaining command-line options for later parsing (after the full option list has been populated)
        fCommandLineParseLater = po::collect_unrecognized(tParsedOpts.options, po::include_positional);
        /* some debugging couts
        std::cout << "there are " << fCommandLineParseLater.size() << " tokens to parse later." << std::endl;
        for (unsigned i = 0; i < fCommandLineParseLater.size(); i++)
        {
            std::cout << "   " << fCommandLineParseLater[i] << std::endl;
        }
        */

        // Create the variable map from the general options
        po::variables_map tGeneralOptsVarMap;
        po::store(tParsedOpts, tGeneralOptsVarMap);
        po::notify(tGeneralOptsVarMap);

        // Use the general options information
        if (tGeneralOptsVarMap.count("help"))
        {
            fPrintHelpMessage = true;
        }
        if (tGeneralOptsVarMap.count("help-config"))
        {
            fPrintHelpMessageAfterConfig = true;
        }
        if (tGeneralOptsVarMap.count("version"))
        {
            fPrintVersionMessage = true;
        }
        if (tGeneralOptsVarMap.count("config"))
        {
            fConfigFilename = tGeneralOptsVarMap["config"].as< string >();
        }
        if (tGeneralOptsVarMap.count("json"))
        {
            fCommandLineJSON = tGeneralOptsVarMap["json"].as< string >();
        }

        return;
    }

    //**************

    void KTCommandLineHandler::PrintHelpMessage()
    {
        KTPROG(utillog, "\nUsage: " << fExecutableName << " [options]\n\n" <<
               "  If using a config file, it should be specified as:  -c config_file.json\n" <<
               "  Config file options can be modified using:  --address.of.option=\"value\"\n" <<
               fPrintHelpOptions);
        return;
    }

    void KTCommandLineHandler::PrintVersionMessage()
    {
        KTPROG(utillog, fExecutableName << " -- Version Information\n" << "Built with: " << fPackageString);
        return;
    }



} /* namespace Katydid */
