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
            fComponent(0),
            fFitData(1)
    {
    }

    KTLinearFitResult::KTLinearFitResult(const KTLinearFitResult& orig) :
            KTExtensibleData< KTLinearFitResult >(orig),
            fComponent(orig.fComponent),
            fFitData(orig.fFitData)
    {
    }

    KTLinearFitResult::~KTLinearFitResult()
    {
    }

    KTLinearFitResult& KTLinearFitResult::operator=(const KTLinearFitResult& rhs)
    {
        KTExtensibleData< KTLinearFitResult >::operator=(rhs);
        fFitData = rhs.fFitData;
        return *this;
    }

    std::ostream& operator<<(std::ostream& out, const KTLinearFitResult& hdr)
    {
        out << "# of Components: " << hdr.GetNFits() << '\n';
        for (unsigned iFit = 0; iFit < hdr.GetNFits(); ++iFit)
        {
            out << "Linear Fit Result Contents:\n" <<
                "\tSlope: " << hdr.GetSlope(iFit) << '\n' <<
                "\tIntercept: " << hdr.GetIntercept(iFit) << '\n' <<
                /*"\tFirst local min deviation (sigma): " << hdr.GetFineProbe_sigma_1(iFit) << '\n' <<
                "\tSecond local min deviation (sigma): " << hdr.GetFineProbe_sigma_2(iFit) << '\n' <<
                "\tFirst local min deviation (SNR): " << hdr.GetFineProbe_SNR_1(iFit) << '\n' <<
                "\tSecond local min deviation (SNR): " << hdr.GetFineProbe_SNR_2(iFit) << '\n' <<
                "\tMagnetron peak: " << hdr.GetFFT_peak(iFit) << '\n' <<
                "\tMagnetron peak uncertainty: " << hdr.GetFFT_peak_uncertainty(iFit) << '\n' <<
                "\tMagnetron peak significance (sigma): " << hdr.GetFFT_sigma(iFit) << '\n' <<
                "\tMagnetron peak significance (SNR): " << hdr.GetFFT_SNR(iFit) << '\n' <<
                */"\tFit bandwidth: " << hdr.GetFit_width(iFit) << '\n' <<
                "\tPoints in fit: " << hdr.GetNPoints(iFit) << '\n' <<
                "\tProbe width: " << hdr.GetProbeWidth(iFit) << '\n';
        }
        return out;
    }

} /* namespace Katydid */
