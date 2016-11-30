/*
 * KTROOTTreeTypeWriterTime.hh
 *
 *  Created on: June 10, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITERTIME_HH_
#define KTROOTTREETYPEWRITERTIME_HH_

#include "KTROOTTreeWriter.hh"

#include "KTData.hh"

#include "Rtypes.h"
#include "TString.h"

class TTree;

namespace Katydid
{
    
    struct TEggHeader
    {
        TString* fFilename;
        UInt_t fAcquisitionMode;
        UInt_t fNChannels;
        UInt_t fRunDuration; /// in ms
        Double_t fAcquisitionRate; /// in Hz
        Double_t fCenterFrequency;
        Double_t fMaximumFrequency;
        Double_t fMinimumFrequency;
        TString* fTimestamp;
        TString* fDescription;
    };

    struct TChannelHeader
    {
            UInt_t fNumber;
            TString* fSource;
            UInt_t fRawSliceSize; /// Number of bins per slice before any modification
            UInt_t fSliceSize; /// Number of bins per slice after any initial modification (e.g. by the DAC)
            UInt_t fSliceStride;
            UInt_t fRecordSize; /// Number of bins per Monarch record
            UInt_t fSampleSize; /// Number of elements in each sample
            UInt_t fDataTypeSize; /// in bytes
            UInt_t fBitDepth; /// in bits
            Double_t fVoltageOffset; /// in V
            Double_t fVoltageRange; /// in V
            Double_t fDACGain;
    };


    class KTEggHeader;


    class KTROOTTreeTypeWriterTime : public KTROOTTreeTypeWriter//, public KTTypeWriterAnalysis
    {
        public:
            KTROOTTreeTypeWriterTime();
            virtual ~KTROOTTreeTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteEggHeader(Nymph::KTDataPtr headerPtr);

        public:
            TTree* GetEggHeaderTree() const;
            TTree* GetChannelHeaderTree() const;

        private:
            bool SetupEggHeaderTree();
            bool SetupChannelHeaderTree();

            TTree* fEggHeaderTree;
            TTree* fChannelHeaderTree;

            TEggHeader fEggHeaderData;
            TChannelHeader fChannelHeaderData;

    };

    inline TTree* KTROOTTreeTypeWriterTime::GetEggHeaderTree() const
    {
        return fEggHeaderTree;
    }

    inline TTree* KTROOTTreeTypeWriterTime::GetChannelHeaderTree() const
    {
        return fChannelHeaderTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITERTIME_HH_ */
