/*
 * KTComplexVector.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTComplexVector.hh"

#include <cmath>
#include <iostream>

ClassImp(Katydid::KTComplexVector);

namespace Katydid
{

    KTComplexVector::KTComplexVector() :
            fMagnitude(),
            fPhase()
    {
    }

    KTComplexVector::KTComplexVector(UInt_t nBins, const Double_t* arr1, const Double_t* arr2, const string& mode) :
            fMagnitude((Int_t)nBins),
            fPhase((Int_t)nBins)
    {
        if (mode == string("P"))
        {
            fMagnitude.SetElements(arr1);
            fPhase.SetElements(arr2);
        }
        else if (mode == string("R"))
        {
           for (unsigned int iBin=0; iBin<nBins; iBin++)
            {
                fMagnitude[iBin] = sqrt(arr1[iBin]*arr1[iBin] + arr2[iBin]*arr2[iBin]);
                fPhase[iBin] = atan2(arr2[iBin], arr1[iBin]);
            }
        }
        else
        {
            std::cerr << "Error in KTComplexVector constructor: invalid mode: " << mode << std::endl;
        }
    }

    KTComplexVector::KTComplexVector(const TVectorD& v1, const TVectorD& v2, const string& mode) :
            fMagnitude(),
            fPhase()
    {
        unsigned int nBins = (unsigned int)v1.GetNoElements();
        if (nBins != (unsigned int)v2.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector constructor: number of bins don't match: " << nBins << " and " << v2.GetNoElements() << std::endl;
        }
        else
        {
            fMagnitude.ResizeTo(nBins);
            fPhase.ResizeTo(nBins);
            if (mode == string("P"))
            {
                fMagnitude = v1;
                fPhase = v2;
            }
            else if (mode == string("R"))
            {
                for (unsigned int iBin=0; iBin<nBins; iBin++)
                {
                    fMagnitude[iBin] = sqrt(v1[iBin]*v1[iBin] + v2[iBin]*v2[iBin]);
                    fPhase[iBin] = atan2(v2[iBin], v1[iBin]);
                }
            }
            else
            {
                std::cerr << "Error in KTComplexVector constructor: invalid mode: " << mode << std::endl;
            }
        }
    }

    KTComplexVector::KTComplexVector(const KTComplexVector& orig) :
            fMagnitude(orig.GetMagnitude()),
            fPhase(orig.GetPhase())
    {
    }

    KTComplexVector::~KTComplexVector()
    {
    }

    void KTComplexVector::UsePolar(const TVectorD& mag, const TVectorD& phase)
    {
        if (mag.GetNoElements() != phase.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector::UsePolar: number of bins don't match: " << mag.GetNoElements() << " and " << phase.GetNoElements() << std::endl;
            return;
        }
        fMagnitude.ResizeTo(mag.GetNoElements());
        fPhase.ResizeTo(phase.GetNoElements());
        fMagnitude = mag;
        fPhase = phase;
        return;
    }

    void KTComplexVector::UseRectangular(const TVectorD& real, const TVectorD& imag)
    {
        unsigned int nBins = (unsigned int)real.GetNoElements();
        if (nBins != (unsigned int)imag.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector::UseRectangular: number of bins don't match: " << nBins << " and " << imag.GetNoElements() << std::endl;
            return;
        }

        fMagnitude.ResizeTo(nBins);
        fPhase.ResizeTo(nBins);

        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            fMagnitude[iBin] = sqrt(real[iBin]*real[iBin] + imag[iBin]*imag[iBin]);
            fPhase[iBin] = atan2(imag[iBin], real[iBin]);
        }

        return;
    }

    KTComplexVector& KTComplexVector::operator*=(Double_t mult)
    {
        fMagnitude *= mult;
        return *this;
    }

    TH1D* KTComplexVector::CreateMagnitudeHistogram() const
    {
        TH1D* hist = new TH1D(fMagnitude);
        hist->SetNameTitle("Magnitude", "Magnitude");
        hist->SetYTitle("Magnitude");
        return hist;
    }

    TH1D* KTComplexVector::CreatePhaseHistogram() const
    {
        TH1D* hist = new TH1D(fPhase);
        hist->SetNameTitle("Phase", "Phase");
        hist->SetYTitle("Phase");
        return hist;
    }

    Double_t KTComplexVector::GetMagnitudeAt(Int_t iBin) const
    {
        return fMagnitude[iBin];
    }

    Double_t KTComplexVector::GetPhaseAt(Int_t iBin) const
    {
        return fPhase[iBin];
    }

    Int_t KTComplexVector::GetSize() const
    {
        return fMagnitude.GetNoElements();
    }

    const TVectorD& KTComplexVector::GetMagnitude() const
    {
        return fMagnitude;
    }

    const TVectorD& KTComplexVector::GetPhase() const
    {
        return fPhase;
    }

    void KTComplexVector::SetMagnitudeAt(Int_t iBin, Double_t mag)
    {
        fMagnitude[iBin] = mag;
    }

    void KTComplexVector::SetPhaseAt(Int_t iBin, Double_t phase)
    {
        fPhase[iBin] = phase;
    }

    void KTComplexVector::SetMagnitude(const TVectorD& magnitude)
    {
        fMagnitude = magnitude;
    }

    void KTComplexVector::SetPhase(const TVectorD& phase)
    {
        fPhase = phase;
    }


} /* namespace Katydid */
