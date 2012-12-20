/*
 * KTFrequencyCandidate.hh
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYCANDIDATE_HH_
#define KTFREQUENCYCANDIDATE_HH_

#include "KTWriteableData.hh"

namespace Katydid
{

    class KTFrequencyCandidate
    {
        public:
            KTFrequencyCandidate();
            KTFrequencyCandidate(const KTFrequencyCandidate& orig);
            virtual ~KTFrequencyCandidate();

            KTFrequencyCandidate& operator=(const KTFrequencyCandidate& rhs);

            UInt_t GetFirstBin() const;
            void SetFirstBin(UInt_t bin);

            UInt_t GetLastBin() const;
            void SetLastBin(UInt_t bin);

            Double_t GetMeanFrequency() const;
            void SetMeanFrequency(Double_t freq);

        protected:
            UInt_t fFirstBin;
            UInt_t fLastBin;
            Double_t fMeanFrequency;
    };

    inline UInt_t KTFrequencyCandidate::GetFirstBin() const
    {
        return fFirstBin;
    }

    inline void KTFrequencyCandidate::SetFirstBin(UInt_t bin)
    {
        fFirstBin = bin;
        return;
    }

    inline UInt_t KTFrequencyCandidate::GetLastBin() const
    {
        return fLastBin;
    }

    inline void KTFrequencyCandidate::SetLastBin(UInt_t bin)
    {
        fLastBin = bin;
        return;
    }

    inline Double_t KTFrequencyCandidate::GetMeanFrequency() const
    {
        return fMeanFrequency;
    }

    inline void KTFrequencyCandidate::SetMeanFrequency(Double_t freq)
    {
        fMeanFrequency = freq;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATE_HH_ */
