/*
 * TestTestParameterStore.cc
 *
 *  Created on: Jul 2, 2012
 *      Author: nsoblath
 */

#include "KTParameterStore.hh"

#include <iostream>
#include <string>

using namespace Katydid;
using namespace std;

int main()
{
    KTParameterStore* store = KTParameterStore::GetInstance();

    cout << "Default value as int: " << store->GetParameter<int>("") << endl;
    //cout << "Default value as string: " << store->GetParameter<string>("") << endl;

    return 0;
}
