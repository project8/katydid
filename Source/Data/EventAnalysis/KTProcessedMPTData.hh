/*
 * KTProcessedMPTData.hh
 *
 *  Created on: Mar 14, 2018
 *      Author: ezayas
 */

#ifndef KTPROCESSEDMPTDATA_HH_
#define KTPROCESSEDMPTDATA_HH_

#include "KTData.hh"
#include "KTProcessedTrackData.hh"
#include "KTMemberVariable.hh"

namespace Katydid
{

    class KTProcessedMPTData : public Nymph::KTExtensibleData< KTProcessedMPTData >
    {

        private:
            unsigned fComponent;
            KTProcessedTrackData fMainTrack;
            double fAxialFrequency;

        public:
            KTProcessedMPTData();
            KTProcessedMPTData(const KTProcessedMPTData& orig);
            virtual ~KTProcessedMPTData();

            KTProcessedMPTData& operator=(const KTProcessedMPTData& rhs);

        public:

            unsigned GetComponent() const;
            void SetComponent(unsigned component);

            KTProcessedTrackData GetMainTrack() const;
            void SetMainTrack( const KTProcessedTrackData& track );

            double GetAxialFrequency() const;
            void SetAxialFrequency( double f );

        public:
            static const std::string sName;
    };

    inline unsigned KTProcessedMPTData::GetComponent() const
    {
        return fComponent;
    }

    inline void KTProcessedMPTData::SetComponent(unsigned component)
    {
        fComponent = component;
    }

    inline KTProcessedTrackData KTProcessedMPTData::GetMainTrack() const
    {
        return fMainTrack;
    }

    inline void KTProcessedMPTData::SetMainTrack( const KTProcessedTrackData& track )
    {
        fMainTrack = track;
    }

    inline double KTProcessedMPTData::GetAxialFrequency() const
    {
        return fAxialFrequency;
    }

    inline void KTProcessedMPTData::SetAxialFrequency(double f)
    {
        fAxialFrequency = f;
    }

}
#endif

