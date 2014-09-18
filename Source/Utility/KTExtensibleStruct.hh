/*
 * KTExtensibleStruct.hh
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 *      Original Author: Sanshiro Enomoto <sanshiro@uw.edu> (KDExternalStruct, from the KATRIN experiment)
 */

#ifndef KTEXTENSIBLESTRUCT_HH_
#define KTEXTENSIBLESTRUCT_HH_

namespace Katydid
{

    // *) Grouping is optional. Give "void" to disable (default).
    // *) operator= reallocates extended fields: to avoid this, use Pull().


    template<class XGroupType>
    struct KTExtensibleStructCore : public XGroupType
    {
        public:
            KTExtensibleStructCore(void);
            KTExtensibleStructCore(const KTExtensibleStructCore&);
            virtual ~KTExtensibleStructCore();
            KTExtensibleStructCore& operator=(const KTExtensibleStructCore&);
            virtual void Clear(void);
            template<class XStructType> inline XStructType& Of(void);
            template<class XStructType> inline const XStructType& Of(void) const;
            template<class XStructType> inline bool Has(void) const;
            template<class XStructType> inline XStructType* Detatch(void);
            virtual KTExtensibleStructCore* Clone(void) const = 0;
            virtual void Pull(const KTExtensibleStructCore<XGroupType>& object) = 0;
        protected:
            mutable KTExtensibleStructCore* fNext;
    };



    template<class XInstanceType, class XGroupType>
    struct KTExtensibleStruct : KTExtensibleStructCore< XGroupType >
    {
        public:
            KTExtensibleStruct(void);
            KTExtensibleStruct(const KTExtensibleStruct& object);
            virtual ~KTExtensibleStruct();
            KTExtensibleStruct& operator=(const KTExtensibleStruct& object);
            virtual KTExtensibleStructCore<XGroupType>* Clone(void) const;
            virtual void Pull(const KTExtensibleStructCore<XGroupType>& object);
            void SetIsCopyDisabled(bool flag);
        private:
            bool fIsCopyDisabled;
    };



    template<class XGroupType>
    KTExtensibleStructCore<XGroupType>::KTExtensibleStructCore(void)
    {
        fNext = 0;
    }

    template<class XGroupType>
    KTExtensibleStructCore<XGroupType>::KTExtensibleStructCore(const KTExtensibleStructCore&)
    {
        fNext = 0;
    }

    template<class XGroupType>
    KTExtensibleStructCore<XGroupType>::~KTExtensibleStructCore()
    {
        delete fNext;
        fNext = 0;
    }

    template<class XGroupType>
    KTExtensibleStructCore<XGroupType>& KTExtensibleStructCore<XGroupType>::operator=(const KTExtensibleStructCore&)
    {
        fNext = 0;
        return *this;
    }

    template<class XGroupType>
    void KTExtensibleStructCore<XGroupType>::Clear(void)
    {
    delete fNext;
    fNext = 0;
    }

    template<class XGroupType>
    template<class XStructType>
    inline XStructType& KTExtensibleStructCore<XGroupType>::Of(void)
    {
        XStructType* target = dynamic_cast<XStructType*>(this);
        if (target)
        {
            return *target;
        }

        if (! fNext)
        {
            fNext = new XStructType();
        }

        return fNext->Of<XStructType>();
    }

    template<class XGroupType>
    template<class XStructType>
    inline const XStructType& KTExtensibleStructCore<XGroupType>::Of(void) const
    {
        const XStructType* target = dynamic_cast<const XStructType*>(this);
        if (target)
        {
            return *target;
        }

        if (fNext == 0)
        {
            fNext = new XStructType();
        }

        return fNext->Of<XStructType>();
    }



    template<class XGroupType>
    template<class XStructType>
    inline bool KTExtensibleStructCore<XGroupType>::Has(void) const
    {
        if (dynamic_cast<const XStructType*>(this))
        {
            return true;
        }
        if (fNext)
        {
            return fNext->Has<XStructType>();
        }

        return false;
    }



    template<class XGroupType>
    template<class XStructType>
    inline XStructType* KTExtensibleStructCore<XGroupType>::Detatch(void)
    {
        if (! fNext)
        {
            return 0;
        }
        XStructType* next = dynamic_cast<XStructType*>(fNext);
        if (next)
        {
            if (next->fNext)
            {
                fNext = next->fNext;
                next->fNext = 0;
            }
            return next;
        }
        return fNext->Detatch<XStructType>();
    }



    template<class XInstanceType, class XGroupType>
    KTExtensibleStruct<XInstanceType, XGroupType>::KTExtensibleStruct(void)
    {
        fIsCopyDisabled = false;
    }

    template<class XInstanceType, class XGroupType>
    KTExtensibleStruct<XInstanceType, XGroupType>::~KTExtensibleStruct()
    {
        fIsCopyDisabled = false;
    }

    template<class XInstanceType, class XGroupType>
    KTExtensibleStruct<XInstanceType, XGroupType>::KTExtensibleStruct(const KTExtensibleStruct<XInstanceType, XGroupType>& object) :
            KTExtensibleStructCore<XGroupType>(object)
    {
        // should this check fIsCopyDisabled in object?
        fIsCopyDisabled = false;

        if (object.fNext)
        {
            this->fNext = object.fNext->Clone();
        }
    }

    template<class XInstanceType, class XGroupType>
    KTExtensibleStruct<XInstanceType, XGroupType>& KTExtensibleStruct<XInstanceType, XGroupType>::operator=(const KTExtensibleStruct<XInstanceType, XGroupType>& object)
    {
        // should this check fIsCopyDisabled in object?
        if ((&object == this) || fIsCopyDisabled)
        {
            return *this;
        }

        delete this->fNext;
        this->fNext = 0;

        if (object.fNext)
        {
            this->fNext = object.fNext->Clone();
        }

        return *this;
    }

    template<class XInstanceType, class XGroupType>
    KTExtensibleStructCore<XGroupType>* KTExtensibleStruct<XInstanceType, XGroupType>::Clone(void) const
    {
        // assume CRTP is used properly,
        // otherwise compiling fails here (intended behavior)
        XInstanceType* instance = new XInstanceType(dynamic_cast<const XInstanceType&>(*this));
        if (this->fNext)
        {
            instance->fNext = this->fNext->Clone();
        }
        return instance;
    }

    template<class XInstanceType, class XGroupType>
    void KTExtensibleStruct<XInstanceType, XGroupType>::Pull(const KTExtensibleStructCore<XGroupType>& object)
    {
        if (&object == this) {
            return;
        }

        fIsCopyDisabled = true;
        XInstanceType* instance = dynamic_cast<XInstanceType*>(this);
        if (object.template Has<XInstanceType>())
        {
            instance->operator=(object.template Of<XInstanceType>());
        }
        else
        {
            instance->operator=(XInstanceType());
        }
        fIsCopyDisabled = false;

        if (this->fNext)
        {
            this->fNext->Pull(object);
        }
    }

    template<class XInstanceType, class XGroupType>
    inline void KTExtensibleStruct<XInstanceType, XGroupType>::SetIsCopyDisabled(bool flag)
    {
        fIsCopyDisabled = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTEXTENSIBLESTRUCT_HH_ */
