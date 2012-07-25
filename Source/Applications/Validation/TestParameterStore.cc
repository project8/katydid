/*
 * TestTestParameterStore.cc
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 *
 *  Command-line arguments:
 *    1. Config filename (use TestParameterStoreConfig.json
 */

#include "KTParameterStore.hh"
#include "KTTestConfigurable.hh"

#include <iostream>
#include <string>

using namespace Katydid;
using namespace std;

int main(int argc, char** argv)
{
    string configFilename(argv[1]);

    KTParameterStore* store = KTParameterStore::GetInstance();
    store->ReadConfigFile(configFilename);

    KTTestConfigurable* testObj = new KTTestConfigurable();

    delete testObj;

    return 0;
}
