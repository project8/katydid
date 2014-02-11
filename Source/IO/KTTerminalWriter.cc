/*
 * KTTerminalWriter.cc
 *
 *  Created on: Dec 20 2013
 *      Author: nsoblath
 */

#include "KTTerminalWriter.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    static KTNORegistrar< KTWriter, KTTerminalWriter > sTermWriterRegistrar("terminal-writer");
    static KTNORegistrar< KTProcessor, KTTerminalWriter > sTermWProcRegistrar("terminal-writer");

    KTTerminalWriter::KTTerminalWriter(const std::string& name) :
            KTWriterWithTypists< KTTerminalWriter >(name)
    {
    }

    KTTerminalWriter::~KTTerminalWriter()
    {
    }

    bool KTTerminalWriter::Configure(const KTPStoreNode* node)
    {
        return true;
    }

} /* namespace Katydid */
