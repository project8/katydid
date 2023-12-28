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

#include "logger.hh"

namespace Katydid
{
    
    LOGGER(termlog, "KTTerminalWriter");

    class KTTerminalWriter;

    typedef Nymph::KTDerivedTypeWriter< KTTerminalWriter > KTTerminalTypeWriter;

  /*!
     @class KTTerminalWriter
     @author N. S. Oblath

     @brief Prints data to the terminal

     @details 

     Configuration name: "terminal-writer"

     Available configuration values:

     Slots:
     - "header": void (Nymph::KTDataPtr) -- Print the egg header information
     - "ts": void (Nymph::KTDataPtr) -- Prints information about each slice
     - "dig": void (Nymph::KTDataPtr) -- Prints information about digitizer test results
     - "track": void (Nymph::KTDataPtr) -- Prints information about a processed track
     - "fit-result": void (Nymph::KTDataPtr) -- Prints information about a density fit
     - "summary": void (const KTProcSummary*) -- Prints the processor summary information
     
    */

    class KTTerminalWriter : public Nymph::KTWriterWithTypists< KTTerminalWriter, KTTerminalTypeWriter >
    {
        public:
            KTTerminalWriter(const std::string& name = "terminal-writer");
            virtual ~KTTerminalWriter();

            bool Configure(const scarab::param_node* node);

    };

} /* namespace Katydid */
#endif /* KTTERMINALWRITER_HH_ */
