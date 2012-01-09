/**
 @file KTEgg.hh
 @brief Contains KTEgg
 @details Reads Egg data files: parses the header and produces events.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include "Rtypes.h"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;

namespace Katydid
{
    class KTEvent;

    class KTEgg
    {
        public:
            struct HeaderInfo
            {
                Int_t fEventSize;
                Int_t fFrameIDSize;
                Int_t fRecordSize;
                Int_t fTimeStampSize;
                Double_t fRunLength;
                Double_t fSampleRate;
                Double_t fHertzPerSampleRateUnit;
                Double_t fSecondsPerRunLengthUnit;

                HeaderInfo() :
                    fTimeStampSize(0),
                    fFrameIDSize(0),
                    fRecordSize(0),
                    fEventSize(0),
                    fRunLength(0.),
                    fSampleRate(0.),
                    fHertzPerSampleRateUnit(1.),
                    fSecondsPerRunLengthUnit(1.)
                {}
            };

        public:
            KTEgg();
            virtual ~KTEgg();

            Bool_t BreakEgg();
            Bool_t ParseEggHeader();
            KTEvent* HatchNextEvent();

            const string& GetFileName() const;
            const ifstream& GetEggStream() const;
            UInt_t GetHeaderSize() const;
            const string& GetHeader() const;
            const string& GetPrelude() const;

            const HeaderInfo& GetHeaderInfo() const;
            Int_t GetEventSize() const;
            Int_t GetFrameIDSize() const;
            Int_t GetRecordSize() const;
            Double_t GetRunLength() const;
            Double_t GetApproxRecordLength() const; /// deprecated
            Double_t GetSampleRate() const;
            Int_t GetTimeStampSize() const;

            Double_t GetHertzPerSampleRateUnit() const;
            Double_t GetSecondsPerRunLengthUnit() const;
            Double_t GetSecondsPerApproxRecordLengthUnit() const; /// deprecated


            void SetFileName(const string& fileName);
            void SetHeaderSize(UInt_t size);
            void SetHeader(const string& header);
            void SetPrelude(const string& prelude);

            void SetEventSize(Int_t size);
            void SetFrameIDSize(Int_t size);
            void SetRecordSize(Int_t size);
            void SetRunLength(Double_t length);
            void SetApproxRecordLength(Double_t length); /// deprecated
            void SetSampleRate(Double_t rate);
            void SetTimeStampSize(Int_t size);

            void SetHertzPerSampleRateUnit(Double_t hpsru);
            void SetSecondsPerRunLengthUnit(Double_t sprlu);
            void SetSecondsPerApproxRecordLengthUnit(Double_t spslu); /// deprecated

        private:
            template< typename XReturnType >
            XReturnType ConvertFromCharArray(char* value);

            string fFileName;
            ifstream fEggStream;
            string fPrelude;
            UInt_t fHeaderSize;
            string fHeader;

            HeaderInfo fHeaderInfo;

            static const ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

            ClassDef(KTEgg, 1);

    };

    inline const string& KTEgg::GetFileName() const
    {
        return fFileName;
    }

    inline const ifstream& KTEgg::GetEggStream() const
    {
        return fEggStream;
    }

    inline UInt_t KTEgg::GetHeaderSize() const
    {
        return fHeaderSize;
    }

    inline const string& KTEgg::GetHeader() const
    {
        return fHeader;
    }

    inline const string& KTEgg::GetPrelude() const
    {
        return fPrelude;
    }

    inline const KTEgg::HeaderInfo& KTEgg::GetHeaderInfo() const
    {
        return fHeaderInfo;
    }

    inline Int_t KTEgg::GetEventSize() const
    {
        return fHeaderInfo.fEventSize;
    }

    inline Int_t KTEgg::GetFrameIDSize() const
    {
        return fHeaderInfo.fFrameIDSize;
    }

    inline Int_t KTEgg::GetRecordSize() const
    {
        return fHeaderInfo.fRecordSize;
    }

    inline Double_t KTEgg::GetRunLength() const
    {
        return fHeaderInfo.fRunLength;
    }

    inline Double_t KTEgg::GetApproxRecordLength() const
    {
        return GetRunLength();
    }

    inline Double_t KTEgg::GetSampleRate() const
    {
        return fHeaderInfo.fSampleRate;
    }

    inline Int_t KTEgg::GetTimeStampSize() const
    {
        return fHeaderInfo.fTimeStampSize;
    }

    inline Double_t KTEgg::GetHertzPerSampleRateUnit() const
    {
        return fHeaderInfo.fHertzPerSampleRateUnit;
    }

    inline Double_t KTEgg::GetSecondsPerRunLengthUnit() const
    {
        return fHeaderInfo.fSecondsPerRunLengthUnit;
    }

    inline Double_t KTEgg::GetSecondsPerApproxRecordLengthUnit() const
    {
        return GetSecondsPerRunLengthUnit();
    }


    inline void KTEgg::SetFileName(const string& fileName)
    {
        this->fFileName = fileName;
    }

    inline void KTEgg::SetHeaderSize(UInt_t size)
    {
        this->fHeaderSize = size;
    }

    inline void KTEgg::SetHeader(const string& header)
    {
        //delete fHeader;
        this->fHeader = header;
    }

    inline void KTEgg::SetPrelude(const string& prelude)
    {
        //delete fPrelude;
        this->fPrelude = prelude;
    }

    inline void KTEgg::SetEventSize(Int_t size)
    {
        fHeaderInfo.fEventSize = size;
    }

    inline void KTEgg::SetFrameIDSize(Int_t size)
    {
        fHeaderInfo.fFrameIDSize = size;
    }

    inline void KTEgg::SetRecordSize(Int_t size)
    {
        fHeaderInfo.fRecordSize = size;
    }

    inline void KTEgg::SetRunLength(Double_t length)
    {
        fHeaderInfo.fRunLength = length;
    }

    inline void KTEgg::SetApproxRecordLength(Double_t length)
    {
        SetRunLength(length);
    }

    inline void KTEgg::SetSampleRate(Double_t rate)
    {
        fHeaderInfo.fSampleRate = rate;
    }

    inline void KTEgg::SetTimeStampSize(Int_t size)
    {
        fHeaderInfo.fTimeStampSize = size;
    }

    inline void KTEgg::SetHertzPerSampleRateUnit(Double_t hpsru)
    {
        fHeaderInfo.fHertzPerSampleRateUnit = hpsru;
    }

    inline void KTEgg::SetSecondsPerRunLengthUnit(Double_t sprlu)
    {
        fHeaderInfo.fSecondsPerRunLengthUnit = sprlu;
    }

    inline void KTEgg::SetSecondsPerApproxRecordLengthUnit(Double_t spslu)
    {
        SetSecondsPerRunLengthUnit(spslu);
    }


    template< typename XReturnType >
    XReturnType KTEgg::ConvertFromCharArray(char* value)
    {
        stringstream converter;
        XReturnType converted;
        converter << value;
        converter >> converted;
        return converted;
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
 Coming soon (sorry!)

 <br>
 \section ExternalCode External Packages and Imported Code
 <hr>
 Two external packages are distributed with Katydid:
 \li <a href="http://rapidxml.sourceforge.net">RapidXml</a> is used for parsing the event header in the Egg files.  The code is distributed under the Boost Software License v1.0.
 \li <a href="hhtp://cimg.sourceforge.net">CImg</a> version 1.4.9 is available for any image processing tasks.  It is distributed under the CeCILL License.

 Code has also been imported with permission from the Kassiopeia package developed by the KATRIN collaboration.  The imported code resides in the Utility and Framework classes and is restricted to infrastructure-related activities.

 <!--The source of this documentation can be found in: Katydid/Egg/KTEgg.hh-->

 */

#endif /* KTEGG_HH_ */
