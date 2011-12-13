#include "KTMessage.hh"

#include "KTCoreManagerTable.hh"
#include "KTManagerTable.hh"

#include "KTTextFile.hh"

#include <cstdio>
#include <cstdlib>
#include <execinfo.h>

namespace Katydid
{

    KTMessage::KTMessage() :
        fKey( "" ),
        fSystemDescription( "GENERIC" ),
        fSystemPrefix( "" ),
        fSystemSuffix( "" ),

        fTopSeparator( "**********" ),
        fBottomSeparator( "**********" ),

        fErrorColorPrefix( "\33[31;1m" ),
        fErrorColorSuffix( "\33[0m" ),
        fErrorDescription( "ERROR" ),

        fWarningColorPrefix( "\33[33;1m" ),
        fWarningColorSuffix( "\33[0m" ),
        fWarningDescription( "WARNING" ),

        fNormalColorPrefix( "\33[32;1m" ),
        fNormalColorSuffix( "\33[0m" ),
        fNormalDescription( "NORMAL" ),

        fDebugColorPrefix( "\33[36;1m" ),
        fDebugColorSuffix( "\33[0m" ),
        fDebugDescription( "DEBUG" ),

        fDefaultColorPrefix( "\33[37;1m" ),
        fDefaultColorSuffix( "\33[0m" ),
        fDefaultDescription( "UNKNOWN" ),

        fSeverity( eNormal ),
        fPrefix( fNormalColorPrefix + fSystemPrefix ),
        fSuffix( fSystemSuffix + fNormalColorSuffix ),

        fDescriptionBuffer(),
        fDescription( "" ),

        fOriginLine(),
        fOriginLines(),

        fMessageLine(),
        fMessageLines(),

        fTerminalVerbosity( eNormal ),
        fLogVerbosity( eNormal ),
        fLogFile( NULL )
    {
        fTerminalVerbosity = eDebug;
        fLogVerbosity = eDebug;
    }
    KTMessage::~KTMessage()
    {
    }

    const string& KTMessage::GetKey()
    {
        return fKey;
    }
    void KTMessage::SetKey( const string& aKey )
    {
        fKey = aKey;
        return;
    }

    KTMessage& KTMessage::operator+( const KTMessageSeverity& aSeverity )
    {
        SetSeverity( aSeverity );
        return *this;
    }
    KTMessage& KTMessage::operator()( const KTMessageSeverity& aSeverity )
    {
        SetSeverity( aSeverity );
        return *this;
    }

    KTMessage& KTMessage::operator<( const KTMessageNewline& )
    {
        fOriginLines.push_back( fOriginLine.str() );

        fOriginLine.clear();
        fOriginLine.str( "" );

        return *this;
    }
    KTMessage& KTMessage::operator<( const KTMessageEnd& )
    {
        Flush();
        return *this;
    }

    KTMessage& KTMessage::operator<<( const KTMessageNewline& )
    {
        fMessageLines.push_back( fMessageLine.str() );

        fMessageLine.clear();
        fMessageLine.str( "" );
        return *this;
    }
    KTMessage& KTMessage::operator<<( const KTMessageEnd& )
    {
        Flush();
        return *this;
    }

    void KTMessage::SetSeverity( const KTMessageSeverity& aSeverity )
    {
        fDescriptionBuffer.clear();
        fDescriptionBuffer.str( "" );

        switch( aSeverity )
        {

            case eError:
                fPrefix = fErrorColorPrefix + fSystemPrefix;
                fSuffix = fSystemSuffix + fErrorColorSuffix;
                fDescriptionBuffer << fSystemDescription << " " << fErrorDescription << " MESSAGE";
                break;

            case eWarning:
                fPrefix = fWarningColorPrefix + fSystemPrefix;
                fSuffix = fSystemSuffix + fWarningColorSuffix;
                fDescriptionBuffer << fSystemDescription << " " << fWarningDescription << " MESSAGE";
                break;

            case eNormal:
                fPrefix = fNormalColorPrefix + fSystemPrefix;
                fSuffix = fSystemSuffix + fNormalColorSuffix;
                fDescriptionBuffer << fSystemDescription << " " << fNormalDescription << " MESSAGE";
                break;

            case eDebug:
                fPrefix = fDebugColorPrefix + fSystemPrefix;
                fSuffix = fSystemSuffix + fDebugColorSuffix;
                fDescriptionBuffer << fSystemDescription << " " << fDebugDescription << " MESSAGE";
                break;

            default:
                fPrefix = fDefaultColorPrefix + fSystemPrefix;
                fSuffix = fSystemSuffix + fDefaultColorSuffix;
                fDescriptionBuffer << fSystemDescription << " " << fDefaultDescription << " MESSAGE";
                break;
        }

        fSeverity = aSeverity;
        fDescription = fDescriptionBuffer.str();

        return;
    }
    void KTMessage::Flush()
    {
        fOriginLines.push_back( fOriginLine.str() );
        fMessageLines.push_back( fMessageLine.str() );

        if( fSeverity <= fTerminalVerbosity )
        {
            cout << fPrefix << fTopSeparator << fSuffix << '\n';
            cout << fPrefix << fDescription << fSuffix << '\n';

            cout << fPrefix << "ORIGIN:" << fSuffix << '\n';
            for( vector< string >::iterator It = fOriginLines.begin(); It != fOriginLines.end(); It++ )
            {
                cout << fPrefix << "  " << *It << fSuffix << '\n';
            }

            cout << fPrefix << "MESSAGE:" << fSuffix << '\n';
            for( vector< string >::iterator It = fMessageLines.begin(); It != fMessageLines.end(); It++ )
            {
                cout << fPrefix << "  " << *It << fSuffix << '\n';
            }

            cout << fPrefix << fBottomSeparator << fSuffix << '\n';
        }

        if( (fSeverity <= fLogVerbosity) && (fLogFile != NULL) && (fLogFile->IsOpen() == kTRUE) )
        {
            *(fLogFile->File()) << fSystemPrefix << fTopSeparator << fSystemSuffix << '\n';
            *(fLogFile->File()) << fSystemPrefix << fDescription << fSystemSuffix << '\n';

            *(fLogFile->File()) << fSystemPrefix << "ORIGIN:" << fSystemSuffix << '\n';
            for( vector< string >::iterator It = fOriginLines.begin(); It != fOriginLines.end(); It++ )
            {
                *(fLogFile->File()) << fSystemPrefix << "  " << *It << fSystemSuffix << '\n';
            }

            *(fLogFile->File()) << fSystemPrefix << "MESSAGE:" << fSystemSuffix << '\n';
            for( vector< string >::iterator It = fMessageLines.begin(); It != fMessageLines.end(); It++ )
            {
                *(fLogFile->File()) << fSystemPrefix << "  " << *It << fSystemSuffix << '\n';
            }
            *(fLogFile->File()) << fSystemPrefix << fBottomSeparator << fSystemSuffix << '\n';
        }

        while( !fOriginLines.empty() )
        {
            fOriginLines.pop_back();
        }
        fOriginLine.clear();
        fOriginLine.str( "" );

        while( !fMessageLines.empty() )
        {
            fMessageLines.pop_back();
        }
        fMessageLine.clear();
        fMessageLine.str( "" );

        if( fSeverity == eError )
        {
            InitiateShutdown();
        }

        return;
    }

    void KTMessage::InitiateShutdown()
    {
        // For obvious reasons, printing should be done here with cout, not any derived message class
        string tSourceOfShutdown( "KTManagerTable" );
        if( KTCoreManagerTable::GetInstance()->CoreManagerExists() )
        {
            tSourceOfShutdown = string( "KTCoreManagerTable" );
        }

        // This is for the situation in which a CoreManager is being used
        if( fSeverity <= fTerminalVerbosity )
        {
            cout << fPrefix << fTopSeparator << fSuffix << '\n';
            cout << fPrefix << "KTMessage FATAL ERROR MESSAGE" << fSuffix << '\n';
            cout << fPrefix << "ORIGIN:" << fSuffix << '\n';
            cout << fPrefix << "  KTMessage::InitiateShutdown" << fSuffix << '\n';
            cout << fPrefix << "MESSAGE:" << fSuffix << '\n';
            cout << fPrefix << "  Initiating shutdown via the KTCoreManagerTable" << fSuffix << '\n';

            // print the stack trace
            cout << '\n';
            void *array[10];
            size_t size;
            Char_t **strings;
            size_t i;
            size = backtrace(array, 10);
            strings = backtrace_symbols(array, size);
            cout << fPrefix <<  "Stack trace: obtained " << size << " stack frames.\n";
            for (i=0; i<size; i++)
                cout << fPrefix << strings[i] << '\n';
            free(strings);
            cout << '\n';

            cout << fPrefix << fBottomSeparator << fSuffix << endl;
        }
        if( (fSeverity <= fLogVerbosity) && (fLogFile != NULL) && (fLogFile->IsOpen() == kTRUE) )
        {
            *(fLogFile->File()) << fSystemPrefix << fTopSeparator << fSystemSuffix << '\n';
            *(fLogFile->File()) << fPrefix << "KTMessage FATAL ERROR MESSAGE" << fSystemSuffix << '\n';
            *(fLogFile->File()) << fPrefix << "ORIGIN:" << fSystemSuffix << '\n';
            *(fLogFile->File()) << fPrefix << "  KTMessage::InitiateShutdown" << fSystemSuffix << '\n';
            *(fLogFile->File()) << fPrefix << "MESSAGE:" << fSystemSuffix << '\n';
            *(fLogFile->File()) << fPrefix << "  Initiating shutdown via the KTCoreManagerTable" << fSystemSuffix << '\n';

            // print the stack trace
            void *array[10];
            size_t size;
            Char_t **strings;
            size_t i;
            size = backtrace(array, 10);
            strings = backtrace_symbols(array, size);
            *(fLogFile->File()) << '\n';
            *(fLogFile->File()) << fPrefix << "Stack trace: obtained " << size << " stack frames.\n";
            for (i=0; i<size; i++)
                *(fLogFile->File()) << fPrefix << strings[i] << '\n';
            free(strings);
            *(fLogFile->File()) << '\n';

            *(fLogFile->File()) << fPrefix << fBottomSeparator << fSystemSuffix << endl;
        }

        if( KTCoreManagerTable::GetInstance()->CoreManagerExists() )
        {
            KTCoreManagerTable::GetInstance()->ShutdownCoreManager();
        }
        else
        {
            KTManagerTable::GetInstance()->ShutdownManagers();
        }

        exit( 1 );
        return;
    }

    void KTMessage::SetTerminalVerbosity( KTMessageSeverity aVerbosity )
    {
        fTerminalVerbosity = aVerbosity;
        return;
    }
    void KTMessage::SetLogVerbosity( KTMessageSeverity aVerbosity )
    {
        fLogVerbosity = aVerbosity;
        return;
    }
    void KTMessage::SetLogFile( KTTextFile* aLogFile )
    {
        fLogFile = aLogFile;
        return;
    }

}
