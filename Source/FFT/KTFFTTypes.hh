/*
 * KTFFTTypes.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */


#ifndef KTFFTTYPES_HH_
#define KTFFTTYPES_HH_

#include "complexpolar.hh"
#include "KTPhysicalArray.hh"

#include "Rtypes.h"

// these go with the temporary function
#ifdef ROOT_FOUND
#include "TH1.h"
#endif

namespace Katydid
{

    typedef KTPhysicalArray< 1, complexpolar< Double_t > > KTFrequencySpectrum;


    // This function is temporary!  It will be moved to a writer class.
#ifdef ROOT_FOUND
    TH1D* CreatePowerSpectrumHistFromFreqSpect(const std::string& histName, const KTFrequencySpectrum* freqSpect);
#endif

}

#endif /* KTFFTTYPES_HH_ */
