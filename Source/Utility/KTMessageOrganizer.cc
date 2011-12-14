/*
 * KTMessageOrganizer.cxx
 *
 *  Created on: Nov 1, 2011
 *      Author: nsoblath
 */

#include "KTMessageOrganizer.hh"

//#include "KTTextFile.hh"
#include "KTMessage.hh"

#include "KTUtilityMessage.hh"

namespace Katydid
{

    KTMessageOrganizer* KTMessageOrganizer::fInstance = NULL;
    KTMessageOrganizer* KTMessageOrganizer::GetInstance()
    {
        if( fInstance == NULL )
        {
            fInstance = new KTMessageOrganizer();
        }
        return fInstance;
    }

    KTMessageOrganizer::KTMessageOrganizer() :
        //KTCommandLineUser(),
        fMessageMap(),
        //fLogFile( NULL ),
        fTerminalVerbosity( eNormal )//,
        //fLogVerbosity( eNormal )
    {
#ifdef DEBUG_VERBOSE
        fTerminalVerbosity = eDebug;
        //fLogVerbosity = eDebug;
#endif
#ifdef DEBUG_VERBOSE
        if (fTerminalVerbosity >= eDebug)
            cout << "KTMessageOrganizer::KTMessageOrganizer: Message organizer created" << endl;
#endif
    }

    KTMessageOrganizer::~KTMessageOrganizer()
    {
    }
    /*
    void KTMessageOrganizer::SetLogFile( KTTextFile* aTextFile )
    {
        fLogFile = aTextFile;
        MessageIt MIt;
        for( MIt = fMessageMap.begin(); MIt != fMessageMap.end(); MIt++ )
        {
            MIt->second->SetLogFile( fLogFile );
        }
        return;
    }
    */
    //********
    //messages
    //********

    void KTMessageOrganizer::AddMessage( KTMessage* aMessage )
    {
        string aKey = aMessage->GetKey();
        MessageIt TIt = fMessageMap.find( aKey );
        if( TIt == fMessageMap.end() )
        {
            aMessage->SetTerminalVerbosity( fTerminalVerbosity );
            //aMessage->SetLogVerbosity( fLogVerbosity );
            //aMessage->SetLogFile( fLogFile );

            fMessageMap.insert( MessageEntry( aMessage->GetKey(), aMessage ) );
        }
        return;
    }
    KTMessage* KTMessageOrganizer::GetMessage( const string& aKey )
    {
        MessageIt TIt = fMessageMap.find( aKey );
        if( TIt != fMessageMap.end() )
        {
            return TIt->second;
        }
        return NULL;
    }
    KTMessage* KTMessageOrganizer::DemandMessage( const string& aKey )
    {
        MessageIt TIt = fMessageMap.find( aKey );
        if( TIt != fMessageMap.end() )
        {
            return TIt->second;
        }
        else
        {
            KTMessage* aMessage = new KTMessage();

            aMessage->SetKey( aKey );
            aMessage->SetTerminalVerbosity( fTerminalVerbosity );
            //aMessage->SetLogVerbosity( fLogVerbosity );
            //aMessage->SetLogFile( fLogFile );

            fMessageMap.insert( MessageEntry( aMessage->GetKey(), aMessage ) );

            return aMessage;
        }
    }
    void KTMessageOrganizer::RemoveMessage( KTMessage* aFile )
    {
        const string& aKey = aFile->GetKey();
        MessageIt TIt = fMessageMap.find( aKey );
        if( TIt != fMessageMap.end() )
        {
            fMessageMap.erase( TIt );
        }
        return;
    }

    void KTMessageOrganizer::SetTerminalVerbosity( const UInt_t& aVerbosity )
    {
        MessageIt MIt;
        for( MIt = fMessageMap.begin(); MIt != fMessageMap.end(); MIt++ )
        {
            MIt->second->SetTerminalVerbosity( aVerbosity );
        }

        fTerminalVerbosity = aVerbosity;

        return;
    }
    /*
    void KTMessageOrganizer::SetLogVerbosity( const UInt_t& aVerbosity )
    {
        MessageIt MIt;
        for( MIt = fMessageMap.begin(); MIt != fMessageMap.end(); MIt++ )
        {
            MIt->second->SetLogVerbosity( aVerbosity );
        }

        fLogVerbosity = aVerbosity;
        return;
    }
    */
    /*
    void KTMessageOrganizer::AddCommandLineOptions()
    {
        string tKey("Message");
        Bool_t OptionsSet = fCLHandler->ProposeNewOptionGroup( tKey, "Message options" );
        if( OptionsSet )
        {
            OptionsSet = OptionsSet && fCLHandler->AddOption<UInt_t>( tKey, "Set the global terminal verbosity", "verbosity", 'V' );
            //OptionsSet = OptionsSet && fCLHandler->AddOption<UInt_t>( tKey, "Set the global log verbosity", "log-verbosity", 'L' );
            OptionsSet = OptionsSet && fCLHandler->FinalizeNewOptionGroup( tKey );
        }
        if ( ! OptionsSet )
        {
            umsg < "KTMessageOrganizer::AddCommandLineOptions";
            umsg(eWarning) << "Something went wrong while command line options were being set!" << ret;
            umsg << "Message command line options are not available." << eom;
        }
        return;
    }

    void KTMessageOrganizer::UseParsedCommandLineImmediately()
    {
        if( fCLHandler->IsCommandLineOptSet( "verbosity" ) )
            SetTerminalVerbosity( fCLHandler->GetCommandLineValue< UInt_t >( "verbosity" ) );
        //if ( fCLHandler->IsCommandLineOptSet( "log-verbosity" ) )
            //SetLogVerbosity( fCLHandler->GetCommandLineValue< UInt_t >( "log-verbosity" ) );
        return;
    }
    */

} /* namespace Katydid */
