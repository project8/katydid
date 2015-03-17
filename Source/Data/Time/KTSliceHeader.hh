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

            unsigned GetNComponents() const;
            KTSliceHeader& SetNComponents(unsigned num);

            // Slice information

            bool GetIsNewAcquisition() const;
            void SetIsNewAcquisition(bool flag);
            double GetTimeInRun() const;
            void SetTimeInRun(double time);
            double GetTimeInAcq() const;
            void SetTimeInAcq(double time);
            uint64_t GetSliceNumber() const;
            void SetSliceNumber(uint64_t slice);
            unsigned GetNSlicesIncluded() const;
            void SetNSlicesIncluded(unsigned nSlices);

            unsigned GetRawSliceSize() const;
            void SetRawSliceSize(unsigned size);
            unsigned GetSliceSize() const;
            void SetSliceSize(unsigned size);
            double GetSliceLength() const;
            void SetSliceLength(double length);
            double GetNonOverlapFrac() const;
            void SetNonOverlapFrac(double frac);
            double GetSampleRate() const;
            void SetSampleRate(double sampleRate);
            double GetBinWidth() const;
            void SetBinWidth(double binWidth);

            void CalculateBinWidthAndSliceLength();

            double GetTimeInRunAtSample(unsigned sample);


            // Record-related information

            unsigned GetStartRecordNumber() const;
            void SetStartRecordNumber(unsigned rec);

            unsigned GetStartSampleNumber() const;
            void SetStartSampleNumber(unsigned sample);

            void SetStartRecordAndSample(std::pair< unsigned, unsigned > rsPair);

            unsigned GetEndRecordNumber() const;
            void SetEndRecordNumber(unsigned rec);

            unsigned GetEndSampleNumber() const;
            void SetEndSampleNumber(unsigned sample);

            void SetEndRecordAndSample(std::pair< unsigned, unsigned > rsPair);

            unsigned GetRecordSize() const;
            void SetRecordSize(unsigned record);

            std::pair< unsigned, unsigned > GetRecordSamplePairAtSample(unsigned sampleInSlice);


            // Per-Component Information

            monarch::TimeType GetTimeStamp(unsigned component = 0) const;
            void SetTimeStamp(monarch::TimeType timeStamp, unsigned component = 0);

            monarch::AcquisitionIdType GetAcquisitionID(unsigned component = 0) const;
            void SetAcquisitionID(monarch::AcquisitionIdType acqId, unsigned component = 0);

            monarch::RecordIdType GetRecordID(unsigned component = 0) const;
            void SetRecordID(monarch::RecordIdType recId, unsigned component = 0);

            monarch::TimeType GetTimeStampAtSample(unsigned sample, unsigned component = 0);

        private:
            struct PerComponentData
            {
                monarch::TimeType fTimeStamp; // in nsec
                monarch::AcquisitionIdType fAcquisitionID;
                monarch::RecordIdType fRecordID;
            };

            double fTimeInRun; // in sec
            double fTimeInAcq; // in sec
            uint64_t fSliceNumber;
            unsigned fNSlicesIncluded; // for meta-slices
            bool fIsNewAcquisition;

            unsigned fRawSliceSize; // number of bins before any modification
            unsigned fSliceSize; // number of bins
            double fSliceLength; // in sec
            double fNonOverlapFrac; // fraction of the slice for which there is no overlap with another slice
            double fSampleRate; // in Hz
            double fBinWidth; // in sec

            unsigned fStartRecordNumber; // record in the run in which the slice starts
            unsigned fStartSampleNumber; // sample number in the start record
            unsigned fEndRecordNumber; // record in the run in which the slice ends
            unsigned fEndSampleNumber; // sample number in the end record

            unsigned fRecordSize; // number of bins in the records on the egg file

            std::vector< PerComponentData > fComponentData;

            // Some temporary storage members to avoid allocating new variables
            unsigned fTemp1, fTemp2, fTemp3;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTSliceHeader& hdr);


    inline unsigned KTSliceHeader::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline KTSliceHeader& KTSliceHeader::SetNComponents(unsigned num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline bool KTSliceHeader::GetIsNewAcquisition() const
    {
        return fIsNewAcquisition;
    }

    inline void KTSliceHeader::SetIsNewAcquisition(bool flag)
    {
        fIsNewAcquisition = flag;
        return;
    }

    inline double KTSliceHeader::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline void KTSliceHeader::SetTimeInRun(double tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline double KTSliceHeader::GetTimeInAcq() const
    {
        return fTimeInAcq;
    }

    inline void KTSliceHeader::SetTimeInAcq(double tiacq)
    {
        fTimeInAcq = tiacq;
        return;
    }

    inline uint64_t KTSliceHeader::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTSliceHeader::SetSliceNumber(uint64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

    inline unsigned KTSliceHeader::GetNSlicesIncluded() const
    {
        return fNSlicesIncluded;
    }
    inline void KTSliceHeader::SetNSlicesIncluded(unsigned nSlices)
    {
        fNSlicesIncluded = nSlices;
        return;
    }

    inline unsigned KTSliceHeader::GetRawSliceSize() const
    {
        return fRawSliceSize;
    }

    inline void KTSliceHeader::SetRawSliceSize(unsigned size)
    {
        fRawSliceSize = size;
        return;
    }

    inline unsigned KTSliceHeader::GetSliceSize() const
    {
        return fSliceSize;
    }

    inline void KTSliceHeader::SetSliceSize(unsigned size)
    {
        fSliceSize = size;
        return;
    }

    inline double KTSliceHeader::GetSliceLength() const
    {
        return fSliceLength;
    }

    inline void KTSliceHeader::SetSliceLength(double length)
    {
        fSliceLength = length;
        return;
    }

    inline double KTSliceHeader::GetNonOverlapFrac() const
    {
        return fNonOverlapFrac;
    }

    inline void KTSliceHeader::SetNonOverlapFrac(double frac)
    {
        fNonOverlapFrac = frac;
        return;
    }

    inline double KTSliceHeader::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline void KTSliceHeader::SetSampleRate(double sampleRate)
    {
        fSampleRate = sampleRate;
        return;
    }

    inline double KTSliceHeader::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTSliceHeader::SetBinWidth(double binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

    inline void KTSliceHeader::CalculateBinWidthAndSliceLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetSliceLength(double(fSliceSize) * fBinWidth);
        return;
    }

    inline double KTSliceHeader::GetTimeInRunAtSample(unsigned sample)
    {
        return fTimeInRun + fBinWidth * double(sample);
    }


    inline unsigned KTSliceHeader::GetStartRecordNumber() const
    {
        return fStartRecordNumber;
    }

    inline void KTSliceHeader::SetStartRecordNumber(unsigned rec)
    {
        fStartRecordNumber = rec;
        return;
    }

    inline unsigned KTSliceHeader::GetStartSampleNumber() const
    {
        return fStartSampleNumber;
    }

    inline void KTSliceHeader::SetStartSampleNumber(unsigned sample)
    {
        fStartSampleNumber = sample;
        return;
    }

    inline void KTSliceHeader::SetStartRecordAndSample(std::pair< unsigned, unsigned > rsPair)
    {
        fStartRecordNumber = rsPair.first;
        fStartSampleNumber = rsPair.second;
        return;
    }

    inline unsigned KTSliceHeader::GetEndRecordNumber() const
    {
        return fEndRecordNumber;
    }

    inline void KTSliceHeader::SetEndRecordNumber(unsigned rec)
    {
        fEndRecordNumber = rec;
        return;
    }

    inline void KTSliceHeader::SetEndRecordAndSample(std::pair< unsigned, unsigned > rsPair)
    {
        fEndRecordNumber = rsPair.first;
        fEndSampleNumber = rsPair.second;
        return;
    }

    inline unsigned KTSliceHeader::GetEndSampleNumber() const
    {
        return fEndSampleNumber;
    }

    inline void KTSliceHeader::SetEndSampleNumber(unsigned sample)
    {
        fEndSampleNumber = sample;
        return;
    }

    inline unsigned KTSliceHeader::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTSliceHeader::SetRecordSize(unsigned size)
    {
        fRecordSize = size;
        return;
    }

    inline std::pair< unsigned, unsigned > KTSliceHeader::GetRecordSamplePairAtSample(unsigned sampleInSlice)
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

    inline monarch::TimeType KTSliceHeader::GetTimeStamp(unsigned component) const
    {
        return fComponentData[component].fTimeStamp;
    }

    inline void KTSliceHeader::SetTimeStamp(monarch::TimeType timeStamp, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fTimeStamp = timeStamp;
        return;
    }

    inline monarch::AcquisitionIdType KTSliceHeader::GetAcquisitionID(unsigned component) const
    {
        return fComponentData[component].fAcquisitionID;
    }

    inline void KTSliceHeader::SetAcquisitionID(monarch::AcquisitionIdType acqId, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fAcquisitionID = acqId;
        return;
    }

    inline monarch::RecordIdType KTSliceHeader::GetRecordID(unsigned component) const
    {
        return fComponentData[component].fRecordID;
    }

    inline void KTSliceHeader::SetRecordID(monarch::RecordIdType recId, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fRecordID = recId;
        return;
    }

    inline monarch::TimeType KTSliceHeader::GetTimeStampAtSample(unsigned sample, unsigned component)
    {
        return fComponentData[component].fTimeStamp + (monarch::TimeType)sample * (monarch::TimeType)(fBinWidth * 1.e9); // have to convert bin width to ns
    }


} /* namespace Katydid */
#endif /* KTSLICEHEADER_HH_ */
