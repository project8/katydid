/*
 * KTROOTTreeTypeWriterEgg.hh
 *
 *  Created on: June 10, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTTREETYPEWRITEREGG_HH_
#define KTROOTTREETYPEWRITEREGG_HH_

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
        UInt_t fRawSliceSize; /// Number of bins per slice before any modification
        UInt_t fSliceSize; /// Number of bins per slice after any initial modification (e.g. by the DAC)
        UInt_t fSliceStride;
        UInt_t fRecordSize; /// Number of bins per Monarch record
        UInt_t fRunDuration;
        Double_t fAcquisitionRate; /// in Hz
        TString* fTimestamp;
        TString* fDescription;
        UInt_t fRunType;
        UInt_t fRunSource;
        UInt_t fFormatMode;
        UInt_t fDataTypeSize; /// in bytes
        UInt_t fBitDepth; /// in bits
        Double_t fVoltageMin; /// in V
        Double_t fVoltageRange; /// in V
    };


    class KTEggHeader;


    class KTROOTTreeTypeWriterEgg : public KTROOTTreeTypeWriter//, public KTTypeWriterAnalysis
    {
        public:
            KTROOTTreeTypeWriterEgg();
            virtual ~KTROOTTreeTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteEggHeader(KTDataPtr headerPtr);

        public:
            TTree* GetEggHeaderTree() const;

        private:
            bool SetupEggHeaderTree();

            TTree* fEggHeaderTree;

            TEggHeader fEggHeaderData;

    };

    inline TTree* KTROOTTreeTypeWriterEgg::GetEggHeaderTree() const
    {
        return fEggHeaderTree;
    }

} /* namespace Katydid */


#endif /* KTROOTTREETYPEWRITEREGG_HH_ */
