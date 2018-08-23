/**
 @file KTIterativeTrackClustering.cc
 @brief Contains KTIterativeTrackClustering
 @details Groups collinear tracks into one
 @author: C. Claessens
 @date: August 7, 2017
 */

#include "KTIterativeTrackClustering.hh"
#include "KTLogger.hh"

#include <vector>
#include <cmath>

#ifndef NDEBUG
#include <sstream>
#endif



namespace Katydid
{
    KTLOGGER(itclog, "KTIterativeTrackClustering");

    KT_REGISTER_PROCESSOR(KTIterativeTrackClustering, "iterative-track-clustering");

    KTIterativeTrackClustering::KTIterativeTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fTimeGapTolerance(0.005),
            fFrequencyAcceptance(185000.0),
            fMaxTrackWidth(50000.0),
            fLargeMaxTrackWidth(250000.0),
            fCompTracks(),
            fNewTracks(),
            fNewSeqLineCands(),
            fCompSeqLineCands(),
            fNTracks(0),
            fTrackSignal("track", this),
            fSeqLineCandSignal("seq-cand", this),
            fDoneSignal("clustering-done", this),
            fTakeTrackSlot("track", this, &KTIterativeTrackClustering::TakeTrack),
            fTakeSeqLineCandSlot("seq-cand", this, &KTIterativeTrackClustering::TakeSeqLineCandidate)
    {
        RegisterSlot("do-clustering", this, &KTIterativeTrackClustering::DoClusteringSlot);
    }

    KTIterativeTrackClustering::~KTIterativeTrackClustering()
    {
    }

    bool KTIterativeTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetTimeGapTolerance(node->get_value("time-gap-tolerance", GetTimeGapTolerance()));
        SetFrequencyAcceptance(node->get_value("frequency-acceptance", GetFrequencyAcceptance()));

        if (node->has("max-track-width"))
        {
            SetMaxTrackWidth(node->get_value<double>("max-track-width"));
        }
        if (node->has("large-max-track-width"))
        {
            SetLargeMaxTrackWidth(node->get_value<double>("large-max-track-width"));
        }
        return true;
    }

    bool KTIterativeTrackClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;


        KTDEBUG(itclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data
        fCompTracks.push_back(track);

        return true;
    }
    bool KTIterativeTrackClustering::TakeSeqLineCandidate(KTSequentialLineData& SeqLineCand)
    {

        KTDEBUG(itclog, "Taking SeqLine candidate: (" << SeqLineCand.GetStartTimeInRunC() << ", " << SeqLineCand.GetStartFrequency() << ", " << SeqLineCand.GetEndTimeInRunC() << ", " << SeqLineCand.GetEndFrequency() << ")");

        // copy the full track data
        fCompSeqLineCands.push_back(SeqLineCand);

        return true;
    }
    void KTIterativeTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(itclog, "An error occurred while running the iterative track clustering");
        }
        return;
    }

    bool KTIterativeTrackClustering::Run()
    {
        if (fCompTracks.size() != 0 )
        {
            KTINFO( itclog, "Clustering procTracks");
            return DoCandidateClustering(fCompTracks, fNewTracks);
        }
        else
        {
            KTINFO( itclog, "Clustering SeqLine Candidates");
            return DoCandidateClustering(fCompSeqLineCands, fNewSeqLineCands);
        }
    }

    const void KTIterativeTrackClustering::CombineCandidates(const KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
    {
        KTDEBUG(itclog, "Matching candidates are: "<< oldTrack.GetTrackID()<<" - "<<newTrack.GetTrackID());

        if (oldTrack.GetStartTimeInRunC() < newTrack.GetStartTimeInRunC())
        {
            newTrack.SetStartTimeInRunC( oldTrack.GetStartTimeInRunC());
            newTrack.SetStartTimeInAcq( oldTrack.GetStartTimeInAcq());
            newTrack.SetStartFrequency( oldTrack.GetStartFrequency());
            newTrack.SetStartTimeInRunCSigma( oldTrack.GetStartTimeInRunCSigma());
            newTrack.SetStartFrequencySigma( oldTrack.GetStartFrequencySigma());
        }
        if (oldTrack.GetEndTimeInRunC() > newTrack.GetEndTimeInRunC())
        {
            newTrack.SetEndTimeInRunC( oldTrack.GetEndTimeInRunC());
            newTrack.SetEndFrequency( oldTrack.GetEndFrequency());
            newTrack.SetEndTimeInRunCSigma( oldTrack.GetEndTimeInRunCSigma());
            newTrack.SetEndFrequencySigma( oldTrack.GetEndFrequencySigma());
        }
        newTrack.SetSlope( (newTrack.GetEndFrequency() - newTrack.GetStartFrequency())/(newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC()));

        newTrack.SetNTrackBins( newTrack.GetNTrackBins() + oldTrack.GetNTrackBins() );
        newTrack.SetTotalTrackSNR( newTrack.GetTotalTrackSNR() + oldTrack.GetTotalTrackSNR() );
        newTrack.SetMaxTrackSNR( std::max( newTrack.GetMaxTrackSNR(), oldTrack.GetMaxTrackSNR() ) );
        newTrack.SetTotalTrackNUP( newTrack.GetTotalTrackNUP() + oldTrack.GetTotalTrackNUP() );
        newTrack.SetMaxTrackNUP( std::max( newTrack.GetMaxTrackNUP(), oldTrack.GetMaxTrackNUP() ) );
        newTrack.SetTotalWideTrackSNR( newTrack.GetTotalWideTrackSNR() + oldTrack.GetTotalWideTrackSNR() );
        newTrack.SetTotalWideTrackNUP( newTrack.GetTotalWideTrackNUP() + oldTrack.GetTotalWideTrackNUP() );
        newTrack.SetTotalPower( newTrack.GetTotalPower() + oldTrack.GetTotalPower());

        newTrack.SetTotalPowerSigma( sqrt(newTrack.GetTotalPowerSigma()*newTrack.GetTotalPowerSigma() + oldTrack.GetTotalPowerSigma()*oldTrack.GetTotalPowerSigma()) );
        //newTrack.SetTimeLength( newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC());
        newTrack.SetTimeLengthSigma( sqrt(newTrack.GetTimeLengthSigma()*newTrack.GetTimeLengthSigma() + oldTrack.GetTimeLengthSigma()*oldTrack.GetTimeLengthSigma()) );
        //newTrack.SetFrequencyWidth( newTrack.GetEndFrequency() - newTrack.GetStartFrequency() );
        newTrack.SetFrequencyWidthSigma( sqrt(newTrack.GetFrequencyWidthSigma()*newTrack.GetFrequencyWidthSigma() + oldTrack.GetFrequencyWidthSigma()*oldTrack.GetFrequencyWidthSigma()) );
        newTrack.SetSlopeSigma( sqrt(newTrack.GetSlopeSigma()*newTrack.GetSlopeSigma() + oldTrack.GetSlopeSigma()*oldTrack.GetSlopeSigma()));
        newTrack.SetInterceptSigma( sqrt(newTrack.GetInterceptSigma()*newTrack.GetInterceptSigma() + oldTrack.GetInterceptSigma()*oldTrack.GetInterceptSigma()));
    }
    const void KTIterativeTrackClustering::CombineCandidates(const KTSequentialLineData& oldSeqLineCand, KTSequentialLineData& newSeqLineCand)
    {
        KTDEBUG(itclog, "Matching candidates are: "<< oldSeqLineCand.GetCandidateID()<<" - "<<newSeqLineCand.GetCandidateID());

        if (oldSeqLineCand.GetStartTimeInRunC() < newSeqLineCand.GetStartTimeInRunC())
        {
            newSeqLineCand.SetStartTimeInRunC( oldSeqLineCand.GetStartTimeInRunC() );
            newSeqLineCand.SetStartTimeInAcq( oldSeqLineCand.GetStartTimeInAcq() );
            newSeqLineCand.SetStartFrequency( oldSeqLineCand.GetStartFrequency() );
            newSeqLineCand.SetComponent( oldSeqLineCand.GetComponent() );
            newSeqLineCand.SetAcquisitionID( oldSeqLineCand.GetAcquisitionID() );
        }
        if (oldSeqLineCand.GetEndTimeInRunC() > newSeqLineCand.GetEndTimeInRunC())
        {
            newSeqLineCand.SetEndTimeInRunC( oldSeqLineCand.GetEndTimeInRunC());
            newSeqLineCand.SetEndFrequency( oldSeqLineCand.GetEndFrequency());
        }
        newSeqLineCand.SetSlope( (newSeqLineCand.GetEndFrequency() - newSeqLineCand.GetStartFrequency())/(newSeqLineCand.GetEndTimeInRunC() - newSeqLineCand.GetStartTimeInRunC()) );

        KTDiscriminatedPoints points = oldSeqLineCand.GetPoints();
        for(KTDiscriminatedPoints::const_iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
        {
            //KTDEBUG( itclog, "Adding points from oldSeqLineCand to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
            newSeqLineCand.AddPoint(*pointIt);
        }
    }

    void KTIterativeTrackClustering::EmitCandidates(std::vector<KTProcessedTrackData>& compCands)
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<compCands.size());
        KTINFO(itclog, "Clustering done.");

        std::vector<KTProcessedTrackData>::iterator trackIt = compCands.begin();

        while(trackIt!=compCands.end())
        {
            // Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
            newTrack.SetComponent( trackIt->GetComponent() );
            newTrack.SetAcquisitionID( trackIt->GetAcquisitionID() );
            newTrack.SetTrackID( fNTracks );
            fNTracks++;

            newTrack.SetStartTimeInRunC( trackIt->GetStartTimeInRunC() );
            newTrack.SetStartTimeInRunCSigma( trackIt->GetStartTimeInRunCSigma() );
            newTrack.SetEndTimeInRunC( trackIt->GetEndTimeInRunC() );
            newTrack.SetEndTimeInRunCSigma( trackIt->GetEndTimeInRunCSigma() );
            newTrack.SetStartTimeInAcq( trackIt->GetStartTimeInAcq() );
            newTrack.SetTimeLength( trackIt->GetTimeLength() );
            newTrack.SetTimeLengthSigma( trackIt->GetTimeLengthSigma() );
            newTrack.SetStartFrequency( trackIt->GetStartFrequency() );
            newTrack.SetStartFrequency( trackIt->GetStartFrequencySigma() );
            newTrack.SetEndFrequency( trackIt->GetEndFrequency() );
            newTrack.SetEndFrequencySigma( trackIt->GetEndFrequencySigma() );
            newTrack.SetSlope( trackIt->GetSlope() );
            newTrack.SetSlopeSigma( trackIt->GetSlopeSigma() );
            newTrack.SetTotalPower( trackIt->GetTotalPower() );
            newTrack.SetTotalPowerSigma( trackIt->GetTotalPowerSigma() );
            newTrack.SetFrequencyWidth( trackIt->GetFrequencyWidth() );
            newTrack.SetFrequencyWidthSigma( trackIt->GetFrequencyWidthSigma() );
            newTrack.SetIntercept( trackIt->GetFrequencyWidth() );
            newTrack.SetInterceptSigma( trackIt->GetInterceptSigma() );

            newTrack.SetNTrackBins( trackIt->GetNTrackBins() );
            newTrack.SetTotalTrackSNR( trackIt->GetTotalTrackSNR() );
            newTrack.SetMaxTrackSNR( trackIt->GetMaxTrackSNR() );
            newTrack.SetTotalTrackNUP( trackIt->GetTotalTrackNUP() );
            newTrack.SetMaxTrackNUP( trackIt->GetMaxTrackNUP() );
            newTrack.SetTotalWideTrackSNR( trackIt->GetTotalWideTrackSNR() );
            newTrack.SetTotalWideTrackNUP( trackIt->GetTotalWideTrackNUP() );


            // Process & emit new track

            KTINFO(itclog, "Now processing tracksCandidates");
            ProcessNewTrack( newTrack );

            //KTDEBUG(itclog, "Emitting track signal");
            fCandidates.insert( data );
            fTrackSignal( data );

            trackIt = compCands.erase(trackIt);
        }
    }
    void KTIterativeTrackClustering::EmitCandidates(std::vector<KTSequentialLineData>& compCands)
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<compCands.size());
        KTINFO(itclog, "Clustering done.");


        std::vector<KTSequentialLineData>::iterator candIt = compCands.begin();

        while( candIt!=compCands.end() )
        {
            // Set up new data object
            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTSequentialLineData& newSeqLineCand = data->Of< KTSequentialLineData >();
            newSeqLineCand.SetComponent( candIt->GetComponent() );
            newSeqLineCand.SetAcquisitionID( candIt->GetAcquisitionID() );
            newSeqLineCand.SetCandidateID( fNTracks );
            fNTracks++;

            newSeqLineCand.SetSlope(candIt->GetSlope());

            KTDiscriminatedPoints& points = candIt->GetPoints();
            for(KTDiscriminatedPoints::const_iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
            {
                //KTDEBUG( itclog, "Adding points to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
                newSeqLineCand.AddPoint( *pointIt );
            }
            newSeqLineCand.CalculateTotalPower();
            newSeqLineCand.CalculateTotalSNR();
            newSeqLineCand.CalculateTotalNUP();

            fCandidates.insert( data );
            fSeqLineCandSignal( data );

            candIt = compCands.erase(candIt);
        }
    }
    const void KTIterativeTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }


} /* namespace Katydid */
