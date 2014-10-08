/*
 * KTTestConfigurable.hh
 *
 *  Created on: Jul 25, 2012
 *      Author: nsoblath
 */

#ifndef KTTESTCONFIGURABLE_HH_
#define KTTESTCONFIGURABLE_HH_

#include "KTConfigurable.hh"

namespace Katydid
{
    class KTTestConfigurable : public KTConfigurable
    {
        public:
            KTTestConfigurable(const std::string& name = "test-configurable");
            virtual ~KTTestConfigurable();

        public:
            bool Configure(const KTParamNode* node);

        public:
            int GetIntData() const;
            double GetDoubleData() const;
            const std::string& GetStringData() const;

            void PrintConfig() const;

        private:
            int fIntData;
            double fDoubleData;
            std::string fStringData;
    };

    inline int KTTestConfigurable::GetIntData() const
    {
        return fIntData;
    }

    inline double KTTestConfigurable::GetDoubleData() const
    {
        return fDoubleData;
    }

    inline const std::string& KTTestConfigurable::GetStringData() const
    {
        return fStringData;
    }

} /* namespace Katydid */
#endif /* KTTESTCONFIGURABLE_HH_ */
