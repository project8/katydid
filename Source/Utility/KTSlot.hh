/*
 * KTSlot.hh
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

#include "KTConnection.hh"
#include "KTSignal.hh"

#include <boost/function.hpp>
#include <boost/signals2.hpp>

#include <typeinfo>
#include <iostream>

namespace Katydid
{
    template< typename XFuncSig >
    struct RawFuncSignatureContainer
    {
        typedef XFuncSig function_type;
        typedef boost::signals2::signal< XFuncSig > signal_type;
    };


    class SlotException : public std::logic_error
    {
        public:
            SlotException(std::string const& why);
    };

    class KTSlotWrapper : public boost::noncopyable
    {
        private:
            class KTInternalSlotWrapper
            {
                public:
                    KTInternalSlotWrapper() {}
                    virtual ~KTInternalSlotWrapper() {}

                    virtual KTConnection Connect(KTSignalWrapper* signalWrap) = 0;
            };

            template< typename XSignature, typename XTypeContainer >
            class KTSpecifiedInternalSlotWrapper : public KTInternalSlotWrapper, public boost::noncopyable
            {
                public:
                    KTSpecifiedInternalSlotWrapper(XSignature* funcPtr, XTypeContainer* typeCont=NULL) :
                            fSlot(funcPtr)
                    {
                        std::cout << "from internal slot wrapper: signature type is " << typeid(funcPtr).name() << std::endl;
                    }
                    virtual ~KTSpecifiedInternalSlotWrapper()
                    {
                        //delete fSlot;
                    }

                    virtual KTConnection Connect(KTSignalWrapper* signalWrap)
                    {
                        typedef KTSignalWrapper::KTInternalSignalWrapper SignalWrapperBase;
                        typedef KTSignalWrapper::KTSpecifiedInternalSignalWrapper< typename XTypeContainer::signal_type > SignalWrapper;

                        SignalWrapperBase* internalSignalWrap = signalWrap->GetInternal();
                        SignalWrapper* derivedSignalWrapper = dynamic_cast< SignalWrapper* >(internalSignalWrap);
                        if (derivedSignalWrapper == NULL)
                        {
                            std::cout << "unable to cast signal; throwing exception" << std::endl;
                            throw SignalException("Incorrect cast from KTInternalSignalWrapper* to derived type");
                        }
                        return derivedSignalWrapper->GetSignal()->connect(*fSlot);
                    }

                private:
                    XSignature* fSlot; // is owned by this KTSlot
            };

            KTInternalSlotWrapper* fSlotWrapper;

        public:
            template< typename XSignature, typename XTypeContainer >
            KTSlotWrapper(XSignature* signalPtr, XTypeContainer* typeCont) :
                    fSlotWrapper(new KTSpecifiedInternalSlotWrapper< XSignature, XTypeContainer >(signalPtr, typeCont)),
                    fConnection()
            {}
            ~KTSlotWrapper();

            template< typename XSignature >
            XSignature& GetSlot()
            {
                KTSpecifiedInternalSlotWrapper< XSignature, KTConnection >* derivedSlotWrapper = dynamic_cast< KTSpecifiedInternalSlotWrapper< XSignature, KTConnection >* >(fSlotWrapper);
                if (derivedSlotWrapper == NULL)
                {
                    std::cout << "unable to cast slot; throwing exception" << std::endl;
                    throw SlotException("Incorrect cast from KTInternalSlotWrapper* to derived type");
                }
                return derivedSlotWrapper->GetSlot();
            }

            template< typename XSignature >
            void PrintTestFunc()
            {
                std::cout << "### Without Casting ###" << std::endl;
                fSlotWrapper->TestFuncNonVirtual();
                fSlotWrapper->TestFuncVirtual();

                std::cout << "### Casting ###" << std::endl;
                KTSpecifiedInternalSlotWrapper< XSignature, KTConnection >* derivedSlotWrapper = static_cast< KTSpecifiedInternalSlotWrapper< XSignature, KTConnection >* >(fSlotWrapper);
                if (derivedSlotWrapper == NULL)
                {
                    std::cout << "unable to cast slot; throwing exception" << std::endl;
                    throw SlotException("Incorrect cast from KTInternalSlotWrapper* to derived type");
                }

                std::cout << "### With Casting ###" << std::endl;
                derivedSlotWrapper->TestFuncNonVirtual();
                derivedSlotWrapper->TestFuncVirtual();
                derivedSlotWrapper->TestDerivedOnly();

                return;
            }


        private:
            KTSlotWrapper();


        public:
            void SetConnection(KTConnection conn);
            void SetConnection(KTSignalWrapper* signalWrap);
            void Disconnect();

        private:
            KTConnection fConnection;

    };

    inline void KTSlotWrapper::SetConnection(KTConnection conn)
    {
        fConnection = conn;
        return;
    }

    inline void KTSlotWrapper::Disconnect()
    {
        fConnection.disconnect();
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
