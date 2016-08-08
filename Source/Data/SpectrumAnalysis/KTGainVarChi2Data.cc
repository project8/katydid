/*
 * KTGainVarChi2Data.cc
 *
 *  Created on: Oct 8, 2015
 *      Author: ezayas
 */

#include "KTGainVarChi2Data.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTGainVarChi2Data::sName("gain-variation-chi2");

    KTGainVarChi2Data::KTGainVarChi2Data() :
            KTExtensibleData< KTGainVarChi2Data >(),
            fComponentData(1)
    {
    }

    KTGainVarChi2Data::KTGainVarChi2Data(const KTGainVarChi2Data& orig) :
            KTExtensibleData< KTGainVarChi2Data >(orig),
            
            fComponentData(orig.fComponentData)
    {
    }

    KTGainVarChi2Data::~KTGainVarChi2Data()
    {
    }

    KTGainVarChi2Data& KTGainVarChi2Data::operator=(const KTGainVarChi2Data& rhs)
    {
        KTExtensibleData< KTGainVarChi2Data >::operator=(rhs);
        fComponentData = rhs.fComponentData;

        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTGainVarChi2Data& hdr)
    {
        out << "# of Components: " << hdr.GetNComponents() << '\n';
        for (unsigned iComponent = 0; iComponent < hdr.GetNComponents(); ++iComponent)
        {
            out << "Chi-squared Contents:\n" <<
                "\tChi-squared: " << hdr.GetChi2(iComponent) << '\n' <<
                "\tNDF: " << hdr.GetNDF(iComponent) << '\n' <<
                "\tReduced chi-squared: " << hdr.GetChi2(iComponent)/hdr.GetNDF(iComponent) << '\n';
        
        }
        return out;
    }

} /* namespace Katydid */