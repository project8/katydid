/*
 * KTComplexVector.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTCOMPLEXVECTOR_HH_
#define KTCOMPLEXVECTOR_HH_

#include "TVectorT.h"
#include "TH1.h"

#include <string>
using std::string;

namespace Katydid
{
    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTComplexVector
    {
        public:
            KTComplexVector();
            KTComplexVector(UInt_t nBins);
            KTComplexVector(UInt_t nBins, const Double_t* arr1, const Double_t* arr2, const string& mode);
            KTComplexVector(const TVectorD& v1, const TVectorD& v2, const string& mode);
            KTComplexVector(const KTComplexVector& orig);
            virtual ~KTComplexVector();

            virtual void UsePolar(const TVectorD& mag, const TVectorD& phase);
            virtual void UseRectangular(const TVectorD& real, const TVectorD& imag);

            KTComplexVector& operator*=(Double_t mult);

            // element-by-element division; div assumed real so only magnitude is divided
            KTComplexVector& operator/=(const KTPhysicalArray< 1, Double_t >& div);

            virtual TH1D* CreateMagnitudeHistogram(const std::string& name) const;
            virtual TH1D* CreateMagnitudeHistogram() const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name) const;
            virtual TH1D* CreatePhaseHistogram() const;

            virtual KTPhysicalArray< 1, Double_t >* CreateMagnitudePhysArr() const;
            virtual KTPhysicalArray< 1, Double_t >* CreatePhasePhysArr() const;

            Double_t GetMagnitudeAt(Int_t iBin) const;
            Double_t GetPhaseAt(Int_t iBin) const;

            Int_t GetSize() const;

            TVectorD& GetMagnitude();
            const TVectorD& GetMagnitude() const;
            TVectorD& GetPhase();
            const TVectorD& GetPhase() const;

            void SetMagnitudeAt(Int_t iBin, Double_t mag);
            void SetPhaseAt(Int_t iBin, Double_t phase);

            void SetMagnitude(const TVectorD& mag);
            void SetPhase(const TVectorD& phase);

       protected:

            TVectorD fMagnitude;
            TVectorD fPhase;

            ClassDef(KTComplexVector, 2);
    };

} /* namespace Katydid */
#endif /* KTCOMPLEXVECTOR_HH_ */
