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
            fMagnitude(new TVectorD()),
            fPhase(new TVectorD())
    {
    }

    KTComplexVector::KTComplexVector(Int_t nBins, const Double_t* arr1, const Double_t* arr2, const string& mode)
    {
        if (mode == string("P"))
        {
            fMagnitude = new TVectorD(nBins, arr1);
            fPhase = new TVectorD(nBins, arr2);
        }
        else if (mode == string("R"))
        {
            fMagnitude = new TVectorD(nBins);
            fPhase = new TVectorD(nBins);
            for (Int_t iBin=0; iBin<nBins; iBin++)
            {
                (*fMagnitude)[iBin] = sqrt(arr1[iBin]*arr1[iBin] + arr2[iBin]*arr2[iBin]);
                (*fPhase)[iBin] = atan2(arr2[iBin], arr1[iBin]);
            }
        }
        else
        {
            std::cerr << "Error in KTComplexVector constructor: invalid mode: " << mode << std::endl;
            fMagnitude = new TVectorD();
            fPhase = new TVectorD();
        }
    }

    KTComplexVector::KTComplexVector(const TVectorD& v1, const TVectorD& v2, const string& mode)
    {
        Int_t nBins = v1.GetNoElements();
        if (nBins != v2.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector constructor: number of bins don't match: " << nBins << " and " << v2.GetNoElements() << std::endl;
            fMagnitude = new TVectorD();
            fPhase = new TVectorD();
        }
        else
        {
            if (mode == string("P"))
            {
                fMagnitude = new TVectorD(v1);
                fPhase = new TVectorD(v2);
            }
            else if (mode == string("R"))
            {
                fMagnitude = new TVectorD(nBins);
                fPhase = new TVectorD(nBins);
                for (Int_t iBin=0; iBin<nBins; iBin++)
                {
                    (*fMagnitude)[iBin] = sqrt(v1[iBin]*v1[iBin] + v2[iBin]*v2[iBin]);
                    (*fPhase)[iBin] = atan2(v2[iBin], v1[iBin]);
                }
            }
            else
            {
                std::cerr << "Error in KTComplexVector constructor: invalid mode: " << mode << std::endl;
                fMagnitude = new TVectorD();
                fPhase = new TVectorD();
            }
        }
    }

    KTComplexVector::KTComplexVector(const KTComplexVector& orig) :
            fMagnitude(new TVectorD(*(orig.GetMagnitude()))),
            fPhase(new TVectorD(*(orig.GetPhase())))
    {
    }

    KTComplexVector::~KTComplexVector()
    {
        delete fMagnitude;
        delete fPhase;
    }

    void KTComplexVector::UsePolar(const TVectorD& mag, const TVectorD& phase)
    {
        if (mag.GetNoElements() != phase.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector::UsePolar: number of bins don't match: " << mag.GetNoElements() << " and " << phase.GetNoElements() << std::endl;
            return;
        }
        delete fMagnitude;
        delete fPhase;
        fMagnitude = new TVectorD(mag);
        fPhase = new TVectorD(phase);
        return;
    }

    void KTComplexVector::UseRectangular(const TVectorD& real, const TVectorD& imag)
    {
        Int_t nBins = real.GetNoElements();
        if (nBins != imag.GetNoElements())
        {
            std::cerr << "Error in KTComplexVector::UseRectangular: number of bins don't match: " << nBins << " and " << imag.GetNoElements() << std::endl;
            return;
        }

        delete fMagnitude;
        delete fPhase;

        fMagnitude = new TVectorD(nBins);
        fPhase = new TVectorD(nBins);

        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            (*fMagnitude)[iBin] = sqrt(real[iBin]*real[iBin] + imag[iBin]*imag[iBin]);
            (*fPhase)[iBin] = atan2(imag[iBin], real[iBin]);
        }

        return;
    }

    KTComplexVector& KTComplexVector::operator*=(Double_t mult)
    {
        (*fMagnitude) *= mult;
        return *this;
    }

    TH1D* KTComplexVector::CreateMagnitudeHistogram() const
    {
        TH1D* hist = new TH1D(*fMagnitude);
        hist->SetNameTitle("Magnitude", "Magnitude");
        hist->SetYTitle("Magnitude");
        return hist;
    }

    TH1D* KTComplexVector::CreatePhaseHistogram() const
    {
        TH1D* hist = new TH1D(*fPhase);
        hist->SetNameTitle("Phase", "Phase");
        hist->SetYTitle("Phase");
        return hist;
    }

    Double_t KTComplexVector::GetMagnitudeAt(Int_t iBin) const
    {
        return (*fMagnitude)[iBin];
    }

    Double_t KTComplexVector::GetPhaseAt(Int_t iBin) const
    {
        return (*fPhase)[iBin];
    }

    Int_t KTComplexVector::GetSize() const
    {
        return fMagnitude->GetNoElements();
    }

    TVectorD* KTComplexVector::GetMagnitude() const
    {
        return fMagnitude;
    }

    TVectorD* KTComplexVector::GetPhase() const
    {
        return fPhase;
    }

    void KTComplexVector::SetMagnitudeAt(Int_t iBin, Double_t mag)
    {
        (*fMagnitude)[iBin] = mag;
    }

    void KTComplexVector::SetPhaseAt(Int_t iBin, Double_t phase)
    {
        (*fPhase)[iBin] = phase;
    }

    void KTComplexVector::SetMagnitude(const TVectorD& magnitude)
    {
        delete fMagnitude;
        fMagnitude = new TVectorD(magnitude);
    }

    void KTComplexVector::SetPhase(const TVectorD& phase)
    {
        delete fPhase;
        fPhase = new TVectorD(phase);
    }


} /* namespace Katydid */
