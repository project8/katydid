/*
 * KTTerminalWriter.cc
 *
 *  Created on: Dec 20 2013
 *      Author: nsoblath
 */

#include "KTTerminalWriter.hh"

#include "KTParam.hh"

using std::string;

namespace Katydid
{
    KT_REGISTER_WRITER(KTTerminalWriter, "terminal-writer");
    KT_REGISTER_PROCESSOR(KTTerminalWriter, "terminal-writer");

    KTTerminalWriter::KTTerminalWriter(const std::string& name) :
            KTWriterWithTypists< KTTerminalWriter, KTTerminalTypeWriter >(name)
    {
    }

    KTTerminalWriter::~KTTerminalWriter()
    {
    }

    bool KTTerminalWriter::Configure(const KTParamNode* node)
    {
        return true;
    }

} /* namespace Katydid */
