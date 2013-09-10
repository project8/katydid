/*
 * KTSliceInfo.hh
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#ifndef KTSLICEHEADER_HH_
#define KTSLICEHEADER_HH_

#include "KTData.hh"

#include "MonarchTypes.hpp"

#include <utility>
#include <vector>

namespace Katydid
{
    class KTSliceHeader : public KTExtensibleData< KTSliceHeader >
    {
        public:
            KTSliceHeader();
            KTSliceHeader(const KTSliceHeader& orig);
            virtual ~KTSliceHeader();

            KTSliceHeader& operator=(const KTSliceHeader& rhs);

            /// Copy the data in rhs only; do not copy any additional extensible data
            void CopySliceHeaderOnly(const KTSliceHeader& rhs);

            UInt_t GetNComponents() const;
            KTSliceHeader& SetNComponents(UInt_t num);

            // Slice information

            Bool_t GetIsNewAcquisition() const;
            void SetIsNewAcquisition(Bool_t flag);
            Double_t GetTimeInRun() const;
            void SetTimeInRun(Double_t time);
            ULong64_t GetSliceNumber() const;
            void SetSliceNumber(ULong64_t slice);

            UInt_t GetSliceSize() const;
            void SetSliceSize(UInt_t size);
            Double_t GetSliceLength() const;
            void SetSliceLength(Double_t length);
            Double_t GetNonOverlapFrac() const;
            void SetNonOverlapFrac(Double_t frac);
            Double_t GetSampleRate() const;
            void SetSampleRate(Double_t sampleRate);
            Double_t GetBinWidth() const;
            void SetBinWidth(Double_t binWidth);

            void CalculateBinWidthAndSliceLength();

            Double_t GetTimeInRunAtSample(UInt_t sample);


            // Record-related information

            UInt_t GetStartRecordNumber() const;
            void SetStartRecordNumber(UInt_t rec);

            UInt_t GetStartSampleNumber() const;
            void SetStartSampleNumber(UInt_t sample);

            void SetStartRecordAndSample(std::pair< UInt_t, UInt_t > rsPair);

            UInt_t GetEndRecordNumber() const;
            void SetEndRecordNumber(UInt_t rec);

            UInt_t GetEndSampleNumber() const;
            void SetEndSampleNumber(UInt_t sample);

            void SetEndRecordAndSample(std::pair< UInt_t, UInt_t > rsPair);

            UInt_t GetRecordSize() const;
            void SetRecordSize(UInt_t record);

            std::pair< UInt_t, UInt_t > GetRecordSamplePairAtSample(UInt_t sampleInSlice);


            // Per-Component Information

            TimeType GetTimeStamp(UInt_t component = 0) const;
            void SetTimeStamp(TimeType timeStamp, UInt_t component = 0);

            AcquisitionIdType GetAcquisitionID(UInt_t component = 0) const;
            void SetAcquisitionID(AcquisitionIdType acqId, UInt_t component = 0);

            RecordIdType GetRecordID(UInt_t component = 0) const;
            void SetRecordID(RecordIdType recId, UInt_t component = 0);

            TimeType GetTimeStampAtSample(UInt_t sample, UInt_t component = 0);

        private:
            struct PerComponentData
            {
                TimeType fTimeStamp; // in nsec
                AcquisitionIdType fAcquisitionID;
                RecordIdType fRecordID;
            };

            Double_t fTimeInRun; // in sec
            ULong64_t fSliceNumber;
            Bool_t fIsNewAcquisition;

            UInt_t fSliceSize; // number of bins
            Double_t fSliceLength; // in sec
            Double_t fNonOverlapFrac; // fraction of the slice for which there is no overlap with another slice
            Double_t fSampleRate; // in Hz
            Double_t fBinWidth; // in sec

            UInt_t fStartRecordNumber; // record in the run in which the slice starts
            UInt_t fStartSampleNumber; // sample number in the start record
            UInt_t fEndRecordNumber; // record in the run in which the slice ends
            UInt_t fEndSampleNumber; // sample number in the end record

            UInt_t fRecordSize; // number of bins in the records on the egg file

            std::vector< PerComponentData > fComponentData;

            // Some temporary storage members to avoid allocating new variables
            UInt_t fTemp1, fTemp2, fTemp3;

    };

    std::ostream& operator<<(std::ostream& out, const KTSliceHeader& hdr);


    inline UInt_t KTSliceHeader::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline KTSliceHeader& KTSliceHeader::SetNComponents(UInt_t num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline Bool_t KTSliceHeader::GetIsNewAcquisition() const
    {
        return fIsNewAcquisition;
    }

    inline void KTSliceHeader::SetIsNewAcquisition(Bool_t flag)
    {
        fIsNewAcquisition = flag;
        return;
    }

    inline Double_t KTSliceHeader::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline void KTSliceHeader::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline ULong64_t KTSliceHeader::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTSliceHeader::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

    inline UInt_t KTSliceHeader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTSliceHeader::SetSliceSize(UInt_t size)
    {
        fSliceSize = size;
        return;
    }

    inline Double_t KTSliceHeader::GetSliceLength() const
    {
        return fSliceLength;
    }

    inline void KTSliceHeader::SetSliceLength(Double_t length)
    {
        fSliceLength = length;
        return;
    }

    inline Double_t KTSliceHeader::GetNonOverlapFrac() const
    {
        return fNonOverlapFrac;
    }

    inline void KTSliceHeader::SetNonOverlapFrac(Double_t frac)
    {
        fNonOverlapFrac = frac;
        return;
    }

    inline Double_t KTSliceHeader::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline void KTSliceHeader::SetSampleRate(Double_t sampleRate)
    {
        fSampleRate = sampleRate;
        return;
    }

    inline Double_t KTSliceHeader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTSliceHeader::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

    inline void KTSliceHeader::CalculateBinWidthAndSliceLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetSliceLength(Double_t(fSliceSize) * fBinWidth);
        return;
    }

    inline Double_t KTSliceHeader::GetTimeInRunAtSample(UInt_t sample)
    {
        return fTimeInRun + fBinWidth * Double_t(sample);
    }


    inline UInt_t KTSliceHeader::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline void KTSliceHeader::SetStartRecordNumber(UInt_t rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline UInt_t KTSliceHeader::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline void KTSliceHeader::SetStartSampleNumber(UInt_t sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTSliceHeader::SetStartRecordAndSample(std::pair< UInt_t, UInt_t > rsPair)
    {
        fStartRecordNumber = rsPair.first;
        fStartSampleNumber = rsPair.second;
        return;
    }

    inline UInt_t KTSliceHeader::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline void KTSliceHeader::SetEndRecordNumber(UInt_t rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTSliceHeader::SetEndRecordAndSample(std::pair< UInt_t, UInt_t > rsPair)
    {
        fEndRecordNumber = rsPair.first;
        fEndSampleNumber = rsPair.second;
        return;
    }

    inline UInt_t KTSliceHeader::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }

    inline void KTSliceHeader::SetEndSampleNumber(UInt_t sample)
    {
        fEndSampleNumber = sample;
        return;
    }

    inline UInt_t KTSliceHeader::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTSliceHeader::SetRecordSize(UInt_t size)
    {
        fRecordSize = size;
        return;
    }

    inline std::pair< UInt_t, UInt_t > KTSliceHeader::GetRecordSamplePairAtSample(UInt_t sampleInSlice)
    {
        // NOTE 1: doing these both at once, we can take advantage of compiler optimization,
        // which will most likely only perform one division operation
        // NOTE 2: this function does not depend on the slice size; this feature is useful
        // to be able to calculate record offsets without worrying about which slice, exactly, this header pertains to.
        // It's in use (as of this writing) in KTWignerVille.
        fTemp1 = fStartSampleNumber + sampleInSlice; // sample in the record + some number of record lengths
        fTemp2 = fStartRecordNumber + fTemp1 / fRecordSize; // record number
        fTemp3 = fTemp1 % fRecordSize; // sample in the record
        return std::make_pair (fTemp2, fTemp3);
    }

    inline TimeType KTSliceHeader::GetTimeStamp(UInt_t component) const
    {
        return fComponentData[component].fTimeStamp;
    }

    inline void KTSliceHeader::SetTimeStamp(TimeType timeStamp, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fTimeStamp = timeStamp;
        return;
    }

    inline AcquisitionIdType KTSliceHeader::GetAcquisitionID(UInt_t component) const
    {
        return fComponentData[component].fAcquisitionID;
    }

    inline void KTSliceHeader::SetAcquisitionID(AcquisitionIdType acqId, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fAcquisitionID = acqId;
        return;
    }

    inline RecordIdType KTSliceHeader::GetRecordID(UInt_t component) const
    {
        return fComponentData[component].fRecordID;
    }

    inline void KTSliceHeader::SetRecordID(RecordIdType recId, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fRecordID = recId;
        return;
    }

    inline TimeType KTSliceHeader::GetTimeStampAtSample(UInt_t sample, UInt_t component)
    {
        return fComponentData[component].fTimeStamp + (TimeType)sample * (TimeType)(fBinWidth * 1.e9); // have to convert bin width to ns
    }


} /* namespace Katydid */
#endif /* KTSLICEHEADER_HH_ */
