/*
 * KTSignalWrapper.hh
 *
 *  Created on: Aug 8, 2012
 *      Author: nsoblath
 */

#ifndef KTSIGNAL_HH_
#define KTSIGNAL_HH_

#include "KTConnection.hh"

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/utility.hpp>

#include <iostream>
#include <typeinfo>
//#include <set>

namespace Katydid
{
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

                    //virtual KTConnection Connect(void* slotPtr) = 0;
                    void TestFuncNonVirtual() {std::cout << "signal, non virtual: base" << std::endl;}
                    virtual void TestFuncVirtual() {std::cout << "signal, virtual: base" << std::endl;}
            };

            template< typename XSignature >
            class KTSpecifiedInternalSignalWrapper : public KTInternalSignalWrapper, public boost::noncopyable
            {
                public:
                    //typedef boost::signals2::signal< XSignature > signal_type;
                    typedef XSignature signal_type;
                    KTSpecifiedInternalSignalWrapper(signal_type* signalPtr) : fSignal(signalPtr)
                    {
                        std::cout << "internal signal wrapper: signature is " << typeid(signalPtr).name() << std::endl;
                    }
                    virtual ~KTSpecifiedInternalSignalWrapper() {}
                    template< typename XSlotType >
                    KTConnection Connect(XSlotType& slot)
                    {
                        return fSignal->connect(slot);
                    }
                    void TestFuncNonVirtual() {std::cout << "signal, non virtual: derived" << std::endl;}
                    virtual void TestFuncVirtual() {std::cout << "signal, virtual: derived" << std::endl;}

                    signal_type* GetSignal() const
                    {
                        return fSignal;
                    }
                private:
                    signal_type* fSignal; //not owned by this KTSignalWrapper
            };

            KTInternalSignalWrapper* fSignalWrapper;

        public:
            //KTSignalWrapper(boost::signals2::signal< XSignature >* signalPtr) : fSignalWrapper(NULL)
            template< typename XSignature >
            KTSignalWrapper(XSignature* signalPtr) : fSignalWrapper(NULL)
            {
                fSignalWrapper = new KTSpecifiedInternalSignalWrapper< XSignature >(signalPtr);
            }
            ~KTSignalWrapper();

            //KTConnection Connect(const typename boost::signals2::signal< XSignature >::slot_type& slot)
            template< typename XSignature >
            KTConnection Connect(XSignature& slot)
            {
                KTSpecifiedInternalSignalWrapper< XSignature >* derivedSignalWrapper = dynamic_cast< KTSpecifiedInternalSignalWrapper< XSignature >* >(fSignalWrapper);
                if (derivedSignalWrapper == NULL)
                {
                    std::cout << "unable to cast signal; throwing exception" << std::endl;
                    throw SignalException("Incorrect cast from KTInternalSignalWrapper* to derived type");
                }
                return derivedSignalWrapper->Connect(slot);
            }

            template< typename XSignature >
            void PrintTestFunc()
            {
                std::cout << "### Without Casting ###" << std::endl;
                fSignalWrapper->TestFuncNonVirtual();
                fSignalWrapper->TestFuncVirtual();

                std::cout << "### Casting ###" << std::endl;
                KTSpecifiedInternalSignalWrapper< XSignature >* derivedSignalWrapper = dynamic_cast< KTSpecifiedInternalSignalWrapper< XSignature >* >(fSignalWrapper);
                if (derivedSignalWrapper == NULL)
                {
                    std::cout << "unable to cast signal; throwing exception" << std::endl;
                    throw SignalException("Incorrect cast from KTInternalSignalWrapper* to derived type");
                }

                std::cout << "### With Casting ###" << std::endl;
                derivedSignalWrapper->TestFuncNonVirtual();
                derivedSignalWrapper->TestFuncVirtual();

                return;
            }

        private:
            KTSignalWrapper();

            KTInternalSignalWrapper* GetInternal() const;

    };

    inline KTSignalWrapper::KTInternalSignalWrapper* KTSignalWrapper::GetInternal() const
    {
        return fSignalWrapper;
    }

} /* namespace Katydid */
#endif /* KTSIGNAL_HH_ */
