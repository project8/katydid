/*
 * KTSignal.hh
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

//#include <set>

namespace Katydid
{

    class KTSignal : public boost::noncopyable
    {
        private:
            struct KTSignalConcept
            {
                virtual ~KTSignalConcept() {}
                //virtual KTConnection Connect(void* slotPtr) = 0;
            };

            template< typename XSignature >
            struct KTSignalModel : public KTSignalConcept, public boost::noncopyable
            {
                public:
                    typedef boost::signals2::signal< XSignature > signal_type;
                    KTSignalModel(signal_type* signalPtr) : fSignal(signalPtr) {}
                    virtual ~KTSignalModel() {}
                    template< typename XSlotType >
                    KTConnection Connect(const XSlotType& slot)
                    {
                        //typename signal_type::slot_type* castSlotPtr = static_cast< typename signal_type::slot_type* >(slotPtr);
                        /*
                        if (castSlotPtr == NULL)
                        {
                            // TODO: throw exception
                        }
                        */
                        return fSignal->connect(slot);
                    }
                private:
                    signal_type* fSignal; //not owned by this KTSignal
            };

            KTSignalConcept* fSignal;

        public:
            template< typename XSignature >
            KTSignal(boost::signals2::signal< XSignature >* signalPtr) : fSignal(NULL)
            {
                fSignal = new KTSignalModel< XSignature >(signalPtr);
            }
            ~KTSignal();

            template< typename XSignature >
            KTConnection Connect(const typename boost::signals2::signal< XSignature >::slot_type& slot)
            {
                KTSignalModel< XSignature >* derivedSignalModel = dynamic_cast< KTSignalModel< XSignature >* >(fSignal);
                if (derivedSignalModel == NULL)
                {
                    // throw error
                }
                return derivedSignalModel->Connect(slot);
            }

        private:
            KTSignal();

    };

    /*
    template< typename XSignature >
    struct KTSignal
    {
            typedef KTSignal< XSignature > signal_wrapper_type;
            typedef boost::signals2::signal< XSignature > signal_type;
            typedef signal_type::slot_type slot_type;
            signal_type signal;
    };
    */
    /*
    //class KTConnection;

    class KTSignal
    {
        public:
            KTSignal();
            virtual ~KTSignal();

            void AddConnection(KTConnection* connection);

            void ExecuteSignals();

        protected:
            std::multiset< KTConnection* > fConnections;
    };
    */

} /* namespace Katydid */
#endif /* KTSIGNAL_HH_ */
