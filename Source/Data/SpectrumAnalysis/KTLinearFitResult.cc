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
/*          fSlope(0.),
            fIntercept(0.),
            fIntercept_deviation(0.),
            fFineProbe_sigma_1(0.),
            fFineProbe_sigma_2(0.),
            fFineProbe_SNR_1(0.),
            fFineProbe_SNR_2(0.),
            //fFFT_peak(0.),
            //fFFT_peak_uncertainty(0.),
            //fFFT_sigma(0.),
            //fFFT_SNR(0.),
            fFit_width(0.),
            fNPoints(0),
            fProbeWidth(0.)
*/
    {
    }

    KTLinearFitResult::KTLinearFitResult(const KTLinearFitResult& orig) :
            KTExtensibleData< KTLinearFitResult >(orig),
            
            fComponentData(orig.fComponentData)
/*          fSlope(orig.fSlope),
            fIntercept(orig.fIntercept),
            fIntercept_deviation(orig.fIntercept_deviation),
            fFineProbe_sigma_1(orig.fFineProbe_sigma_1),
            fFineProbe_sigma_2(orig.fFineProbe_sigma_2),
            fFineProbe_SNR_1(orig.fFineProbe_SNR_1),
            fFineProbe_SNR_2(orig.fFineProbe_SNR_2),
            //fFFT_peak(orig.fFFT_peak),
            //fFFT_peak_uncertainty(orig.fFFT_peak_uncertainty),
            //fFFT_sigma(orig.fFFT_sigma),
            //fFFT_SNR(orig.fFFT_SNR),
            fFit_width(orig.fFit_width),
            fNPoints(orig.fNPoints),
            fProbeWidth(orig.fProbeWidth)
*/
    {
    }

    KTLinearFitResult::~KTLinearFitResult()
    {
    }

    KTLinearFitResult& KTLinearFitResult::operator=(const KTLinearFitResult& rhs)
    {
        KTExtensibleData< KTLinearFitResult >::operator=(rhs);
        fComponentData = rhs.fComponentData;
/*
        fSlope = rhs.fSlope;
        fIntercept = rhs.fIntercept;
        fIntercept_deviation = rhs.fIntercept_deviation;
        fFineProbe_sigma_1 = orig.fFineProbe_sigma_1;
        fFineProbe_sigma_2 = rhs.fFineProbe_sigma_2;
        fFineProbe_SNR_1 = rhs.fFineProbe_SNR_1;
        fFineProbe_SNR_2 = rhs.fFineProbe_SNR_2;
        //fFFT_peak = rhs.fFFT_peak;
        //fFFT_peak_uncertainty = rhs.fFFT_peak_uncertainty;
        //fFFT_sigma = rhs.fFFT_sigma;
        //fFFT_SNR = rhs.fFFT_SNR;
        fFit_width = rhs.fFit_width;
        fNPoints = rhs.fNPoints;
        fProbeWidth = rhs.fProbeWidth;
*/
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
                "\tIntercept deviation: " << hdr.GetIntercept_deviation(iComponent) << '\n' <<
                "\tFirst local min deviation (sigma): " << hdr.GetFineProbe_sigma_1(iComponent) << '\n' <<
                "\tSecond local min deviation (sigma): " << hdr.GetFineProbe_sigma_2(iComponent) << '\n' <<
                "\tFirst local min deviation (SNR): " << hdr.GetFineProbe_SNR_1(iComponent) << '\n' <<
                "\tSecond local min deviation (SNR): " << hdr.GetFineProbe_SNR_2(iComponent) << '\n' <<
                /*"\tMagnetron peak: " << hdr.GetFFT_peak(iComponent) << '\n' <<
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