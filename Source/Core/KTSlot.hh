/*
 * KTSlot.hh
 *
 *  Created on: Jan 13, 2013
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

#include "KTData.hh"
#include "KTLogger.hh"
#include "KTSignal.hh"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <string>

namespace Katydid
{
    /*
    class KTSlot
    {
        public:
            KTSlot();
            virtual ~KTSlot();
    };
    */

    /*!
     @class KTSlotOneArg
     @author N. S. Oblath

     @brief Creates a slot that calls a member function of the func_owner_type object, taking one argument.

     @details
     Usage:
     To use this slot type the function to be called by the slot must exist in an object of type FuncOwnerType.
     The function should have the signature ReturnType (ArgumentType).

     In your Processor's header add a member variable of type KTSlotOneArg< ProcessorType, ArgumentType, ReturnType >.
     The variable may be private.

     Initialize the slot with the name of the slot, the address of the owner of the slot function, and the function pointer.
     Optionally, if the Processor is separate from the owner of the slot function, the Processor address is specified as the second argument to the constructor.
    */
    template< class XFuncOwnerType, class XArgumentType, class XReturnType >
    class KTSlotOneArg// : public KTSlot
    {
        public:
            typedef XFuncOwnerType func_owner_type;
            typedef XArgumentType argument_type;
            typedef XReturnType return_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            KTSlotOneArg(const std::string& name, func_owner_type* owner, return_type (func_owner_type::*func)(argument_type));
            /// Constructor for the case where the processor and the object with the function that will be called are different
            KTSlotOneArg(const std::string& name, KTProcessor* proc, func_owner_type* owner, return_type (func_owner_type::*func)(argument_type));
            virtual ~KTSlotOneArg();

            return_type operator()(argument_type arg);

        protected:
            func_owner_type* fOwner;

            return_type (func_owner_type::*fFuncPtr)(argument_type);
    };

    template< class XFuncOwnerType, class XArgumentType, class XReturnType >
    KTSlotOneArg< XFuncOwnerType, XArgumentType, XReturnType >::KTSlotOneArg(const std::string& name, func_owner_type* owner, return_type (func_owner_type::*func)(argument_type)) :
            //KTSlot(),
            fOwner(owner),
            fFuncPtr(func)
    {
        owner->RegisterSlot(name, owner, fFuncPtr, "");
    }

    template< class XFuncOwnerType, class XArgumentType, class XReturnType >
    KTSlotOneArg< XFuncOwnerType, XArgumentType, XReturnType >::KTSlotOneArg(const std::string& name, KTProcessor* proc, func_owner_type* owner, return_type (func_owner_type::*func)(argument_type)) :
            //KTSlot(),
            fOwner(owner),
            fFuncPtr(func)
    {
        proc->RegisterSlot(name, owner, fFuncPtr, "");
    }

    template< class XProcessorType, class XArgumentType, class XReturnType >
    KTSlotOneArg< XProcessorType, XArgumentType, XReturnType >::~KTSlotOneArg()
    {
    }

    template< class XProcessorType, class XArgumentType, class XReturnType >
    XReturnType KTSlotOneArg< XProcessorType, XArgumentType, XReturnType >::operator()(argument_type arg)
    {
        return (fOwner->*fFuncPtr)(arg);
    }


    /*!
     @class KTDataSlotOneArg
     @author N. S. Oblath

     @brief Creates a slot that takes a boost::shared_ptr<Data> object as the argument; the function that gets called should take DataType& as its argument.

     @details
     Usage:
     This slot type adds the slot function (signature void (boost::shared_ptr<KTData>).
     Your processor (or, optionally, a different object) must have a member function with the signature Bool_t (DataType&).
     The slot function checks that the provided KTData object contains data of type DataType, and then calls the member function.

     In your Processor's header add a member variable of type KTSlotOneArg< ProcessorType, DataType >.
     The variable may be private.

     Initialize the slot with the name of the slot, the address of the owner of the slot function, and the function pointer.
     Optionally, if the Processor is separate from the owner of the slot function, the Processor address is specified as the second argument to the constructor.
    */
    //template< class XFuncOwnerType, class XDataType >
    template< class XDataType >
    class KTSlotDataOneType// : public KTSlot
    {
        public:
            //typedef XFuncOwnerType func_owner_type;
            typedef XDataType data_type;
            typedef KTSlotDataOneType< data_type > func_owner_type;
            typedef boost::shared_ptr< KTData > argument_type;
            typedef Bool_t return_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotDataOneType(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(XDataType&), KTSignalData* signalPtr=NULL);
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotDataOneType(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(XDataType&), KTSignalData* signalPtr=NULL);
            virtual ~KTSlotDataOneType();

            void operator()(boost::shared_ptr< KTData > data);

        protected:
            boost::function< Bool_t (XDataType&) > fFunc;

            //func_owner_type* fOwner;

            //Bool_t (func_owner_type::*fFuncPtr)(XDataType&);

            KTSignalData* fSignalPtr;
    };

    template< class XDataType >
    template< class XFuncOwnerType >
    KTSlotDataOneType< XDataType >::KTSlotDataOneType(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(XDataType&), KTSignalData* signalPtr) :
            //KTSlotOneArg< func_owner_type, boost::shared_ptr< KTData >, Bool_t >(name, owner, func),
            fFunc(boost::bind(func, owner, _1)),
            //fOwner(owner),
            //fFuncPtr(func),
            fSignalPtr(signalPtr)
    {
        owner->RegisterSlot(name, this, &KTSlotDataOneType::operator(), "");
    }

    template< class XDataType >
    template< class XFuncOwnerType >
    KTSlotDataOneType< XDataType >::KTSlotDataOneType(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(XDataType&), KTSignalData* signalPtr) :
            //KTSlotOneArg< func_owner_type, boost::shared_ptr< KTData >, Bool_t >(name, proc, owner, func),
            fFunc(boost::bind(func, owner, _1)),
            //fOwner(owner),
            //fFuncPtr(func),
            fSignalPtr(signalPtr)
    {
        proc->RegisterSlot(name, this, &KTSlotDataOneType::operator(), "");
    }

    template< class XDataType >
    KTSlotDataOneType< XDataType >::~KTSlotDataOneType()
    {
    }

    template< class XDataType >
    void KTSlotDataOneType< XDataType >::operator()(boost::shared_ptr< KTData > data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< data_type >())
        {
            return;
        }
        // Call the function
        //if (! (fOwner->*fFuncPtr)(data->Of< data_type >()))
        if (! fFunc(data->Of< data_type >()))
        {
            return;
        }
        // If there's a signal pointer, emit the signal
        if (fSignalPtr != NULL)
        {
            (*fSignalPtr)(data);
        }
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
