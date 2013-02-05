/*
 * KTData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTDATA_HH_
#define KTDATA_HH_

#include <string>

namespace Katydid
{
    class KTBundle;

    class KTData
    {
        private:
            friend class KTBundle;

        public:
            KTData();
            virtual ~KTData();

        public:
            const std::string& GetName() const;
            void SetName(const std::string& name);

        protected:
            std::string fName;

        public:
            /// Returns a pointer to the bundle from which this data object originates
            virtual KTBundle* GetEvent() const;
            void SetEvent(KTBundle* bundle);

        protected:
            KTBundle* fEvent; // not owned by this object
    };

    inline KTBundle* KTData::GetEvent() const
    {
        return fEvent;
    }

    inline void KTData::SetEvent(KTBundle* bundle)
    {
        fEvent = bundle;
        return;
    }

    inline const std::string& KTData::GetName() const
    {
        return fName;
    }

    inline void KTData::SetName(const std::string& name)
    {
        fName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTDATA_HH_ */
