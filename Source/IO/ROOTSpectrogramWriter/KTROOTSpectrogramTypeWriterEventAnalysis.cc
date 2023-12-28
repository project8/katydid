/*
 * KTROOTSpectrogramTypeWriterEventAnalysis.cc
 *
 *  Created on: Aug 8, 2017
 *      Author: nsoblath
 */

#include "KTROOTSpectrogramTypeWriterEventAnalysis.hh"

#include "KTProcessedTrackData.hh"

#include "TLine.h"
#include "TOrdCollection.h"

#include "logger.hh"

namespace Katydid
{
    LOGGER(publog, "KTROOTSpectrogramTypeWriterEventAnalysis");

    static Nymph::KTTIRegistrar< KTROOTSpectrogramTypeWriter, KTROOTSpectrogramTypeWriterEventAnalysis > sRSTWEARegistrar;

    KTROOTSpectrogramTypeWriterEventAnalysis::KTROOTSpectrogramTypeWriterEventAnalysis() :
            KTROOTSpectrogramTypeWriter(),
            //KTTypeWriterEventAnalysis()
            fLineCollection(NULL)
    {
    }

    KTROOTSpectrogramTypeWriterEventAnalysis::~KTROOTSpectrogramTypeWriterEventAnalysis()
    {
        delete fLineCollection;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::ClearLines()
    {
        delete fLineCollection;
        fLineCollection = NULL;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::OutputSpectrograms()
    {
        if (! fWriter->OpenAndVerifyFile()) return;

        LDEBUG("calling output lines")
        OutputLines();

        return;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::OutputLines()
    {
        LDEBUG(publog, "Outputing lines");
        fWriter->GetFile()->WriteTObject(fLineCollection, "AllLines", "SingleKey");
        fLineCollection = NULL;
        return;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("proc-track", this, &KTROOTSpectrogramTypeWriterEventAnalysis::AddProcessedTrackData);
        return;
    }


    //*********************
    // Processed Track Data
    //*********************

    void KTROOTSpectrogramTypeWriterEventAnalysis::AddProcessedTrackData(Nymph::KTDataPtr data)
    {
        KTProcessedTrackData thisLine = data->Of< KTProcessedTrackData >();

//        if( thisLine.GetIsCut() )
//        {
//            LINFO("The track is cut! Will not add it to the Line Collection");
//            return;
//        }

        if (fLineCollection == NULL)
        {
            fLineCollection = new TOrdCollection();
        }
        TLine* rootLine = new TLine(thisLine.GetStartTimeInRunC(), thisLine.GetStartFrequency(), thisLine.GetEndTimeInRunC(), thisLine.GetEndFrequency());
        fLineCollection->Add(rootLine);

        return;
    }

} /* namespace Katydid */
