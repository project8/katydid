/*
 * KTPowerFitData.hh
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#ifndef KTPOWERFITDATA_HH
#define KTPOWERFITDATA_HH

#include "KTData.hh"

#include <inttypes.h>
#include <utility>
#include <vector>

namespace Katydid
{
    
    class KTPowerFitData : public Nymph::KTExtensibleData< KTPowerFitData >
    {
        public:
            KTPowerFitData();
            KTPowerFitData(const KTPowerFitData& orig);
            virtual ~KTPowerFitData();

            KTPowerFitData& operator=(const KTPowerFitData& rhs);

        public:

            double GetCurvature() const;
            void SetCurvature(double k);

            double GetWidth() const;
            void SetWidth(double sigma);
        
        private:
            double fCurvature;
            double fWidth;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTPowerFitData& hdr);

    inline double KTPowerFitData::GetCurvature() const
    {
        return fCurvature;
    }

    inline void KTPowerFitData::SetCurvature(double k)
    {
        fCurvature = k;
        return;
    }

    inline double KTPowerFitData::GetWidth() const
    {
        return fWidth;
    }

    inline void KTPowerFitData::SetWidth(double sigma)
    {
        fWidth = sigma;
        return;
    }
    

} /* namespace Katydid */
#endif /* KTPOWERFITDATA_HH */
