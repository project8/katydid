/*
 * KTSliceInfo.hh
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#ifndef KTSLICEHEADER_HH_
#define KTSLICEHEADER_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

#include <inttypes.h>
#include <utility>
#include <vector>

namespace Katydid
{
    
    class KTSliceHeader : public Nymph::KTExtensibleData< KTSliceHeader >
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

            MEMBERVARIABLE(double, TimeInRun); // in sec
            MEMBERVARIABLE(double, TimeInAcq); // in sec
            MEMBERVARIABLE(uint64_t, SliceNumber);
            MEMBERVARIABLE(unsigned, NSlicesIncluded); // for meta-slices
            MEMBERVARIABLE(bool, IsNewAcquisition);

            MEMBERVARIABLE(unsigned, RawSliceSize); // number of bins before any modification
            MEMBERVARIABLE(unsigned, SliceSize); // number of bins
            MEMBERVARIABLE(double, SliceLength); // in sec
            MEMBERVARIABLE(double, NonOverlapFrac); // fraction of the slice for which there is no overlap with another slice
            MEMBERVARIABLE(double, SampleRate); // in Hz
            MEMBERVARIABLE(double, BinWidth); // in sec

            MEMBERVARIABLE(unsigned, StartRecordNumber); // record in the run in which the slice starts
            MEMBERVARIABLE(unsigned, StartSampleNumber); // sample number in the start record
            MEMBERVARIABLE(unsigned, EndRecordNumber); // record in the run in which the slice ends
            MEMBERVARIABLE(unsigned, EndSampleNumber); // sample number in the end record

            MEMBERVARIABLE(unsigned, RecordSize); // number of bins in the records on the egg file
            MEMBERVARIABLE(unsigned, FileNumber); //to identify the file being processed
            MEMBERVARIABLE(std::string, Filename); //to identify the file being processed


        public:
            // Slice information

            void CalculateBinWidthAndSliceLength();

            double GetTimeInRunAtSample(unsigned sample);


            // Record-related information

            void SetStartRecordAndSample(std::pair< unsigned, unsigned > rsPair);
            void SetEndRecordAndSample(std::pair< unsigned, unsigned > rsPair);
            std::pair< unsigned, unsigned > GetRecordSamplePairAtSample(unsigned sampleInSlice);

            // Per-Component Information
            uint64_t GetTimeStamp(unsigned component = 0) const;
            void SetTimeStamp(uint64_t timeStamp, unsigned component = 0);

            uint64_t GetAcquisitionID(unsigned component = 0) const;
            void SetAcquisitionID(uint64_t acqId, unsigned component = 0);

            uint64_t GetRecordID(unsigned component = 0) const;
            void SetRecordID(uint64_t recId, unsigned component = 0);

            unsigned GetRawDataFormatType(unsigned component = 0) const;
            void SetRawDataFormatType(unsigned type, unsigned component = 0);

            uint64_t GetTimeStampAtSample(unsigned sample, unsigned component = 0);

        private:
            struct PerComponentData
            {
                uint64_t fTimeStamp; // in nsec
                uint64_t fAcquisitionID;
                uint64_t fRecordID;
                unsigned fRawDataFormatType;
            };

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


    inline void KTSliceHeader::SetStartRecordAndSample(std::pair< unsigned, unsigned > rsPair)
    {
        fStartRecordNumber = rsPair.first;
        fStartSampleNumber = rsPair.second;
        return;
    }

    inline void KTSliceHeader::SetEndRecordAndSample(std::pair< unsigned, unsigned > rsPair)
    {
        fEndRecordNumber = rsPair.first;
        fEndSampleNumber = rsPair.second;
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

    inline uint64_t KTSliceHeader::GetTimeStamp(unsigned component) const
    {
        return fComponentData[component].fTimeStamp;
    }

    inline void KTSliceHeader::SetTimeStamp(uint64_t timeStamp, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fTimeStamp = timeStamp;
        return;
    }

    inline uint64_t KTSliceHeader::GetAcquisitionID(unsigned component) const
    {
        return fComponentData[component].fAcquisitionID;
    }

    inline void KTSliceHeader::SetAcquisitionID(uint64_t acqId, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fAcquisitionID = acqId;
        return;
    }

    inline uint64_t KTSliceHeader::GetRecordID(unsigned component) const
    {
        return fComponentData[component].fRecordID;
    }

    inline void KTSliceHeader::SetRecordID(uint64_t recId, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fRecordID = recId;
        return;
    }

    inline unsigned KTSliceHeader::GetRawDataFormatType(unsigned component) const
    {
        return fComponentData[component].fRawDataFormatType;
    }

    inline void KTSliceHeader::SetRawDataFormatType(unsigned type, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fRawDataFormatType = type;
        return;
    }

    inline uint64_t KTSliceHeader::GetTimeStampAtSample(unsigned sample, unsigned component)
    {
        return fComponentData[component].fTimeStamp + (uint64_t)sample * (uint64_t)(fBinWidth * 1.e9); // have to convert bin width to ns
    }


} /* namespace Katydid */
#endif /* KTSLICEHEADER_HH_ */
