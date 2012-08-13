/**
 @file KTProcessor.hh
 @brief Contains KTProcessor
 @details KTProcessor is the processor base class
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTPROCESSOR_HH_
#define KTPROCESSOR_HH_

// part of the deprecated settings system; will be removed
#include "KTSetting.hh"

#include "KTConnection.hh"
#include "KTSignal.hh"

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>

#include <exception>
//#include <map>
#include <string>

#include "Rtypes.h"

namespace Katydid
{
    //class KTSignal;
    class ProcessorSignalException : public std::exception
    {
        virtual const char* what() const throw()
        {
          return "Signal requested was not found.";
        }
    };

    class KTProcessor
    {
        protected:
            typedef boost::ptr_map< std::string, KTSignal > SignalMap;
            typedef SignalMap::iterator SigMapIt;
            typedef SignalMap::value_type SigMapVal;

        public:
            KTProcessor();
            virtual ~KTProcessor();

            // e.g. subscriber type: boost::bind(&KTFFTEHuntProcessor::ProcessHeader, boost::ref(procEHunt), _1)

            // e.g. XSignalSig: void (KTEgg::HeaderInfo)

            //KTConnection ConnectToSignal(const std::string& signalName, const XSlotType& subscriber)

            template< typename XSignalSig >
            KTConnection ConnectToSignal(const std::string& signalName, const typename boost::signals2::signal< XSignalSig >::slot_type& slot)
            {
                SigMapIt iter = fSignalMap.find(signalName);
                if (iter == fSignalMap.end())
                {
                    throw ProcessorSignalException();
                }

                //KTSignal< XSignalSig >* sigPtr = static_cast< KTSignal< XSignalSig >* >(iter->second);
                //return sigPtr->signal.connect(subscriber);
                return iter->second->Connect< XSignalSig >(slot);
            }
/*
            template< typename XSignalSig >
            KTConnection ConnectToSignal(const std::string& signalName, const typename KTSignal< XSignalSig >::signal_type::slot_type& subscriber)
            {
                SigMapIt iter = fSignalMap.find(signalName);
                if (iter == fSignalMap.end())
                {
                    throw ProcessorSignalException();
                }

                KTSignal< XSignalSig >* sigPtr = static_cast< KTSignal< XSignalSig >* >(iter->second);
                return sigPtr->signal.connect(subscriber);
            }
            */

            template< typename XSignalSig >
            void RegisterSignal(std::string name, boost::signals2::signal< XSignalSig >* signalPtr)
            {
                KTSignal* sig = new KTSignal(signalPtr);
                fSignalMap.insert(name, sig);
                return;
            }

        protected:
            boost::ptr_map< std::string, KTSignal > fSignalMap;

    };
} /* namespace Katydid */
#endif /* KTPROCESSOR_HH_ */
