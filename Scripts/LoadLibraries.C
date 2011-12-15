/*
 * LoadLibraries.C
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include <cstdio>
#include "../cbuild/KatydidConfig.hh"

void LoadLibraries(Bool_t printDir=kFALSE) {
    // establish the directory where the libraries are located relative to the run location
    char libDir[512];
    sprintf(libDir, "%s/lib", INSTALL_PREFIX);
    if (printDir) printf("lib dir: %s", libDir);

    // load libEgg
    char libEgg[512];
    sprintf(libEgg, "%s/libKatydidEgg.so", libDir);
    if (printDir) printf("libEgg: ", libEgg);
    gSystem->Load(libEgg);

    // load libFFT
    char libFFT[512];
    sprintf(libFFT, "%s/libKatydidFFT.so", libDir);
    if (printDir) printf("libFFT: ", libFFT);
    gSystem->Load(libFFT);
}
