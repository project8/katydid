/*
 * KTSlot.hh
 *
 *  Created on: Jan 13, 2013
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

#include <string>

namespace Katydid
{
    class KTSlot
    {
        public:
            KTSlot();
            virtual ~KTSlot();
    };


    template< class XProcessorType, class XReturnType, class XArgumentType >
    class KTSlotOneArg : public KTSlot
    {
        public:
            typedef XProcessorType processor_type;
            typedef XReturnType return_type;
            typedef XArgumentType argument_type;

        public:
            KTSlotOneArg(XProcessorType* proc);
            virtual ~KTSlotOneArg();

            XReturnType operator()(XArgumentType arg);

            void RegisterSlot(const std::string& name, XReturnType (XProcessorType::*func)(XArgumentType), const std::string& signature);

        protected:
            XProcessorType* fProcessor;

            XReturnType (XProcessorType::*fFuncPtr)(XArgumentType);
    };

    template< class XProcessorType, class XReturnType, class XArgumentType >
    KTSlotOneArg< XProcessorType, XReturnType, XArgumentType >::KTSlotOneArg(XProcessorType* proc) :
            KTSlot(),
            fProcessor(proc),
            fFuncPtr(NULL)
    {
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    KTSlotOneArg< XProcessorType, XReturnType, XArgumentType >::~KTSlotOneArg()
    {
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    XReturnType KTSlotOneArg< XProcessorType, XReturnType, XArgumentType >::operator()(XArgumentType arg)
    {
        return fProcessor->fFuncPtr(arg);
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    void KTSlotOneArg< XProcessorType, XReturnType, XArgumentType >::RegisterSlot(const std::string& name, XReturnType (XProcessorType::*func)(XArgumentType), const std::string& signature)
    {
        fFuncPtr = func;
        fProcessor->template RegisterSlot< XProcessorType, void, XArgumentType >(name, fProcessor, fFuncPtr, signature);
        return;
    }



    template< class XProcessorType, class XReturnType, class XArgumentType >
    class KTDataSlotOneArg : public KTSlot
    {
        public:
            typedef XProcessorType processor_type;
            typedef XReturnType return_type;
            typedef XArgumentType argument_type;

        public:
            KTDataSlotOneArg(XProcessorType* proc);
            virtual ~KTDataSlotOneArg();

            XReturnType operator()(XArgumentType arg);

            void RegisterSlot(const std::string& name, XReturnType (XProcessorType::*func)(XArgumentType), const std::string& signature);

        protected:
            XProcessorType* fProcessor;

            XReturnType (XProcessorType::*fFuncPtr)(XArgumentType);
    };

    template< class XProcessorType, class XReturnType, class XArgumentType >
    KTDataSlotOneArg< XProcessorType, XReturnType, XArgumentType >::KTDataSlotOneArg(XProcessorType* proc) :
            KTSlot(),
            fProcessor(proc),
            fFuncPtr(NULL)
    {
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    KTDataSlotOneArg< XProcessorType, XReturnType, XArgumentType >::~KTDataSlotOneArg()
    {
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    XReturnType KTDataSlotOneArg< XProcessorType, XReturnType, XArgumentType >::operator()(XArgumentType arg)
    {
        const XReturnType& retVal = fProcessor->fFuncPtr(arg);
        KTEvent* event = retVal->GetEvent();
        if (event != NULL)
        {
            retVal->SetEvent(event);
            event->AddData(retVal);
        }
        return retVal;
    }

    template< class XProcessorType, class XReturnType, class XArgumentType >
    void KTDataSlotOneArg< XProcessorType, XReturnType, XArgumentType >::RegisterSlot(const std::string& name, XReturnType (XProcessorType::*func)(XArgumentType), const std::string& signature)
    {
        fFuncPtr = func;
        fProcessor->template RegisterSlot< XProcessorType, void, XArgumentType >(name, fProcessor, fFuncPtr, signature);
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
