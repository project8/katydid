/*
 * KTEggReader.hh
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#ifndef KTEGGREADER_HH_
#define KTEGGREADER_HH_

#include "KTData.hh"

#include "factory.hh"
#include "path.hh"

#include <string>

namespace Katydid
{
    
    class KTEggProcessor;

    class KTEggReader
    {
        public:
            typedef std::vector< scarab::path > path_vec;

        public:
            KTEggReader();
            virtual ~KTEggReader();

        public:
            virtual bool Configure(const KTEggProcessor& eggProc) = 0;

            virtual Nymph::KTDataPtr BreakEgg(const path_vec&) = 0;
            virtual Nymph::KTDataPtr HatchNextSlice() = 0;
            virtual bool CloseEgg() = 0;

            virtual unsigned GetNSlicesProcessed() const = 0;
            virtual unsigned GetNRecordsProcessed() const = 0;
            virtual double GetIntegratedTime() const = 0;

    };

#define KT_REGISTER_EGGREADER(reader_class, reader_name) \
        static ::scarab::registrar< KTEggReader, reader_class > sReader##reader_class##Registrar( reader_name );

} /* namespace Katydid */
#endif /* KTEGGREADER_HH_ */
