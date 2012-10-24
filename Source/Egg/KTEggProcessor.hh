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

namespace Katydid
{
    class KTPStoreNode;

    /*!
     @class KTEggProcessor
     @author N. S. Oblath

     @brief Iterates over the events in an Egg file.

     @details
     Iterates over events in an egg file; events are extracted until fNEvents is reached.

     Available configuration options:
     \li \c "number-of-events": UInt_t -- Number of events to process
     \li \c "filename": string -- Egg filename to use
     \li \c "egg-reader": string -- Egg reader to use (options: monarch [default], 2011)
     \li \c "time-series-length": UInt_t -- Specify the size of the time series (select 0 to use the Monarch record length)
     \li \c "time-series-type": string -- Type of time series to produce (options: real [default], fftw [not available with the 2011 egg reader])

     Command-line options defined
     \li \c -n (n-events): Number of events to process
     \li \c -e (egg-file): Egg filename to use
     \li \c -z (--use-2011-egg-reader): Use the 2011 egg reader

     Signals:
     \li \c void (KTEgg::HeaderInfo info) emitted when the file header is parsed.
     \li \c void (UInt_t iEvent, const KTEvent* eventPtr) emitted when an event is read from the file.
     \li \c void () emitted when a file is finished.
    */
    class KTEggProcessor : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTEggHeader*) >::signal HeaderSignal;
            typedef KTSignal< void (KTEvent*) >::signal EventSignal;
            typedef KTSignal< void () >::signal EggDoneSignal;

        public:
            enum EggReaderType
            {
                k2011EggReader,
                kMonarchEggReader
            };

            enum TimeSeriesType
            {
                kRealTimeSeries,
                kFFTWTimeSeries
            };

        public:
            KTEggProcessor();
            virtual ~KTEggProcessor();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t Run();

            Bool_t ProcessEgg();

            UInt_t GetNEvents() const;
            const std::string& GetFilename() const;
            EggReaderType GetEggReaderType() const;
            UInt_t GetRecordSizeRequest() const;
            TimeSeriesType GetTimeSeriesType() const;

            void SetNEvents(UInt_t nEvents);
            void SetFilename(const std::string& filename);
            void SetEggReaderType(EggReaderType type);
            void SetRecordSizeRequest(UInt_t size);
            void SetTimeSeriesType(TimeSeriesType type);

        private:
            UInt_t fNEvents;

            std::string fFilename;

            EggReaderType fEggReaderType;

            UInt_t fRecordSizeRequest;

            TimeSeriesType fTimeSeriesType;

            //***************
            // Signals
            //***************

        private:
            HeaderSignal fHeaderSignal;
            EventSignal fEventSignal;
            EggDoneSignal fEggDoneSignal;

    };

    inline Bool_t KTEggProcessor::Run()
    {
        return ProcessEgg();
    }

    inline UInt_t KTEggProcessor::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTEggProcessor::SetNEvents(UInt_t nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline const std::string& KTEggProcessor::GetFilename() const
    {
        return fFilename;
    }

    inline void KTEggProcessor::SetFilename(const std::string& filename)
    {
        fFilename = filename;
        return;
    }

    inline KTEggProcessor::EggReaderType KTEggProcessor::GetEggReaderType() const
    {
        return fEggReaderType;
    }

    inline void KTEggProcessor::SetEggReaderType(EggReaderType type)
    {
        fEggReaderType = type;
        return;
    }

    inline UInt_t KTEggProcessor::GetRecordSizeRequest() const
    {
        return fRecordSizeRequest;
    }

    inline void KTEggProcessor::SetRecordSizeRequest(UInt_t size)
    {
        fRecordSizeRequest = size;
        return;
    }

    inline KTEggProcessor::TimeSeriesType KTEggProcessor::GetTimeSeriesType() const
    {
        return fTimeSeriesType;
    }

    inline void KTEggProcessor::SetTimeSeriesType(TimeSeriesType type)
    {
        fTimeSeriesType = type;
        return;
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
