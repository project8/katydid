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
    class KTEvent;

    class KTData
    {
        private:
            friend class KTEvent;

        public:
            KTData();
            virtual ~KTData();

        public:
            const std::string& GetName() const;
            void SetName(const std::string& name);

        protected:
            std::string fName;

        public:
            /// Returns a pointer to the event from which this data object originates
            virtual KTEvent* GetEvent() const;
            void SetEvent(KTEvent* event);

        protected:
            KTEvent* fEvent; // not owned by this object
    };

    inline KTEvent* KTData::GetEvent() const
    {
        return fEvent;
    }

    inline void KTData::SetEvent(KTEvent* event)
    {
        fEvent = event;
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
