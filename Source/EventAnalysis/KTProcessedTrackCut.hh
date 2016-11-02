/*
 * KTProcessedTrackCut.hh
 *
 *  Created on: Nov 2, 2016
 *      Author: ezayas
 */

#ifndef KTPROCESSEDTRACKCUT_HH_
#define KTPROCESSEDTRACKCUT_HH_

#include "KTCut.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTProcessedTrackCut
     @author E. Zayas

     @brief Cuts on KTProcessedTrackData using the member variable fIsCut

     @details
     KTProcessedTrackData objects must have fIsCut == false to pass the cut

     Configuration name: "track-cut"
    */

    class KTProcessedTrackCut : public Nymph::KTCut
    {

     public:
         struct Result : Nymph::KTCutResult
         {
             static const std::string sName;
         };

     public:
         KTProcessedTrackCut(const std::string& name = "track-cut");
         ~KTProcessedTrackCut();

         bool Configure(const scarab::param_node* node);

     public:
         bool PerformCut(Nymph::KTData& data, KTProcessedTrackData& trackData);
         bool Apply(Nymph::KTDataPtr dataPtr);

    };
} // namespace Katydid

#endif /* KTPROCESSEDTRACKCUT_HH_ */
