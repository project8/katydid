/*
 * KTLinearFitResult.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: ezayas
 */

#include "KTLinearFitResult.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTLinearFitResult::sName("linear-fit-result");

    KTLinearFitResult::KTLinearFitResult() :
            KTExtensibleData< KTLinearFitResult >(),
            fComponentData(1)
    {
    }

    KTLinearFitResult::KTLinearFitResult(const KTLinearFitResult& orig) :
            KTExtensibleData< KTLinearFitResult >(orig),
            
            fComponentData(orig.fComponentData)
    {
    }

    KTLinearFitResult::~KTLinearFitResult()
    {
    }

    KTLinearFitResult& KTLinearFitResult::operator=(const KTLinearFitResult& rhs)
    {
        KTExtensibleData< KTLinearFitResult >::operator=(rhs);
        fComponentData = rhs.fComponentData;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTLinearFitResult& hdr)
    {
        out << "# of Components: " << hdr.GetNComponents() << '\n';
        for (unsigned iComponent = 0; iComponent < hdr.GetNComponents(); ++iComponent)
        {
            out << "Linear Fit Result Contents:\n" <<
                "\tSlope: " << hdr.GetSlope(iComponent) << '\n' <<
                "\tIntercept: " << hdr.GetIntercept(iComponent) << '\n' <<
                /*"\tFirst local min deviation (sigma): " << hdr.GetFineProbe_sigma_1(iComponent) << '\n' <<
                "\tSecond local min deviation (sigma): " << hdr.GetFineProbe_sigma_2(iComponent) << '\n' <<
                "\tFirst local min deviation (SNR): " << hdr.GetFineProbe_SNR_1(iComponent) << '\n' <<
                "\tSecond local min deviation (SNR): " << hdr.GetFineProbe_SNR_2(iComponent) << '\n' <<
                "\tMagnetron peak: " << hdr.GetFFT_peak(iComponent) << '\n' <<
                "\tMagnetron peak uncertainty: " << hdr.GetFFT_peak_uncertainty(iComponent) << '\n' <<
                "\tMagnetron peak significance (sigma): " << hdr.GetFFT_sigma(iComponent) << '\n' <<
                "\tMagnetron peak significance (SNR): " << hdr.GetFFT_SNR(iComponent) << '\n' <<
                */"\tFit bandwidth: " << hdr.GetFit_width(iComponent) << '\n' <<
                "\tPoints in fit: " << hdr.GetNPoints(iComponent) << '\n' <<
                "\tProbe width: " << hdr.GetProbeWidth(iComponent) << '\n';
        }
        return out;
    }

} /* namespace Katydid */
