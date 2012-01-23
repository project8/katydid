/*
 * KTSetting.hh
 *
 *  Created on: Jan 17, 2012
 *      Author: nsoblath
 */

#ifndef KTSETTING_HH_
#define KTSETTING_HH_

#include <boost/any.hpp>

#include <cstdlib>

#include <iostream>
#include <string>
#include <sstream>

using std::istringstream;
using std::ostringstream;
using std::string;

namespace Katydid
{
    class KTSetting
    {
        public:
            KTSetting();
            template< typename XValueType >
            KTSetting(const string& name, XValueType value);
            ~KTSetting();

        public:
            const string& GetName() const;
            void SetName(const string& name);

        private:
            string fName;

        public:
            template< typename XValueType >
            XValueType GetValue() const;

            template< typename XValueType >
            void SetValue(XValueType value);

        private:
            boost::any fValue;
    };

    template< typename XValueType >
    KTSetting::KTSetting(const string& name, XValueType value) :
        fName(name),
        fValue(value)
    {
    }

    inline const string& KTSetting::GetName() const
    {
        return fName;
    }

    inline void KTSetting::SetName(const string& name)
    {
        fName = name;
        return;
    }

    template< typename XValueType >
    inline XValueType KTSetting::GetValue() const
    {
        try
        {
            XValueType returnVal = boost::any_cast<XValueType>(fValue);
            return returnVal;
        }
        catch(const boost::bad_any_cast &)
        {
            std::cout << "Error in KTSetting::GetValue() (name = " << fName << "): Unable to cast to the given type." << std::endl;
            exit(-1);
            //return XValueType();
        }
    }

    template< typename XValueType >
    inline void KTSetting::SetValue(XValueType value)
    {
        fValue = value;
        return;
    }

} /* namespace Katydid */
#endif /* KTSETTING_HH_ */
