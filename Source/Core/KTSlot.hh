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
    KTLOGGER(slotlog, "katydid.core");

    template< typename Signature>
    class KTSlotNoArg
    {
        public:
            typedef boost::function< Signature > function_signature;
            typedef typename function_signature::result_type return_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotNoArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)());
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotNoArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)());
            virtual ~KTSlotNoArg();

            return_type operator()();

        protected:
            boost::function< Signature > fFunc;

    };

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
    template< typename Signature>
    class KTSlotOneArg
    {
        public:
            typedef boost::function< Signature > function_signature;
            typedef typename function_signature::result_type return_type;
            typedef typename function_signature::argument_type argument_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotOneArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(argument_type));
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotOneArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(argument_type));
            virtual ~KTSlotOneArg();

            return_type operator()(argument_type arg);

        protected:
            boost::function< Signature > fFunc;

    };

    template< typename Signature>
    class KTSlotTwoArg
    {
        public:
            typedef boost::function< Signature > function_signature;
            typedef typename function_signature::result_type return_type;
            typedef typename function_signature::first_argument_type first_argument_type;
            typedef typename function_signature::second_argument_type second_argument_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotTwoArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(first_argument_type, second_argument_type));
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotTwoArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(first_argument_type, second_argument_type));
            virtual ~KTSlotTwoArg();

            return_type operator()(first_argument_type arg1, second_argument_type arg2);

        protected:
            boost::function< Signature > fFunc;

    };



    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotNoArg< Signature >::KTSlotNoArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)()) :
            fFunc(boost::bind(func, owner, _1))
    {
        owner->RegisterSlot(name, this, &KTSlotNoArg::operator());
    }

    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotNoArg< Signature >::KTSlotNoArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)()) :
            fFunc(boost::bind(func, owner))
    {
        proc->RegisterSlot(name, this, &KTSlotNoArg::operator());
    }

    template< typename Signature>
    KTSlotNoArg< Signature >::~KTSlotNoArg()
    {
    }

    template< typename Signature>
    typename KTSlotNoArg< Signature >::return_type KTSlotNoArg< Signature >::operator()()
    {
        return fFunc();
    }




    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotOneArg< Signature >::KTSlotOneArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(argument_type)) :
            fFunc(boost::bind(func, owner, _1))
    {
        owner->RegisterSlot(name, this, &KTSlotOneArg::operator());
    }

    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotOneArg< Signature >::KTSlotOneArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(argument_type)) :
            fFunc(boost::bind(func, owner, _1))
    {
        proc->RegisterSlot(name, this, &KTSlotOneArg::operator());
    }

    template< typename Signature>
    KTSlotOneArg< Signature >::~KTSlotOneArg()
    {
    }

    template< typename Signature>
    typename KTSlotOneArg< Signature >::return_type KTSlotOneArg< Signature >::operator()(argument_type arg)
    {
        return fFunc(arg);
    }





    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotTwoArg< Signature >::KTSlotTwoArg(const std::string& name, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(first_argument_type, second_argument_type)) :
            fFunc(boost::bind(func, owner, _1, _2))
    {
        owner->RegisterSlot(name, this, &KTSlotTwoArg::operator());
    }

    template< typename Signature>
    template< class XFuncOwnerType >
    KTSlotTwoArg< Signature >::KTSlotTwoArg(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, return_type (XFuncOwnerType::*func)(first_argument_type, second_argument_type)) :
            fFunc(boost::bind(func, owner, _1, _2))
    {
        proc->RegisterSlot(name, this, &KTSlotTwoArg::operator());
    }

    template< typename Signature>
    KTSlotTwoArg< Signature >::~KTSlotTwoArg()
    {
    }

    template< typename Signature>
    typename KTSlotTwoArg< Signature >::return_type KTSlotTwoArg< Signature >::operator()(first_argument_type arg1, second_argument_type arg2)
    {
        return fFunc(arg1, arg2);
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
    template< class XDataType >
    class KTSlotDataOneType
    {
        public:
            typedef XDataType data_type;
            typedef boost::function< void (boost::shared_ptr< KTData >) > function_signature;
            typedef typename function_signature::result_type return_type;
            typedef typename function_signature::argument_type argument_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotDataOneType(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(data_type&), KTSignalData* signalPtr=NULL);
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotDataOneType(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(data_type&), KTSignalData* signalPtr=NULL);
            virtual ~KTSlotDataOneType();

            void operator()(boost::shared_ptr< KTData > data);

        protected:
            boost::function< Bool_t (data_type&) > fFunc;

            KTSignalData* fSignalPtr;
    };


    template< class XDataType1, class XDataType2 >
    class KTSlotDataTwoTypes
    {
        public:
            typedef XDataType1 first_data_type;
            typedef XDataType2 second_data_type;
            typedef boost::function< void (boost::shared_ptr< KTData >) > function_signature;
            typedef typename function_signature::result_type return_type;
            typedef typename function_signature::argument_type argument_type;

        public:
            /// Constructor for the case where the processor has the function that will be called by the slot
            template< class XFuncOwnerType >
            KTSlotDataTwoTypes(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(first_data_type&, second_data_type&), KTSignalData* signalPtr=NULL);
            /// Constructor for the case where the processor and the object with the function that will be called are different
            template< class XFuncOwnerType >
            KTSlotDataTwoTypes(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(first_data_type&, second_data_type&), KTSignalData* signalPtr=NULL);
            virtual ~KTSlotDataTwoTypes();

            void operator()(boost::shared_ptr< KTData > data);

        protected:
            boost::function< Bool_t (first_data_type&, second_data_type&) > fFunc;

            KTSignalData* fSignalPtr;
    };





    template< class XDataType >
    template< class XFuncOwnerType >
    KTSlotDataOneType< XDataType >::KTSlotDataOneType(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(data_type&), KTSignalData* signalPtr) :
            fFunc(boost::bind(func, owner, _1)),
            fSignalPtr(signalPtr)
    {
        owner->RegisterSlot(name, this, &KTSlotDataOneType::operator());
    }

    template< class XDataType >
    template< class XFuncOwnerType >
    KTSlotDataOneType< XDataType >::KTSlotDataOneType(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(data_type&), KTSignalData* signalPtr) :
            fFunc(boost::bind(func, owner, _1)),
            fSignalPtr(signalPtr)
    {
        proc->RegisterSlot(name, this, &KTSlotDataOneType::operator());
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
            KTERROR(slotlog, "Data not found with type <" << typeid(data_type).name() << ">");
            return;
        }
        // Call the function
        if (! fFunc(data->Of< data_type >()))
        {
            KTERROR(slotlog, "Something went wrong while analyzing data with type <" << typeid(data_type).name() << ">");
            return;
        }
        // If there's a signal pointer, emit the signal
        if (fSignalPtr != NULL)
        {
            (*fSignalPtr)(data);
        }
        return;
    }



    template< class XDataType1, class XDataType2 >
    template< class XFuncOwnerType >
    KTSlotDataTwoTypes< XDataType1, XDataType2 >::KTSlotDataTwoTypes(const std::string& name, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(first_data_type&, second_data_type&), KTSignalData* signalPtr) :
            fFunc(boost::bind(func, owner, _1, _2)),
            fSignalPtr(signalPtr)
    {
        owner->RegisterSlot(name, this, &KTSlotDataTwoTypes::operator());
    }

    template< class XDataType1, class XDataType2 >
    template< class XFuncOwnerType >
    KTSlotDataTwoTypes< XDataType1, XDataType2 >::KTSlotDataTwoTypes(const std::string& name, KTProcessor* proc, XFuncOwnerType* owner, Bool_t (XFuncOwnerType::*func)(first_data_type&, second_data_type&), KTSignalData* signalPtr) :
            fFunc(boost::bind(func, owner, _1, _2)),
            fSignalPtr(signalPtr)
    {
        proc->RegisterSlot(name, this, &KTSlotDataTwoTypes::operator());
    }

    template< class XDataType1, class XDataType2 >
    KTSlotDataTwoTypes< XDataType1, XDataType2 >::~KTSlotDataTwoTypes()
    {
    }

    template< class XDataType1, class XDataType2 >
    void KTSlotDataTwoTypes< XDataType1, XDataType2 >::operator()(boost::shared_ptr< KTData > data)
    {
        // Standard data slot pattern:
        // Check to ensure that the required data type is present
        if (! data->Has< first_data_type >())
        {
            KTERROR(slotlog, "Data not found with type <" << typeid(first_data_type).name() << ">");
            return;
        }
        if (! data->Has< second_data_type >())
        {
            KTERROR(slotlog, "Data not found with type <" << typeid(second_data_type).name() << ">");
            return;
        }
        // Call the function
        if (! fFunc(data->Of< first_data_type >(), data->Of< second_data_type >()))
        {
            KTERROR(slotlog, "Something went wrong while analyzing data with types <" << typeid(first_data_type).name() << "> and <" << typeid(second_data_type).name() << ">");
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
