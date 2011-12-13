#ifndef KTMESSAGE_H_
#define KTMESSAGE_H_

#include "Rtypes.h"

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

namespace Katydid
{

    class KTTextFile;

    typedef UInt_t KTMessageSeverity;
    class KTMessageNewline
    {
    };
    class KTMessageEnd
    {
    };

    static const KTMessageSeverity eError = 0;
    static const KTMessageSeverity eWarning = 1;
    static const KTMessageSeverity eNormal = 2;
    static const KTMessageSeverity eDebug = 3;
    static const KTMessageNewline ret = KTMessageNewline();
    static const KTMessageEnd eom = KTMessageEnd();

    class KTMessage
    {
        public:
            KTMessage();
            virtual ~KTMessage();

            //**************
            //identification
            //**************

        public:
            const string& GetKey();
            void SetKey( const string& aKey );

        protected:
            string fKey;

            //*********
            //interface
            //*********

        public:
            KTMessage& operator+( const KTMessageSeverity& aSeverity );
            KTMessage& operator() ( const KTMessageSeverity& aSeverity );

            KTMessage& operator<( const KTMessageNewline& );
            KTMessage& operator<( const KTMessageEnd& );
            template< class XPrintable >
            KTMessage& operator<( const XPrintable& aFragment )
            {
                fOriginLine << aFragment;
                return *this;
            }

            KTMessage& operator<<( const KTMessageNewline& );
            KTMessage& operator<<( const KTMessageEnd& );
            template< class XPrintable >
            KTMessage& operator<<( const XPrintable& aFragment )
            {
                fMessageLine << aFragment;
                return *this;
            }

        private:
            void SetSeverity( const KTMessageSeverity& aSeverity );
            void Flush();
            void InitiateShutdown();

        protected:
            string fSystemDescription;
            string fSystemPrefix;
            string fSystemSuffix;

            string fTopSeparator;
            string fBottomSeparator;

            string fErrorColorPrefix;
            string fErrorColorSuffix;
            string fErrorDescription;

            string fWarningColorPrefix;
            string fWarningColorSuffix;
            string fWarningDescription;

            string fNormalColorPrefix;
            string fNormalColorSuffix;
            string fNormalDescription;

            string fDebugColorPrefix;
            string fDebugColorSuffix;
            string fDebugDescription;

            string fDefaultColorPrefix;
            string fDefaultColorSuffix;
            string fDefaultDescription;

        private:
            KTMessageSeverity fSeverity;
            string fPrefix;
            string fSuffix;

            stringstream fDescriptionBuffer;
            string fDescription;

            stringstream fOriginLine;
            vector< string > fOriginLines;

            stringstream fMessageLine;
            vector< string > fMessageLines;

            //********
            //settings
            //********

        public:
            void SetTerminalVerbosity( KTMessageSeverity aVerbosity );
            void SetLogVerbosity( KTMessageSeverity aVerbosity );
            void SetLogFile( KTTextFile* aLogFile );

        private:
            KTMessageSeverity fTerminalVerbosity;
            KTMessageSeverity fLogVerbosity;
            KTTextFile* fLogFile;
    };

}

#endif /* KTMESSAGE_H_ */
