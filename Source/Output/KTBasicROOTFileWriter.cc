/*
 * KTBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"

#include "KTEvent.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

#include <sstream>

using std::stringstream;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

/*
    KTBasicROOTTypeWriter::KTBasicROOTTypeWriter() :
            //KTTypeWriter(),
            fFileWriter(NULL)
    {
    }

    KTBasicROOTTypeWriter::~KTBasicROOTTypeWriter()
    {
    }
*/

    static KTDerivedRegistrar< KTWriter, KTBasicROOTFileWriter > sBRFWriterRegistrar("basic-root-writer");
    static KTDerivedRegistrar< KTProcessor, KTBasicROOTFileWriter > sBRFWProcRegistrar("basic-root-writer");

    KTBasicROOTFileWriter::KTBasicROOTFileWriter() :
            KTWriterWithTypists< KTBasicROOTFileWriter >(),
            //KTWriter(),
            //fTypeWriters(),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
    {
        fConfigName = "basic-root-writer";

        RegisterSlot("write-data", this, &KTBasicROOTFileWriter::Publish);
/*
        KTTIFactory< KTBasicROOTTypeWriter >* twFactory = KTTIFactory< KTBasicROOTTypeWriter >::GetInstance();
        for (KTTIFactory< KTBasicROOTTypeWriter >::FactoryCIt factoryIt = twFactory->GetFactoryMapBegin();
                factoryIt != twFactory->GetFactoryMapEnd();
                factoryIt++)
        {
            KTBasicROOTTypeWriter* newTypeWriter = twFactory->Create(factoryIt);
            newTypeWriter->fWriter = this;
            newTypeWriter->RegisterSlots();
            fTypeWriters.push_back(newTypeWriter);
        }
*/
    }

    KTBasicROOTFileWriter::~KTBasicROOTFileWriter()
    {
/*
        while (! fTypeWriters.empty())
        {
            delete fTypeWriters.back();
            fTypeWriters.pop_back();
        }
*/
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTBasicROOTFileWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
        }

        // Command-line settings
        //SetFilename(fCLHandler->GetCommandLineValue< string >("broot-output-file", fTransformFlag));

        return true;
    }

    Bool_t KTBasicROOTFileWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            fFile = new TFile(fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Output file <" << fFilename << "> did not open!");
            return false;
        }
        return true;
    }

    void KTBasicROOTFileWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTBasicROOTFileWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }

} /* namespace Katydid */
