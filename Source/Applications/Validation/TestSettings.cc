/*
 * TestSettings.cc
 *
 *  Created on: Jan 18, 2012
 *      Author: nsoblath
 */

#include "KTSetting.hh"

#include <iostream>
#include <string>

using namespace Katydid;

int main(int argc, char** argv)
{
    bool success = true;

    KTSetting set1("set1", (int)1);
    std::cout << "#line 1# " << set1.GetValue<int>() << std::endl;
    success = success && set1.GetValue<int>() == 1;

    set1.SetValue<unsigned>(5);
    std::cout << "#line 2# " << set1.GetValue<unsigned>() << std::endl;
    success = success && set1.GetValue<unsigned>() == 5;

    set1.SetValue<float>(20.);
    std::cout << "#line 3# " << set1.GetValue<float>() << std::endl;
    success = success && set1.GetValue<float>() == 20.;

    KTSetting set2("set2", std::string("test string"));
    std::cout << "#line 4# " << set2.GetValue<std::string>() << std::endl;
    success = success && set2.GetValue<std::string>() == std::string("test string");

    KTSetting set3("set3", new int(14));
    std::cout << "#line 5# " << *(set3.GetValue<int*>()) << std::endl;
    success = success && *(set3.GetValue<int*>()) == 14;

    if (success) return 0;
    return 1;
}

