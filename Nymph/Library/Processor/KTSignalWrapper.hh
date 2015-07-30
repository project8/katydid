/*
 * KTSignalWrapper.hh
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#ifndef KTSIGNALWRAPPER_HH_
#define KTSIGNALWRAPPER_HH_

#include <boost/signals2.hpp>
#include <boost/utility.hpp>

#include <iostream>

namespace Nymph
{
    template< typename Signature >
    struct KTSignalConcept
    {
        typedef Signature signature;
        typedef boost::signals2::signal< Signature > signal;
        typedef typename boost::signals2::signal< Signature >::slot_type slot_type;
    };

    class SignalException : public std::logic_error
    {
        public:
            SignalException(std::string const& why);
    };

    class KTSignalWrapper : public boost::noncopyable
    {
        public:
            friend class KTSlotWrapper;

        private:
            class KTInternalSignalWrapper
            {
                public:
                    KTInternalSignalWrapper() {}
                    virtual ~KTInternalSignalWrapper() {}
            };

            template< typename XSignature >
            class KTSpecifiedInternalSignalWrapper : public KTInternalSignalWrapper, public boost::noncopyable
            {
                public:
                    KTSpecifiedInternalSignalWrapper(XSignature* signalPtr) : fSignal(signalPtr)
                    {}
                    virtual ~KTSpecifiedInternalSignalWrapper() {}

                    XSignature* GetSignal() const
                    {
                        return fSignal;
                    }
                private:
                    XSignature* fSignal; //not owned by this KTSignalWrapper
            };

        public:
            template< typename XSignature >
            KTSignalWrapper(XSignature* signalPtr);
            ~KTSignalWrapper();

        private:
            KTSignalWrapper();

            KTInternalSignalWrapper* GetInternal() const;

            KTInternalSignalWrapper* fSignalWrapper;

    };

    template< typename XSignature >
    KTSignalWrapper::KTSignalWrapper(XSignature* signalPtr) :
            fSignalWrapper(NULL)
    {
        fSignalWrapper = new KTSpecifiedInternalSignalWrapper< XSignature >(signalPtr);
    }

    inline KTSignalWrapper::KTInternalSignalWrapper* KTSignalWrapper::GetInternal() const
    {
        return fSignalWrapper;
    }

} /* namespace Nymph */
#endif /* KTSIGNALWRAPPER_HH_ */
