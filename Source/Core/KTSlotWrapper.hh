/*
 * KTSlotWrapper.hh
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#ifndef KTSLOTWRAPPER_HH_
#define KTSLOTWRAPPER_HH_

#include "KTConnection.hh"
#include "KTSignalWrapper.hh"

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

                    virtual KTConnection Connect(KTSignalWrapper* signalWrap, int groupNum=-1) = 0;
            };

            template< typename XSignature, typename XTypeContainer >
            class KTSpecifiedInternalSlotWrapper : public KTInternalSlotWrapper, public boost::noncopyable
            {
                public:
                    KTSpecifiedInternalSlotWrapper(XSignature* funcPtr, XTypeContainer* typeCont=NULL) :
                            fSlot(funcPtr)
                    {
                        (void)typeCont; // to suppress warnings
                    }
                    virtual ~KTSpecifiedInternalSlotWrapper()
                    {
                        delete fSlot;
                    }

                    virtual KTConnection Connect(KTSignalWrapper* signalWrap, int groupNum=-1)
                    {
                        typedef KTSignalWrapper::KTInternalSignalWrapper SignalWrapperBase;
                        typedef KTSignalWrapper::KTSpecifiedInternalSignalWrapper< typename XTypeContainer::signal > SignalWrapper;

                        SignalWrapperBase* internalSignalWrap = signalWrap->GetInternal();
                        SignalWrapper* derivedSignalWrapper = dynamic_cast< SignalWrapper* >(internalSignalWrap);
                        if (derivedSignalWrapper == NULL)
                        {
                            throw SignalException("In KTSpecifiedInternalSlotWrapper::Connect:\nUnable to cast from KTInternalSignalWrapper* to derived type.");
                        }
                        if (groupNum >= 0)
                        {
                            return derivedSignalWrapper->GetSignal()->connect(groupNum, *fSlot);
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
            void SetConnection(KTSignalWrapper* signalWrap, int groupNum=-1);
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

    inline void KTSlotWrapper::SetConnection(KTSignalWrapper* signalWrap, int groupNum)
    {
        fConnection = this->fSlotWrapper->Connect(signalWrap, groupNum);
        return;
    }

    inline void KTSlotWrapper::Disconnect()
    {
        fConnection.disconnect();
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOTWRAPPER_HH_ */
