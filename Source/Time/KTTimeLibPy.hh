/**
 @file KTTimeLibPy.hh
 @brief Collects python wrappers for Time submodule
 @author: B. H. LaRoque
 @date: April 11, 2017
 */

#ifndef KTTIMELIBPY_HH_
#define KTTIMELIBPY_HH_

#include "KTEggReaderPy.hh"
#include "KTEgg3ReaderPy.hh"
#include "KTRSAMatReaderPy.hh"
#include "KTEggProcessorPy.hh"

void export_KTTimePy()
{
    export_KTEggReaderPy();
    export_KTEgg3ReaderPy();
    export_KTRSAMatReaderPy();
    export_KTEggProcessorPy();
}
#endif /* KTTIMELIBPY_HH_ */
