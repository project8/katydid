/*
 * KTProcSummary.hh
 *
 *  Created on: May 9, 2013
 *      Author: nsoblath
 */

#ifndef KTPROCSUMMARY_HH_
#define KTPROCSUMMARY_HH_

#include "Rtypes.h"

#include <string>

namespace Katydid
{
    class KTProcSummary
    {
        public:
            KTProcSummary();
            KTProcSummary(const KTProcSummary& orig);
            ~KTProcSummary();

            KTProcSummary& operator=(const KTProcSummary& rhs);

        public:
            void SetNSlicesProcessed(UInt_t slices);
            UInt_t GetNSlicesProcessed() const;

            void SetNRecordsProcessed(UInt_t recs);
            UInt_t GetNRecordsProcessed() const;

            void SetIntegratedTime(Double_t acqr);
            Double_t GetIntegratedTime() const;

        protected:
            UInt_t fNSlicesProcessed;
            UInt_t fNRecordsProcessed; /// if any samples from a record were used, it's counted
            Double_t fIntegratedTime; /// # of slices * slice size * bin width

    };

    inline void KTProcSummary::SetNSlicesProcessed(UInt_t slices)
    {
        fNSlicesProcessed = slices;
        return;
    }

    inline UInt_t KTProcSummary::GetNSlicesProcessed() const
    {
        return fNSlicesProcessed;
    }

    inline void KTProcSummary::SetNRecordsProcessed(UInt_t recs)
    {
        fNRecordsProcessed = recs;
        return;
    }

    inline UInt_t KTProcSummary::GetNRecordsProcessed() const
    {
        return fNRecordsProcessed;
    }

     inline void KTProcSummary::SetIntegratedTime(Double_t time)
    {
        fIntegratedTime = time;
        return;
    }

    inline Double_t KTProcSummary::GetIntegratedTime() const
    {
        return fIntegratedTime;
    }


} /* namespace Katydid */
#endif /* KTPROCSUMMARY_HH_ */
