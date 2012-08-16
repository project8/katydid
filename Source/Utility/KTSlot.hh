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

namespace Katydid
{
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
                    {}
                    virtual ~KTSpecifiedInternalSlotWrapper()
                    {
                        //delete fSlot;
                    }

                    virtual KTConnection Connect(KTSignalWrapper* signalWrap)
                    {
                        typedef KTSignalWrapper::KTInternalSignalWrapper SignalWrapperBase;
                        typedef KTSignalWrapper::KTSpecifiedInternalSignalWrapper< typename XTypeContainer::signal > SignalWrapper;

                        SignalWrapperBase* internalSignalWrap = signalWrap->GetInternal();
                        SignalWrapper* derivedSignalWrapper = dynamic_cast< SignalWrapper* >(internalSignalWrap);
                        if (derivedSignalWrapper == NULL)
                        {
                            throw SignalException("Unable to cast from KTInternalSignalWrapper* to derived type");
                        }
                        return derivedSignalWrapper->GetSignal()->connect(*fSlot);
                    }

                private:
                    XSignature* fSlot; // is owned by this KTSlot
            };

        public:
            template< typename XSignature, typename XTypeContainer >
            KTSlotWrapper(XSignature* signalPtr, XTypeContainer* typeCont);
            ~KTSlotWrapper();

        private:
            KTSlotWrapper();

            KTInternalSlotWrapper* fSlotWrapper;


        public:
            void SetConnection(KTConnection conn);
            void SetConnection(KTSignalWrapper* signalWrap);
            void Disconnect();

        private:
            KTConnection fConnection;

    };

    template< typename XSignature, typename XTypeContainer >
    KTSlotWrapper::KTSlotWrapper(XSignature* signalPtr, XTypeContainer* typeCont) :
            fSlotWrapper(new KTSpecifiedInternalSlotWrapper< XSignature, XTypeContainer >(signalPtr, typeCont)),
            fConnection()
    {}

    inline void KTSlotWrapper::SetConnection(KTConnection conn)
    {
        fConnection = conn;
        return;
    }

    inline void KTSlotWrapper::SetConnection(KTSignalWrapper* signalWrap)
    {
        fConnection = this->fSlotWrapper->Connect(signalWrap);
        return;
    }

    inline void KTSlotWrapper::Disconnect()
    {
        fConnection.disconnect();
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
