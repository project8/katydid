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
            KTTestConfigurable();
            virtual ~KTTestConfigurable();

        protected:
            Bool_t ConfigureFromPStore(const KTPStoreNode* node);
            Bool_t ConfigureFromCL();

        public:
            Int_t GetIntData() const;
            Double_t GetDoubleData() const;
            const std::string& GetStringData() const;

        private:
            Int_t fIntData;
            Double_t fDoubleData;
            std::string fStringData;
    };

    inline Int_t KTTestConfigurable::GetIntData() const
    {
        return fIntData;
    }

    inline Double_t KTTestConfigurable::GetDoubleData() const
    {
        return fDoubleData;
    }

    inline const std::string& KTTestConfigurable::GetStringData() const
    {
        return fStringData;
    }

} /* namespace Katydid */
#endif /* KTTESTCONFIGURABLE_HH_ */
