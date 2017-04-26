/**
 @file KTTimeLibPy.hh
 @brief Collects python wrappers for Time submodule
 @author: B. H. LaRoque
 @date: April 11, 2017
 */

#ifndef KTTIMELIBPY_HH_
#define KTTIMELIBPY_HH_

#include "KTEggProcessorPy.hh"

void export_KTTimePy()
{
    export_KTEggProcessorPy();
}
#endif /* KTTIMELIBPY_HH_ */
