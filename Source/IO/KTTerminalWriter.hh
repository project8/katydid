/**
 @file KTTerminalWriter.hh
 @brief Contains KTTerminalWriter
 @details Writes data to the terminal
 @author: N. S. Oblath
 @date: Dec 20 2013
 */

#ifndef KTTERMINALWRITER_HH_
#define KTTERMINALWRITER_HH_

#include "KTWriter.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(termlog, "KTTerminalWriter");

    class KTTerminalWriter;

    typedef KTDerivedTypeWriter< KTTerminalWriter > KTTerminalTypeWriter;

  /*!
     @class KTTerminalWriter
     @author N. S. Oblath

     @brief Prints data to the terminal

     @details 

     Configuration name: "terminal-writer"

     Available configuration values:

     Slots:
     - "dig":
     
    */

    class KTTerminalWriter : public KTWriterWithTypists< KTTerminalWriter >
    {
        public:
            KTTerminalWriter(const std::string& name = "terminal-writer");
            virtual ~KTTerminalWriter();

            bool Configure(const KTPStoreNode* node);

    };

} /* namespace Katydid */
#endif /* KTTERMINALWRITER_HH_ */
