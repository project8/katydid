/*
 * KTReader.hh
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#ifndef KTREADER_HH_
#define KTREADER_HH_

#include "KTPrimaryProcessor.hh"

namespace Katydid
{
    class KTReader : public KTPrimaryProcessor
    {
        public:
            KTReader(const std::string& name = "default-reader-name");
            virtual ~KTReader();

    };

#define KT_REGISTER_READER(reader_class, reader_name) \
        static KTNORegistrar< KTReader, reader_class > s##reader_class##ReaderRegistrar(reader_name);

} /* namespace Katydid */
#endif /* KTREADER_HH_ */
