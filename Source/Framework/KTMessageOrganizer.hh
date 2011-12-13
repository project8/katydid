/*
 * KTMessageOrganizer.hh
 *
 *  Created on: Nov 1, 2011
 *      Author: nsoblath
 */

#ifndef KTMESSAGEORGANIZER_H_
#define KTMESSAGEORGANIZER_H_

#include "KTCommandLineUser.hh"

#include <map>
using std::map;

#include <string>
using std::string;

namespace Katydid
{
    class KTMessage;
    class KTTextFile;

    class KTMessageOrganizer :
        public KTCommandLineUser
    {

        public:
            static KTMessageOrganizer* GetInstance();

        private:
            KTMessageOrganizer();
            virtual ~KTMessageOrganizer();
            static KTMessageOrganizer* fInstance;

        private:
            typedef map< string, KTMessage* > MessageMap;
            typedef MessageMap::value_type MessageEntry;
            typedef MessageMap::iterator MessageIt;
            typedef MessageMap::const_iterator MessageCIt;

        public:
            void AddMessage( KTMessage* aMessage );
            KTMessage* GetMessage( const string& aMessage );
            KTMessage* DemandMessage( const string& aMessage );
            void RemoveMessage( KTMessage* aMessage );

            void SetLogFile( KTTextFile* aFile );
            void SetTerminalVerbosity( const UInt_t& aVerbosity );
            void SetLogVerbosity( const UInt_t& aVerbosity );

        private:
            MessageMap fMessageMap;
            KTTextFile* fLogFile;
            UInt_t fTerminalVerbosity;
            UInt_t fLogVerbosity;

        public:
            virtual void AddCommandLineOptions();
            virtual void UseParsedCommandLineImmediately();

    };

} /* namespace Katydid */
#endif /* KTMESSAGEORGANIZER_H_ */
