/*
 * KTWaterfallCandidate.hh
 *
 *  Created on: Mar 14, 2012
 *      Author: nsoblath
 */

#ifndef KTWATERFALLCANDIDATE_HH_
#define KTWATERFALLCANDIDATE_HH_

//#include "KTPhysicalArray.hh"

#include "TH2.h"

namespace Katydid
{

    class KTWaterfallCandidate : public TObject
    {
        public:
            //typedef KTPhysicalArray< 2, Double_t > DataArray;

        public:
            KTWaterfallCandidate();
            KTWaterfallCandidate(const KTWaterfallCandidate& orig);
            virtual ~KTWaterfallCandidate();

        public:
            KTWaterfallCandidate& operator=(const KTWaterfallCandidate& rhs);

        // Access to the data
        public:
            //const DataArray& GetData() const;
            const TH2D& GetData() const;
            UInt_t GetEventNumber() const;
            Double_t GetStartFrequency() const;
            Double_t GetStartTime() const;
            Double_t GetEndFrequency() const;
            Double_t GetEndTime() const;

            //void SetData(const DataArray& data);
            void SetData(const TH2D& data);
            void SetEventNumber(UInt_t num);
            void SetStartFrequency(Double_t freq);
            void SetStartTime(Double_t time);
            void SetEndFrequency(Double_t freq);
            void SetEndTime(Double_t time);

        private:
            // the actual data that makes up the candidate
            //DataArray fData;
            TH2D fData;

            // characteristics of the event from which it came
            UInt_t fEventNumber;

            // characteristics of the candidate
            Double_t fStartFrequency;
            Double_t fStartTime;
            Double_t fEndFrequency;
            Double_t fEndTime;

            ClassDef(KTWaterfallCandidate, 1);
    };

    //inline const KTWaterfallCandidate::DataArray& KTWaterfallCandidate::GetData() const
    inline const TH2D& KTWaterfallCandidate::GetData() const
    {
        return fData;
    }

    inline UInt_t KTWaterfallCandidate::GetEventNumber() const
    {
        return fEventNumber;
    }

    inline Double_t KTWaterfallCandidate::GetStartFrequency() const
    {
        return fStartFrequency;
    }

    inline Double_t KTWaterfallCandidate::GetStartTime() const
    {
        return fStartTime;
    }

    inline Double_t KTWaterfallCandidate::GetEndFrequency() const
    {
        return fEndFrequency;
    }

    inline Double_t KTWaterfallCandidate::GetEndTime() const
    {
        return fEndTime;
    }

    //inline void KTWaterfallCandidate::SetData(const DataArray& data)
    inline void KTWaterfallCandidate::SetData(const TH2D& data)
    {
        fData = data;
        return;
    }

    inline void KTWaterfallCandidate::SetEventNumber(UInt_t num)
    {
        fEventNumber = num;
        return;
    }

    inline void KTWaterfallCandidate::SetStartFrequency(Double_t freq)
    {
        fStartFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidate::SetStartTime(Double_t time)
    {
        fStartTime = time;
        return;
    }

    inline void KTWaterfallCandidate::SetEndFrequency(Double_t freq)
    {
        fEndFrequency = freq;
        return;
    }

    inline void KTWaterfallCandidate::SetEndTime(Double_t time)
    {
        fEndTime = time;
        return;
    }


} /* namespace Katydid */
#endif /* KTWATERFALLCANDIDATE_HH_ */
