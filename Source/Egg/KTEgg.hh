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

Katydid is a data analysis package for Project 8.
It is written in C++ and uses ROOT classes extensively.

Requirements:
  ROOT (tested with 5.26.00), including the FFTW package (root.cern.ch)
  FFTW3 (if you didn't get ROOT as a binary) (www.fftw.org)
  Mini-XML (www.minixml.org)

Directory contents:
  Scripts - A collection of useful (or not) shell and ROOT scripts
  Source
    Egg - Header and source files for all of the classes (so far)
    Main - Source code for executables

Installation:
  1. cd into the top-level directory (where this README file is located)
  2. Run 'autogen.sh'
  3. Create a directory in which to build (e.g. 'mkdir build')
  4. cd into the build directory
  5. Run '../configure'
  6. Run 'make'
  7. Run 'make install'

Documentation:
  1. This README file contains some useful information.
  2. Code reference material can be found in Documenation/html/index.html

Usage notes:
  1. The library libKatydid will be installed in the lib directory.
  2. The binaries and scripts will be installed in the bin directory.
  3. You will need to load the library (lib/libKatydid.so) into ROOT, if using this code in an interactive ROOT session.
  4. You may need to add Source/Egg to the include path in the ROOT session.
  5. The classes are in the namespace Katydid.
  6. Scripts/PlotPowerSpectrum.C will give you an idea of how the package should be used.

Other notes:
  1. KTEgg and KTEvent are C++ ports of Lisa McBride's monarch library.
  2. The executable ExtractEvents is based on Lisa's code for printing power spectra to text files, though it doesn't perform the FFT or write any files.  It's there for testing purposes only.

Contact:
  Noah Oblath (nsoblath@mit.edu)
*/

#endif /* KTEGG_HH_ */
