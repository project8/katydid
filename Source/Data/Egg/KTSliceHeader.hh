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

            UInt_t GetNComponents() const;

            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            UInt_t GetSliceSize() const;
            Double_t GetSliceLength() const;
            Double_t GetSampleRate() const;
            Double_t GetBinWidth() const;

            UInt_t GetStartRecordNumber() const;
            UInt_t GetStartSampleNumber() const;
            UInt_t GetEndRecordNumber() const;
            UInt_t GetEndSampleNumber() const;

            ClockType GetTimeStamp(UInt_t component = 0) const;
            AcqIdType GetAcquisitionID(UInt_t component = 0) const;
            RecIdType GetRecordID(UInt_t component = 0) const;

            KTSliceHeader& SetNComponents(UInt_t num);

            void SetTimeInRun(Double_t time);
            void SetSliceNumber(ULong64_t slice);

            void SetSliceSize(UInt_t size);
            void SetSliceLength(Double_t length);
            void SetSampleRate(Double_t sampleRate);
            void SetBinWidth(Double_t binWidth);
            void CalculateBinWidthAndSliceLength();

            void SetStartRecordNumber(UInt_t rec);
            void SetStartSampleNumber(UInt_t sample);
            void SetEndRecordNumber(UInt_t rec);
            void SetEndSampleNumber(UInt_t sample);

            void SetTimeStamp(ClockType timeStamp, UInt_t component = 0);
            void SetAcquisitionID(AcqIdType acqId, UInt_t component = 0);
            void SetRecordID(RecIdType recId, UInt_t component = 0);

        private:
            struct PerComponentData
            {
                ClockType fTimeStamp;
                AcqIdType fAcquisitionID;
                RecIdType fRecordID;
            };

            Double_t fTimeInRun; // in sec
            ULong64_t fSliceNumber;

            UInt_t fSliceSize; // number of bins
            Double_t fSliceLength; // in sec
            Double_t fSampleRate; // in Hz
            Double_t fBinWidth; // in sec

            UInt_t fStartRecordNumber; // record in the run in which the slice starts
            UInt_t fStartSampleNumber; // sample number in the start record
            UInt_t fEndRecordNumber; // record in the run in which the slice ends
            UInt_t fEndSampleNumber; // sample number in the end record

            std::vector< PerComponentData > fComponentData;

    };


    inline UInt_t KTSliceHeader::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline Double_t KTSliceHeader::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline ULong64_t KTSliceHeader::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline UInt_t KTSliceHeader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline Double_t KTSliceHeader::GetSliceLength() const
    {
        return fSliceLength;
    }

    inline Double_t KTSliceHeader::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline Double_t KTSliceHeader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline UInt_t KTSliceHeader::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline UInt_t KTSliceHeader::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline UInt_t KTSliceHeader::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline UInt_t KTSliceHeader::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }

    inline ClockType KTSliceHeader::GetTimeStamp(UInt_t component) const
    {
        return fComponentData[component].fTimeStamp;
    }

    inline AcqIdType KTSliceHeader::GetAcquisitionID(UInt_t component) const
    {
        return fComponentData[component].fAcquisitionID;
    }

    inline RecIdType KTSliceHeader::GetRecordID(UInt_t component) const
    {
        return fComponentData[component].fRecordID;
    }


    inline void KTSliceHeader::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTSliceHeader::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

    inline void KTSliceHeader::SetSliceSize(UInt_t size)
    {
        fSliceSize = size;
        return;
    }

    inline void KTSliceHeader::SetSliceLength(Double_t length)
    {
        fSliceLength = length;
        return;
    }

    inline void KTSliceHeader::SetSampleRate(Double_t sampleRate)
    {
        fSampleRate = sampleRate;
    }

    inline void KTSliceHeader::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
    }

    inline void KTSliceHeader::CalculateBinWidthAndSliceLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetSliceLength(Double_t(fSliceSize) * fBinWidth);
        return;
    }

    inline void KTSliceHeader::SetStartRecordNumber(UInt_t rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline void KTSliceHeader::SetStartSampleNumber(UInt_t sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTSliceHeader::SetEndRecordNumber(UInt_t rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTSliceHeader::SetEndSampleNumber(UInt_t sample)
    {
        fEndSampleNumber = sample;
        return;
    }

    inline KTSliceHeader& KTSliceHeader::SetNComponents(UInt_t num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline void KTSliceHeader::SetTimeStamp(ClockType timeStamp, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fTimeStamp = timeStamp;
        return;
    }

    inline void KTSliceHeader::SetAcquisitionID(AcqIdType acqId, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fAcquisitionID = acqId;
        return;
    }

    inline void KTSliceHeader::SetRecordID(RecIdType recId, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fRecordID = recId;
        return;
    }

} /* namespace Katydid */
#endif /* KTSLICEHEADER_HH_ */
