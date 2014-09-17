/*
 * KTROOTData.hh
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTDATA_HH_
#define KTROOTDATA_HH_

#include "KTMemberVariable.hh"

#include "TObject.h"

class TClonesArray;

namespace Katydid
{
    //***********************
    // TProcessedTrackData
    //***********************

    class KTProcessedTrackData;

    class TProcessedTrackData : public TObject
    {
            MEMBERVARIABLE(UInt_t, Component);
            MEMBERVARIABLE(UInt_t, TrackID);

            MEMBERVARIABLE(Bool_t, IsCut);

            MEMBERVARIABLE(Double_t, StartTimeInRunC);
            MEMBERVARIABLE(Double_t, EndTimeInRunC);
            MEMBERVARIABLE(Double_t, TimeLength);
            MEMBERVARIABLE(Double_t, StartFrequency);
            MEMBERVARIABLE(Double_t, EndFrequency);
            MEMBERVARIABLE(Double_t, FrequencyWidth);
            MEMBERVARIABLE(Double_t, Slope);
            MEMBERVARIABLE(Double_t, Intercept);
            MEMBERVARIABLE(Double_t, TotalPower);

            MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
            MEMBERVARIABLE(Double_t, TimeLengthSigma);
            MEMBERVARIABLE(Double_t, StartFrequencySigma);
            MEMBERVARIABLE(Double_t, EndFrequencySigma);
            MEMBERVARIABLE(Double_t, FrequencyWidthSigma);
            MEMBERVARIABLE(Double_t, SlopeSigma);
            MEMBERVARIABLE(Double_t, InterceptSigma);
            MEMBERVARIABLE(Double_t, TotalPowerSigma);

        public:
            TProcessedTrackData();
            TProcessedTrackData(const KTProcessedTrackData& data);
            TProcessedTrackData(const TProcessedTrackData& orig);
            virtual ~TProcessedTrackData();
            TObject* Clone(const char* newname="");
            TProcessedTrackData& operator=(const TProcessedTrackData& rhs);

            void Load(const KTProcessedTrackData& data);
            void Unload(KTProcessedTrackData& data) const;

            ClassDef(TProcessedTrackData, 1);
    };

    //************************
    // TMultiTrackEventData
    //************************

    class KTMultiTrackEventData;

    class TMultiTrackEventData : public TObject
    {
        MEMBERVARIABLE(UInt_t, Component);
        MEMBERVARIABLE(UInt_t, EventID);

        MEMBERVARIABLE(Double_t, StartTimeInRunC);
        MEMBERVARIABLE(Double_t, EndTimeInRunC);
        MEMBERVARIABLE(Double_t, TimeLength);
        MEMBERVARIABLE(Double_t, StartFrequency);
        MEMBERVARIABLE(Double_t, EndFrequency);
        MEMBERVARIABLE(Double_t, MinimumFrequency);
        MEMBERVARIABLE(Double_t, MaximumFrequency);
        MEMBERVARIABLE(Double_t, FrequencyWidth);

        MEMBERVARIABLE(Double_t, StartTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, EndTimeInRunCSigma);
        MEMBERVARIABLE(Double_t, TimeLengthSigma);
        MEMBERVARIABLE(Double_t, StartFrequencySigma);
        MEMBERVARIABLE(Double_t, EndFrequencySigma);
        MEMBERVARIABLE(Double_t, FrequencyWidthSigma);

        MEMBERVARIABLE(Double_t, FirstTrackTimeLength);
        MEMBERVARIABLE(Double_t, FirstTrackFrequencyWidth);
        MEMBERVARIABLE(Double_t, FirstTrackSlope);
        MEMBERVARIABLE(Double_t, FirstTrackIntercept);
        MEMBERVARIABLE(Double_t, FirstTrackTotalPower);

        public:
            TClonesArray* GetTracks() {return fTracks;}

        private:
            TClonesArray* fTracks; //->

        public:
            TMultiTrackEventData();
            TMultiTrackEventData(const KTMultiTrackEventData& data);
            TMultiTrackEventData(const TMultiTrackEventData& orig);
            virtual ~TMultiTrackEventData();
            TObject* Clone(const char* newname="");
            TMultiTrackEventData& operator=(const TMultiTrackEventData& rhs);

            void Load(const KTMultiTrackEventData& data);
            void Unload(KTMultiTrackEventData& data) const;

            ClassDef(TMultiTrackEventData, 1);
    };


}


#endif /* KTROOTDATA_HH_ */
