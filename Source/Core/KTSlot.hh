/*
 * KTSlot.hh
 *
 *  Created on: Jan 13, 2013
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

#include "KTEvent.hh"
#include "KTSignal.hh"
#include "KTSignalWrapper.hh"

#include <boost/shared_ptr.hpp>

#include <deque>
#include <string>

namespace Katydid
{
    class KTSlot
    {
        public:
            KTSlot();
            virtual ~KTSlot();
    };


    /*!
     @class KTSlotOneArg
     @author N. S. Oblath

     @brief Creates a slot function that calls a Processor member function taking one argument.

     @details
     Usage:
     To use this slot type your processor must have a member function with the signature void (ArgumentType).

     In your Processor's header add a member variable of type KTSlotOneArg< ProcessorType, ArgumentType >.

     Initialize the slot with the address of the Processor.

     In your Processor's constructor, call the KTSlotOneArg::RegisterSlot function, passing the following as arguments:
     - slot name
     - pointer to the member function that should be called (e.g. &KTMyProcessor::CallThisFunc)
     - the function signature as a string
    */
    template< class XProcessorType, class XArgumentType >
    class KTSlotOneArg : public KTSlot
    {
        public:
            typedef XProcessorType processor_type;
            typedef XArgumentType argument_type;

        public:
            KTSlotOneArg(XProcessorType* proc);
            virtual ~KTSlotOneArg();

            void operator()(XArgumentType arg);

            void RegisterSlot(const std::string& name, void (XProcessorType::*func)(XArgumentType), const std::string& signature);

        protected:
            XProcessorType* fProcessor;

            void (XProcessorType::*fFuncPtr)(XArgumentType);
    };

    template< class XProcessorType, class XArgumentType >
    KTSlotOneArg< XProcessorType, XArgumentType >::KTSlotOneArg(XProcessorType* proc) :
            KTSlot(),
            fProcessor(proc),
            fFuncPtr(NULL)
    {
    }

    template< class XProcessorType, class XArgumentType >
    KTSlotOneArg< XProcessorType, XArgumentType >::~KTSlotOneArg()
    {
    }

    template< class XProcessorType, class XArgumentType >
    void KTSlotOneArg< XProcessorType, XArgumentType >::operator()(XArgumentType arg)
    {
        (fProcessor->*fFuncPtr)(arg);
        return;
    }

    template< class XProcessorType, class XArgumentType >
    void KTSlotOneArg< XProcessorType, XArgumentType >::RegisterSlot(const std::string& name, void (XProcessorType::*func)(XArgumentType), const std::string& signature)
    {
        fFuncPtr = func;
        fProcessor->template RegisterSlot< XProcessorType, void, XArgumentType >(name, fProcessor, fFuncPtr, signature);
        return;
    }


    /*!
     @class KTDataSlotOneArg
     @author N. S. Oblath

     @brief Creates a slot function that calls a Processor member function taking one Data type as an argument.

     @details
     Usage:
     To use this slot type your processor must have a member function with the signature OutputDataType (const InputDataType*).
     OutputDataType and InputDataType should be the data types themselves, without any const modifiers, ampersands, or asterisks.

     In your Processor's header add a member variable of type KTDataSlotOneArg< ProcessorType, OutputDataType, InputDataType >.

     Initialize the slot with the address of the Processor.

     In your Processor's constructor, call the KTDataSlotOneArg::RegisterSlot function, passing the following as arguments:
     - slot name
     - pointer to the member function that should be called (e.g. &KTMyProcessor::CallThisFunc)
     - the function signature as a string

     Also in the Processor's constructor, if you want a signal called upon creation of the new data object, assign the address of the signal with KTDataSlotOneArg::SetSignal.
    */
    template< class XProcessorType, class XReturnDataType, class XDataType >
    class KTDataSlotOneArg : public KTSlot
    {
        public:
            typedef XProcessorType processor_type;
            typedef XReturnDataType return_type;
            typedef XDataType argument_type;

            typedef KTDataSignal< XProcessorType, XReturnDataType > signal_type;

        public:
            KTDataSlotOneArg(XProcessorType* proc);
            virtual ~KTDataSlotOneArg();

            void operator()(const XDataType* data);

            void RegisterSlot(const std::string& name, XReturnDataType* (XProcessorType::*func)(const XDataType*), const std::string& signature);

            void SetSignal(signal_type* signalPtr);

        protected:
            XProcessorType* fProcessor;

            XReturnDataType* (XProcessorType::*fFuncPtr)(const XDataType*);

            signal_type* fSignalPtr;
    };

    template< class XProcessorType, class XReturnDataType, class XDataType >
    KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >::KTDataSlotOneArg(XProcessorType* proc) :
            KTSlot(),
            fProcessor(proc),
            fFuncPtr(NULL),
            fSignalPtr(NULL)
    {
    }

    template< class XProcessorType, class XReturnType, class XDataType >
    KTDataSlotOneArg< XProcessorType, XReturnType, XDataType >::~KTDataSlotOneArg()
    {
    }

    template< class XProcessorType, class XReturnDataType, class XDataType >
    void KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >::operator()(const XDataType* data)
    {
        // Standard data slot pattern:
        // First the processor acts on the data, returning a new data object
        XReturnDataType* newData = (fProcessor->*fFuncPtr)(data);
        // Extract the event pointer from the old data, and set the event in the new data (even if it's NULL
        KTEvent* event = data->GetEvent();
        newData->SetEvent(event);
        // If the event isn't NULL, add the new data to the event
        if (event != NULL)
        {
            event->AddData(newData);
        }
        // If there's a signal pointer, emit the signal
        if (fSignalPtr != NULL)
        {
            fSignalPtr->EmitSignal(newData);
        }
        return;
    }

    template< class XProcessorType, class XReturnDataType, class XDataType >
    void KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >::RegisterSlot(const std::string& name, XReturnDataType* (XProcessorType::*func)(const XDataType*), const std::string& signature)
    {
        fFuncPtr = func;
        fProcessor->RegisterSlot(name, this, &KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >::operator(), signature);
        return;
    }

    template< class XProcessorType, class XReturnDataType, class XDataType >
    void KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >::SetSignal(signal_type* signalPtr)
    {
        fSignalPtr = signalPtr;
        return;
    }



    /*!
     @class KTEventSlotOneArg
     @author N. S. Oblath

     @brief Creates a slot function that calls a Processor member function taking an event pointer as the argument.

     @details
     KTEventSlotOneArg uses a type erasure pattern to hold and use Data slots that deal with different types of input and output data.
     Pointers to the individual Data slots are held in a std::deque after wrapping them in a KTInternalSlotFunction object.
     The specified slot type is hidden in the derived KTSpecifiedInternalSlotFunction class.
     KTEventSlotOneArg uses the Data slot via the generic function KTInternalSlotFunction::FindAndProcessData.

     Usage:
     To use this slot type your processor must one ore more KTDataSlotOneArg members that you wish to use.

     In your Processor's header add a member variable of type KTEventSlotOneArg< ProcessorType >.

     Initialize the slot with the address of the Processor.

     In your Processor's constructor, call the KTEventSlotOneArg::RegisterSlot function, passing the following as arguments:
     - slot name
     - pointer to the input data name

     Also in the Processor's constructor, call the KTEventSlotOneArg::AddDataSlot function for each data slot you wish to be called.
     The argument for this function is the address of the KTDataSlotOneArg object.
    */
    template< class XProcessorType >
    class KTEventSlotOneArg : public KTSlot
    {
        public:
            typedef XProcessorType processor_type;

        private:
            class KTInternalSlotFunction
            {
                public:
                    KTInternalSlotFunction() {}
                    virtual ~KTInternalSlotFunction() {}

                    virtual Bool_t FindAndProcessData(const std::string* dataName, boost::shared_ptr<KTEvent> event) = 0;
            };

            template< class XReturnDataType, class XDataType >
            class KTSpecifiedInternalSlotFunction : public KTInternalSlotFunction, public boost::noncopyable
            {
                public:
                    KTSpecifiedInternalSlotFunction(KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >* dataSlotPtr) :
                        KTInternalSlotFunction(),
                        fDataSlotPtr(dataSlotPtr)
                    {}
                    virtual ~KTSpecifiedInternalSlotFunction() {}

                    virtual Bool_t FindAndProcessData(const std::string* dataName, boost::shared_ptr<KTEvent> event)
                    {
                        const XDataType* inputData = event->GetData< XDataType >(*dataName);
                        if (inputData != NULL)
                        {
                            (*fDataSlotPtr)(inputData);
                            return true;
                        }
                        return false;
                    }

                private:
                    KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >* fDataSlotPtr;
            };

        public:
            KTEventSlotOneArg(XProcessorType* proc);
            virtual ~KTEventSlotOneArg();

            void operator()(boost::shared_ptr<KTEvent> event);

            void RegisterSlot(const std::string& slotName, const std::string* inputDataName);

            template< class XReturnDataType, class XDataType >
            void AddDataSlot(KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >* dataSlotPtr);

        protected:
            XProcessorType* fProcessor;

            std::deque< KTInternalSlotFunction* > fSlotFunctions;

            const std::string* fInputDataName;

    };

    template< class XProcessorType >
    KTEventSlotOneArg< XProcessorType >::KTEventSlotOneArg(XProcessorType* proc) :
            KTSlot(),
            fProcessor(proc),
            fSlotFunctions(),
            fInputDataName(NULL)
    {
    }

    template< class XProcessorType >
    KTEventSlotOneArg< XProcessorType >::~KTEventSlotOneArg()
    {
        while (! fSlotFunctions.empty())
        {
            delete fSlotFunctions.back();
            fSlotFunctions.pop_back();
        }
    }

    template< class XProcessorType >
    void KTEventSlotOneArg< XProcessorType >::operator()(boost::shared_ptr<KTEvent> event)
    {
        for (typename std::deque< KTInternalSlotFunction* >::iterator slotIt = fSlotFunctions.begin(); slotIt != fSlotFunctions.end(); slotIt++)
        {
            if ((*slotIt)->FindAndProcessData(fInputDataName, event))
                break;
        }
        return;
    }

    template< class XProcessorType >
    template< class XReturnDataType, class XDataType >
    void KTEventSlotOneArg< XProcessorType >::AddDataSlot(KTDataSlotOneArg< XProcessorType, XReturnDataType, XDataType >* dataSlotPtr)
    {
        KTInternalSlotFunction* newSlotFcn = new KTSpecifiedInternalSlotFunction< XReturnDataType, XDataType >(dataSlotPtr);
        fSlotFunctions.push_back(newSlotFcn);
        return;
    }

    template< class XProcessorType >
    void KTEventSlotOneArg< XProcessorType >::RegisterSlot(const std::string& slotName, const std::string* inputDataName)
    {
        fInputDataName = inputDataName;
        fProcessor->RegisterSlot(slotName, this, &KTEventSlotOneArg< XProcessorType >::operator(), "void (boost::shared_ptr<KTEvent>)");
        return;
    }


} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
