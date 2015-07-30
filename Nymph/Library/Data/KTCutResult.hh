/*
 * KTCutResult.hh
 *
 *  Created on: Sept 19, 2014
 *      Author: nsoblath
 */

#ifndef KTCUTRESULT_HH_
#define KTCUTRESULT_HH_

#include "KTExtensibleStruct.hh"

#include "KTMemberVariable.hh"

#include <string>

namespace Nymph
{
    class KTCutResultCore
    {
        public:
            KTCutResultCore() :
                    fState(false)
            {}
            virtual ~KTCutResultCore() {}

            virtual const std::string& Name() const = 0;

            MEMBERVARIABLE_PROTECTED(bool, State);
    };

    typedef KTExtensibleStructCore< KTCutResultCore > KTCutResult;

    template< class XDerivedType >
    class KTExtensibleCutResult : public KTExtensibleStruct< XDerivedType, KTCutResultCore >
    {
        public:
            KTExtensibleCutResult() {}
            virtual ~KTExtensibleCutResult() {}

            const std::string& Name() const;
    };

    template< class XDerivedType >
    inline const std::string& KTExtensibleCutResult< XDerivedType >::Name() const
    {
        return XDerivedType::sName;
    }

} /* namespace Nymph */

#endif /* KTCUTRESULT_HH_ */
