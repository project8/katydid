/*
 * KTPaperShredder.cc
 *
 *  Created on: Aug 15, 2017
 *      Author: ezayas
 */

#include "KTPaperShredder.hh"

#include "KTProcessedTrackData.hh"
#include "KTEggHeader.hh"


namespace Katydid
{
    KTLOGGER(sdlog, "KTPaperShredder");

    KT_REGISTER_PROCESSOR(KTPaperShredder, "paper-shredder");

    KTPaperShredder::KTPaperShredder(const std::string& name) :
            KTProcessor(name),
            fStripeSize(0.01),
            fStripeStride(0.01),
            fNSlicesSize(1),
            fNSlicesStride(1),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fAcquisitionTime(0.01),
            fStripeSignal("stripe", this),
            fHeaderSlot("header", this, &KTPaperShredder::MakeStripes, &fStripeSignal)
    {
    }

    KTPaperShredder::~KTPaperShredder()
    {
    }

    bool KTPaperShredder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("n-slices-size"))
        {
            SetNSlicesSize(node->get_value< unsigned >("n-slices-size"));
        }
        if (node->has("n-slices-stride"))
        {
            SetNSlicesStride(node->get_value< unsigned >("n-slices-stride"));
        }
        else
        {
            SetNSlicesStride(GetNSlicesSize());
        }

        if (node->has("stripe-size"))
        {
            SetStripeSize(node->get_value< double >("stripe-size"));
        }
        if (node->has("stripe-stride"))
        {
            SetStripeStride(node->get_value< double >("stripe-stride"));
        }
        else
        {
            SetStripeStride(GetStripeSize());
        }

        if( node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }

        if( node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        return true;
    }

    bool KTPaperShredder::MakeStripes( KTEggHeader& header )
    {
        fAcquisitionTime = header.GetRunDuration();

        double t_start = 0.;
        while( t_start < fAcquisitionTime )
        {
            for( unsigned iComponent = 0; iComponent < header.GetNChannels(); ++iComponent )
            {
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& fakeTrack = data->Of< KTProcessedTrackData >();

                fakeTrack.SetComponent( iComponent );

                fakeTrack.SetStartTimeInRunC( t_start );
                fakeTrack.SetEndTimeInRunC( t_start + GetStripeSize() );

                fakeTrack.SetStartFrequency( GetMinFrequency() );
                fakeTrack.SetEndFrequency( GetMaxFrequency() );

                fStripeSignal( data );
            }

            t_start += GetStripeStride();
        }

        return true;
    }

} /* namespace Katydid */
