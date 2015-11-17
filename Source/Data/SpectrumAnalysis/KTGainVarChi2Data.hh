/*
 * KTGainVarChi2Data.hh
 *
 *  Created on: Oct 8, 2015
 *      Author: ezayas
 */

#ifndef KTGAINVARCHI2DATA_HH
#define KTGAINVARCHI2DATA_HH

#include "KTData.hh"

#include <inttypes.h>
#include <utility>
#include <vector>

namespace Katydid
{
    using namespace Nymph;
    class KTGainVarChi2Data : public KTExtensibleData< KTGainVarChi2Data >
    {
        public:
            KTGainVarChi2Data();
            KTGainVarChi2Data(const KTGainVarChi2Data& orig);
            virtual ~KTGainVarChi2Data();

            KTGainVarChi2Data& operator=(const KTGainVarChi2Data& rhs);

            unsigned GetNComponents() const;
            KTGainVarChi2Data& SetNComponents(unsigned num);

        public:
            double GetChi2(unsigned component = 0) const;
            void SetChi2(double chi2, unsigned component = 0);

            uint64_t GetNDF(unsigned component = 0) const;
            void SetNDF(uint64_t ndf, unsigned component = 0);

        private:
            struct PerComponentData
            {
                double Chi2;
                uint64_t NDF;
            };

            std::vector< PerComponentData > fComponentData;

        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTGainVarChi2Data& hdr);

    inline unsigned KTGainVarChi2Data::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline KTGainVarChi2Data& KTGainVarChi2Data::SetNComponents(unsigned num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline double KTGainVarChi2Data::GetChi2(unsigned component) const
    {
        return fComponentData[component].fChi2;
    }

    inline void KTGainVarChi2Data::SetChi2(double chi2, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fChi2 = chi2;
        return;
    }

    inline uint64_t KTGainVarChi2Data::GetNDF(unsigned component) const
    {
        return fComponentData[component].fNDF;
    }

    inline void KTGainVarChi2Data::SetNDF(uint64_t ndf, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fNDF = ndf;
        return;
    }

} /* namespace Katydid */
#endif /* KTGainVarChi2Data_HH_ */
