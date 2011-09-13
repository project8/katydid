/*
 * LoadLibraries.C
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include <cstdio>

void LoadLibraries(const char* dirRelativeToKatydid, Bool_t printDir=kFALSE) {
    // establish the directory where the libraries are located relative to the run location
    char libDir[512];
    sprintf(libDir, "%s/lib", dirRelativeToKatydid);
    if (printDir) printf("lib dir: %s", libDir);

    // load libEgg
    char libEgg[512];
    sprintf(libEgg, "%s/libEgg.so", libDir);
    if (printDir) printf("libEgg: ", libEgg);
    gSystem->Load(libEgg);
}
