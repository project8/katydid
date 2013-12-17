/*
 * KTProcSummary.hh
 *
 *  Created on: May 9, 2013
 *      Author: nsoblath
 */

#ifndef KTPROCSUMMARY_HH_
#define KTPROCSUMMARY_HH_

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
            void SetNSlicesProcessed(unsigned slices);
            unsigned GetNSlicesProcessed() const;

            void SetNRecordsProcessed(unsigned recs);
            unsigned GetNRecordsProcessed() const;

            void SetIntegratedTime(double acqr);
            double GetIntegratedTime() const;

        protected:
            unsigned fNSlicesProcessed;
            unsigned fNRecordsProcessed; /// if any samples from a record were used, it's counted
            double fIntegratedTime; /// # of slices * slice size * bin width

    };

    inline void KTProcSummary::SetNSlicesProcessed(unsigned slices)
    {
        fNSlicesProcessed = slices;
        return;
    }

    inline unsigned KTProcSummary::GetNSlicesProcessed() const
    {
        return fNSlicesProcessed;
    }

    inline void KTProcSummary::SetNRecordsProcessed(unsigned recs)
    {
        fNRecordsProcessed = recs;
        return;
    }

    inline unsigned KTProcSummary::GetNRecordsProcessed() const
    {
        return fNRecordsProcessed;
    }

     inline void KTProcSummary::SetIntegratedTime(double time)
    {
        fIntegratedTime = time;
        return;
    }

    inline double KTProcSummary::GetIntegratedTime() const
    {
        return fIntegratedTime;
    }


} /* namespace Katydid */
#endif /* KTPROCSUMMARY_HH_ */
