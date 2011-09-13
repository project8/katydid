/*
 * KTEgg.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include "KTEvent.hh"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;

class KTArrayUC;

namespace Katydid
{

    class KTEgg
    {
        public:
            KTEgg();
            virtual ~KTEgg();

            Bool_t BreakEgg();
            Bool_t ParseEggHeader();
            Int_t HatchNextEvent();

            const string& GetFileName() const;
            const KTEvent* GetData() const;
            const ifstream& GetEggStream() const;
            UInt_t GetHeaderSize() const;
            const KTArrayUC* GetHeader() const;
            const KTArrayUC* GetPrelude() const;

            void SetFileName(const string& fileName);
            void SetData(KTEvent* data);
            void SetHeaderSize(UInt_t size);
            void SetHeader(KTArrayUC* header);
            void SetPrelude(KTArrayUC* prelude);

        private:
            string fFileName;
            ifstream fEggStream;
            KTArrayUC* fPrelude;
            UInt_t fHeaderSize;
            KTArrayUC* fHeader;
            KTEvent* fData;

            static const ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

            ClassDef(KTEgg, 1);

    };

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
