/*
 * KTEgg.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
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

class KTArrayUC;
class TArrayC;

namespace Katydid
{
    class KTEvent;

    class KTEgg
    {
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

            Int_t GetEventSize() const;
            Int_t GetFrameIDSize() const;
            Int_t GetRecordSize() const;
            Double_t GetApproxRecordLength() const;
            Double_t GetSampleRate() const;
            Int_t GetTimeStampSize() const;

            Double_t GetHertzPerSampleRateUnit() const;
            Double_t GetSecondsPerApproxRecordLengthUnit() const;


            void SetFileName(const string& fileName);
            void SetHeaderSize(UInt_t size);
            void SetHeader(const string& header);
            void SetPrelude(const string& prelude);

            void SetEventSize(Int_t size);
            void SetFrameIDSize(Int_t size);
            void SetRecordSize(Int_t size);
            void SetApproxRecordLength(Double_t length);
            void SetSampleRate(Double_t rate);
            void SetTimeStampSize(Int_t size);

            void SetHertzPerSampleRateUnit(Double_t hpsru);
            void SetSecondsPerApproxRecordLengthUnit(Double_t spslu);

        private:
            template< typename XReturnType >
            XReturnType ConvertFromCharArray(char* value);

            string fFileName;
            ifstream fEggStream;
            string fPrelude;
            UInt_t fHeaderSize;
            string fHeader;

            Int_t fTimeStampSize;
            Int_t fFrameIDSize;
            Int_t fRecordSize;
            Int_t fEventSize;

            Double_t fApproxRecordLength;
            Double_t fSampleRate;

            Double_t fHertzPerSampleRateUnit;
            Double_t fSecondsPerApproxRecordLengthUnit;


            static const ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

            ClassDef(KTEgg, 1);

    };

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

 \attention The source of this documentation can be found in: Katydid/Egg/KTEgg.hh

 \section Version Latest Version: 0.5
 <hr>

 \section Resource Resources at your disposal
 <hr>
 \li User's Manual -- https://github.com/project8/katydid/wiki/Katydid-User&27s-Guide
 \li Reference Manual -- This Doxygen-based source documentation

 \section Help Getting Help
 <hr>
 \li Send your question by email to Noah Oblath: nsoblath-at-mit.edu
 \li For installation problems see below.
 \li For ROOT problems: see the ROOT website (http://root.cern.ch/drupal)

 \section Requirements System Requirements
 <hr>
 Linux/MacOS with a reasonably new C++ compiler:
 \li The minimum supported gcc version is 4.2.
 \li LLVM will hopefully be supported in the future.

 Dependencies:
 \li ROOT (http://root.cern.ch/drupal) version 5.24 or higher
 \li CMake (http://www.cmake.org) version 2.6 or higher


 \section GettingKT Getting Katydid
 <hr>
 You have two options for downloading any distribution of Katydid:

 <ul>
 <li><b>Distribution Tarball</b><br>
 This option is not yet available
 </li>

 <li> <b>Github</b><br>
 Katydid source can be found on the Project 8 Github repository (https://github.com/project8/katydid).
 You will need to have CMake installed on your computer (version 2.6 or higher). It's also useful to have ccmake or cmake-gui to make the configuration step easier.

 The master branch of the source code can be cloned from the Github repository in the standard way (assuming you ahve Git installed and configured on your computer):
 \code
 >  git clone https://github.com/project8/katydid.git
 >  cd katydid
 \endcode
 </li>


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

 \section Problems Installation Problems
 <hr>
 Coming soon (sorry!)


 */

#endif /* KTEGG_HH_ */
