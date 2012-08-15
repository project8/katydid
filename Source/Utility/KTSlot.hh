/*
 * KTSlot.hh
 *
 *  Created on: Feb 10, 2012
 *      Author: nsoblath
 */

#ifndef KTSLOT_HH_
#define KTSLOT_HH_

#include "KTConnection.hh"

#include <boost/function.hpp>

#include <iostream>

namespace Katydid
{
    class SlotException : public std::logic_error
    {
        public:
            SlotException(std::string const& why);
    };

    class KTSlot : public boost::noncopyable
    {
        private:
            struct KTSlotConcept
            {
                virtual ~KTSlotConcept() {}
                //virtual KTConnection Connect(void* slotPtr) = 0;
            };

            template< class XTargetType, typename XSignature >
            struct KTSlotModel : public KTSlotConcept, public boost::noncopyable
            {
                public:
                    //typedef typename boost::signals2::signal< XSignature >::slot_type slot_type;
                    KTSlotModel(XTargetType* target, XSignature* funcPtr, int nArgs) :
                            fTarget(target),
                            fSlot(funcPtr),
                            fNArgs(nArgs)
                    {}
                    virtual ~KTSlotModel() {}
                    XSignature& GetSlot()
                    {
                        return *fSlot;
                    }
                private:
                    XTargetType* fTarget; // not owned by this KTSlot
                    //XSignature XTargetType::* fSlot; //not owned by this KTSlot
                    XSignature* fSlot;
                    int fNArgs;
            };

            KTSlotConcept* fSlot;

        public:
            //KTSlot(XTargetType* target, boost::signals2::signal< XSignature >* signalPtr, int nArgs) :
            template< class XTargetType, typename XSignature >
            KTSlot(XTargetType* target, XSignature* signalPtr, int nArgs) :
                    fSlot(new KTSlotModel< XTargetType, XSignature >(target, signalPtr, nArgs)),
                    fConnection()
            {
            }
            ~KTSlot();

            template< class XTargetType, typename XSignature >
            //typename boost::signals2::signal< XSignature >::slot_type GetSlot()
            XSignature& GetSlot()
            {
                KTSlotModel< XTargetType, XSignature >* derivedSlotModel = dynamic_cast< KTSlotModel< XTargetType, XSignature >* >(fSlot);
                if (derivedSlotModel == NULL)
                {
                    std::cout << "unable to cast slot; throwing exception" << std::endl;
                    throw SlotException("Incorrect cast from KTSlot* to derived type");
                }
                return derivedSlotModel->GetSlot();
            }

            int GetNArgs() const;

        private:
            KTSlot();


        public:
            void SetConnection(KTConnection conn);
            void Disconnect();

        private:
            KTConnection fConnection;

    };

    inline void KTSlot::SetConnection(KTConnection conn)
    {
        fConnection = conn;
        return;
    }

    inline void KTSlot::Disconnect()
    {
        fConnection.disconnect();
        return;
    }

} /* namespace Katydid */
#endif /* KTSLOT_HH_ */
