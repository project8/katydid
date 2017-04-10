/*
 * KTEgg3ReaderPy.hh
 *
 *  Created on: April 07, 2017
 *      Author: laroque
 */

#ifndef KTEGG3READERPY_HH_
#define KTEGG3READERPY_HH_

#include "KTEgg3Reader.hh"

void export_KTEgg3ReaderPy()
{
    using namespace boost::python;
    class_<Katydid::KTEgg3Reader>("KTEgg3Reader",init<>())
        //.def("BreakAnEgg", &KTEggReader::BreakAnEgg, "Open an egg file by name")
        ;
}

/*
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

            Nymph::KTDataPtr BreakAnEgg(const std::string& filename);
            virtual Nymph::KTDataPtr BreakEgg(const path_vec&) = 0;
            virtual Nymph::KTDataPtr HatchNextSlice() = 0;
            virtual bool CloseEgg() = 0;

            virtual unsigned GetNSlicesProcessed() const = 0;
            virtual unsigned GetNRecordsProcessed() const = 0;
            virtual double GetIntegratedTime() const = 0;

    };

    inline Nymph::KTDataPtr KTEggReader::BreakAnEgg(const std::string& filename)
    {
        path_vec filenameVec;
        filenameVec.emplace_back(filename);
        return BreakEgg(filenameVec);
    }
*/
#endif /* KTEGG3READERPY_HH_ */
