/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details Iterates over events in an Egg file.
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTEGGPROCESSOR_HH_
#define KTEGGPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTEgg.hh"

#include "Rtypes.h"

#include <boost/signals2.hpp>

#include <string>

namespace Katydid
{
    class KTEvent;

    /*!
     @class KTEggProcessor
     @author N. S. Oblath

     @brief Iterates over the events in an Egg file.

     @details
     Iterates over events in an egg file; events are extracted until fNEvents is reached.

     A signal is emitted when the header is parsed.  The signature for the signal call is void (KTEgg::HeaderInfo info).
     A signal is emitted for each event extracted.  The signature for the signal call is void (UInt_t iEvent, const KTEvent* eventPtr).
    */
    class KTEggProcessor : public KTProcessor
    {
        public:
            typedef boost::signals2::signal< void (const KTEggHeader*) > HeaderSignal;
            typedef boost::signals2::signal< void (UInt_t, const KTEvent*) > EventSignal;
            typedef boost::signals2::signal< void () > EggDoneSignal;

        public:
            KTEggProcessor();
            virtual ~KTEggProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            Bool_t ProcessEgg(const std::string& fileName);

            UInt_t GetNEvents() const;
            void SetNEvents(UInt_t nEvents);

        private:
            UInt_t fNEvents;

            //***************
            // Signals
            //***************

        public:
            boost::signals2::connection ConnectToHeaderSignal(const HeaderSignal::slot_type &subscriber);
            boost::signals2::connection ConnectToEventSignal(const EventSignal::slot_type &subscriber);
            boost::signals2::connection ConnectToEggDoneSignal(const EggDoneSignal::slot_type &subscriber);

        private:
            HeaderSignal fHeaderSignal;
            EventSignal fEventSignal;
            EggDoneSignal fEggDoneSignal;

    };

    inline UInt_t KTEggProcessor::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTEggProcessor::SetNEvents(UInt_t nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline boost::signals2::connection KTEggProcessor::ConnectToHeaderSignal(const HeaderSignal::slot_type &subscriber)
    {
        return fHeaderSignal.connect(subscriber);
    }

    inline boost::signals2::connection KTEggProcessor::ConnectToEventSignal(const EventSignal::slot_type &subscriber)
    {
        return fEventSignal.connect(subscriber);
    }

    inline boost::signals2::connection KTEggProcessor::ConnectToEggDoneSignal(const EggDoneSignal::slot_type &subscriber)
    {
        return fEggDoneSignal.connect(subscriber);
    }

} /* namespace Katydid */

/*!
 \mainpage Katydid

 <br>
 \section Resource Resources at your disposal
 <hr>
 \li <a href="https://github.com/project8/katydid/wiki">User's Manual</a> -- on Github
 \li Reference Manual -- this Doxygen-based source documentation

 <br>
 \section Help Getting Help
 <hr>
 \li Send your question by email to Noah Oblath: nsoblath-at-mit.edu
 \li For installation problems see below.
 \li For ROOT problems: see the <a href="http://root.cern.ch/drupal">ROOT website</a>

 <br>
 \section Requirements System Requirements
 <hr>
 Linux/MacOS with a reasonably new C++ compiler:
 \li The minimum supported gcc version is 4.2.
 \li LLVM will hopefully be supported in the future.

 Dependencies:
 \li <a href="http://root.cern.ch/drupal">ROOT</a> version 5.24 or higher
 \li <a href="http://www.cmake.org">CMake</a> version 2.6 or higher


 <br>
 \section GettingKT Getting Katydid
 <hr>
 You have two options for downloading any distribution of Katydid:

 <ul>
 <li><b>Distribution Tarball</b><br>
 This option is not yet available
 </li>

 <li> <b>Github</b><br>
 Katydid source can be found on the <a href="https://github.com/project8/katydid">Project 8 Github repository</a>.
 You will need to have CMake installed on your computer (version 2.6 or higher). It's also useful to have ccmake or cmake-gui to make the configuration step easier.

 The master branch of the source code can be cloned from the Github repository in the standard way (assuming you ahve Git installed and configured on your computer):
 \code
 >  git clone https://github.com/project8/katydid.git
 >  cd katydid
 \endcode
 </li>

 <br>
 \section Installing Installing
 <hr>
 The simplest way to configure Katydid is to use the <tt>cmake</tt> command:
 \code
 > cmake .
 \endcode

 Alternatively you can use <tt>ccmake</tt> or <tt>cmake-gui</tt> to interactively select your configuration:
 \code
 > ccmake .
 \endcode
 or
 \code
 > cmake-gui .
 \endcode

 The following two commands will, respectively, compile and install Katydid:
 \code
 > make
 > make install
 \endcode

 If everything compiles and installs correctly, Katydid is ready to use.  If there were problems with the installation process, please see the Installation Problems section below.

 <br>
 \section Problems Installation Problems
 <hr>
 \li There's a bug in Boost that's a problem when compiling with LLVM (including using the default compiler in Mac OS X 10.7).  This was fixed in Boost 1.49.00.  If you don't have access to that version you can fix the problem by making the following change:  on line 565 of boost/signals2/detail/auto_buffer.hpp, change the word "size" to "size_arg" (not including the quotes).  Boost need not be recompiled.

 <br>
 \section ExternalCode External Packages and Imported Code
 <hr>
 Two external packages are distributed with Katydid:
 \li <a href="http://rapidxml.sourceforge.net">RapidXml</a> is used for parsing the event header in the Egg files.  The code is distributed under the Boost Software License v1.0.
 \li <a href="hhtp://cimg.sourceforge.net">CImg</a> version 1.4.9 is available for any image processing tasks.  It is distributed under the CeCILL License.

 Code has also been imported with permission from the Kassiopeia package developed by the KATRIN collaboration.  The imported code resides in the Utility and Framework classes and is restricted to infrastructure-related activities.

 <!--The source of this documentation can be found in: Katydid/Egg/KTEgg.hh-->

 */

#endif /* KTEGGPROCESSOR_HH_ */
