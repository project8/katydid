/*
 * KTDemangle.hh
 *
 *  Created on: Feb 26, 2018
 *      Author: N.S. Oblath
 */

#ifndef KATYDID_KTDEMANGLE_HH_
#define KATYDID_KTDEMANGLE_HH_

#include <cxxabi.h>
#include <string>
#include <typeinfo>


namespace Katydid
{

    std::string Demangle(const char* name)
    {
        char* realname;
        int status;
        std::string strRealname;
        realname = abi::__cxa_demangle(name, 0, 0, &status);
        if (status == 0)
        {
            strRealname = realname;
        }
        free(realname);
        return strRealname;
    }

    std::string DemangledName(const std::type_info& type)
    {
        return Demangle(type.name());
    }

} /* namespace Katydid */

#endif /* KATYDID_KTDEMANGLE_HH_ */
