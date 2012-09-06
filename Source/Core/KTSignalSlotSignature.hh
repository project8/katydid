/*
 * KTSignalSlotSignature.hh
 *
 *  Created on: Sep 6, 2012
 *      Author: nsoblath
 */

#ifndef KTSIGNALSLOTSIGNATURE_HH_
#define KTSIGNALSLOTSIGNATURE_HH_

#include "Rtypes.h"

#include <stdexcept>
#include <string>
#include <vector>

namespace Katydid
{

    class KTSignalSlotSignature
    {
        public:
            KTSignalSlotSignature();
            KTSignalSlotSignature(const std::string& signature);
            virtual ~KTSignalSlotSignature();

            const std::string& GetSignature() const;
            Bool_t SetSignature(const std::string& signature);

            const std::string& GetReturn() const;

            const std::vector< std::string >& GetParameters() const;
            const std::string& GetParameter(UInt_t parNum) const;

        protected:
            Bool_t ParseSignature(const std::string& signature); // called from constructor; do not make virtual

            std::string fSignature;

            std::string fReturn;
            std::vector< std::string > fParameters;
    };

    inline const std::string& KTSignalSlotSignature::GetSignature() const
    {
        return fSignature;
    }

    inline Bool_t KTSignalSlotSignature::SetSignature(const std::string& signature)
    {
        if (ParseSignature(signature))
        {
            fSignature = signature;
            return true;
        }
        return false;
    }

    inline const std::string& KTSignalSlotSignature::GetReturn() const
    {
        return fReturn;
    }

    inline const std::vector< std::string >& KTSignalSlotSignature::GetParameters() const
    {
        return fParameters;
    }

    inline const std::string& KTSignalSlotSignature::GetParameter(UInt_t parNum) const
    {
        if (parNum > fParameters.size())
        {
            throw std::out_of_range("Requested parameter number does not exist");
        }
        return fParameters[parNum];
    }

} /* namespace Katydid */
#endif /* KTSIGNALSLOTSIGNATURE_HH_ */
