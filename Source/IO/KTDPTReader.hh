/*
 * KTDPTReader.hh
 *
 *  Created on: Oct 13, 2016
 *      Author: obla999
 */

#ifndef KATYDID_IO_KTDPTREADER_HH_
#define KATYDID_IO_KTDPTREADER_HH_

#include "KTMemberVariable.hh"
#include "KTReader.hh"

#include <string>

namespace Katydid
{

    /*!
     @class KTDPTReader
     @author N. S. Oblath

     @brief Reads a DPT file and outputs a frequency spectrum

     @details

     Configuration name: "dpt-reader"

     Available configuration values:
     - "input-file": string -- input filename (may be repeated)


     Slots:

     Signals:
     - "fs-polar": void (Nymph::KTDataPtr) -- Emitted after reading a DPT file; Guarantees KTFrequencySpectrumDataPolar
    */
    class KTDPTReader : public Nymph::KTReader
    {
        public:
            KTDPTReader(const std::string& name = "dpt-reader" );
            virtual ~KTDPTReader();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLEREF( std::string, Filename );

        public:
            virtual bool Run();

            bool ReadFile(Nymph::KTData& data);


            //**************
            // Signals
            //**************
        private:
            Nymph::KTSignalData fDPTSignal;

            //**************
            // Slots
            //**************

    };

} /* namespace Katydid */

#endif /* KATYDID_IO_KTDPTREADER_HH_ */
