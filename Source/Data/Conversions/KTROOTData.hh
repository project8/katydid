/*
 * KTROOTData.hh
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTDATA_HH_
#define KTROOTDATA_HH_

#include "TObject.h"

namespace Katydid
{
    class KTProcessedTrackData;

    class TProcessedTrackData : public TObject
    {
        private:
            UInt_t fComponent;
            Bool_t fIsCut;
            Double_t fStartTimeInRunC;
            Double_t fEndTimeInRunC;
            Double_t fTimeLength;
            Double_t fStartFrequency;
            Double_t fEndFrequency;
            Double_t fFrequencyWidth;
            Double_t fSlope;
            Double_t fIntercept;
            Double_t fTotalPower;
            Double_t fStartTimeInRunCSigma;
            Double_t fEndTimeInRunCSigma;
            Double_t fTimeLengthSigma;
            Double_t fStartFrequencySigma;
            Double_t fEndFrequencySigma;
            Double_t fFrequencyWidthSigma;
            Double_t fSlopeSigma;
            Double_t fInterceptSigma;
            Double_t fTotalPowerSigma;

        public:
            TProcessedTrackData();
            TProcessedTrackData(const KTProcessedTrackData& data);
            TProcessedTrackData(const TProcessedTrackData& orig);
            virtual ~TProcessedTrackData();
            TProcessedTrackData& operator=(const TProcessedTrackData& rhs);

            void Load(const KTProcessedTrackData& data);
            void Unload(KTProcessedTrackData& data) const;

            ClassDef(TProcessedTrackData, 1);
    };

}


#endif /* KTROOTDATA_HH_ */
