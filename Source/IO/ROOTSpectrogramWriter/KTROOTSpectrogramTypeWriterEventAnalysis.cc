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

//#include "KTLogger.hh"

using std::vector;

namespace Katydid
{
    //KTLOGGER(publog, "KTROOTSpectrogramTypeWriterEventAnalysis");

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

        KTDEBUG("calling output lines")
        OutputLines();

        return;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::OutputLines()
    {
        KTDEBUG("grab file and cd")
        fWriter->GetFile()->WriteTObject(fLineCollection, "AllLines", "SingleKey");
        fLineCollection = NULL;
        return;
    }

    void KTROOTSpectrogramTypeWriterEventAnalysis::RegisterSlots()
    {
        fWriter->RegisterSlot("proc-track", this, &KTROOTSpectrogramTypeWriterEventAnalysis::AddProcessedTrackData);
        fWriter->RegisterSlot("all-lines", this, &KTROOTSpectrogramTypeWriterEventAnalysis::TakeLine);
        return;
    }


    //*********************
    // Processed Track Data
    //*********************

    void KTROOTSpectrogramTypeWriterEventAnalysis::AddProcessedTrackData(Nymph::KTDataPtr data)
    {
        KTProcessedTrackData thisTrack = data->Of< KTProcessedTrackData >();

        if( !thisTrack.GetIsCut() )
        {
            fWriter->SetMinTime( thisTrack.GetStartTimeInRunC() - fWriter->GetBufferTime() );
            fWriter->SetMaxTime( thisTrack.GetEndTimeInRunC() + fWriter->GetBufferTime() );
            fWriter->SetMinFreq( thisTrack.GetStartFrequency() - fWriter->GetBufferFreq() );
            fWriter->SetMaxFreq( thisTrack.GetEndFrequency() + fWriter->GetBufferFreq() );
        }

        return;
    }


    void KTROOTSpectrogramTypeWriterEventAnalysis::TakeLine(Nymph::KTDataPtr data)
    {
        KTProcessedTrackData thisLine = data->Of< KTProcessedTrackData >();
        
//        if( thisLine.GetIsCut() )
//        {
//            KTINFO("The track is cut! Will not add it to the Line Collection");
//            return;
//        }

        if (fLineCollection == NULL)
        {
            fLineCollection = new TOrdCollection();
        }
        TLine* rootLine = new TLine(thisLine.GetStartTimeInRunC(), thisLine.GetStartFrequency(), thisLine.GetEndTimeInRunC(), thisLine.GetEndFrequency());
        fLineCollection->Add(rootLine);
    }

} /* namespace Katydid */
