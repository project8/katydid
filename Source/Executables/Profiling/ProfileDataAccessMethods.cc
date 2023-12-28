/*
 * ProfileDataAccessMethods.cc
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#include "logger.hh"
#include "KTThroughputProfiler.hh"

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/weak_ptr.hpp>

#include <cstdlib>
#include <map>
#include <string>
#include <utility>
#include <vector>

//using namespace Katydid;
using namespace std;

using boost::unordered_map;

LOGGER(proflog, "ProfileDataAccessMethods");

struct Data
{
        string fMyData;
        virtual ~Data() {}
};

struct DataA: Data
{
        int fValueA;
        virtual ~DataA() {}
};

struct DataB: Data
{
        double fValueB;
        virtual ~DataB() {}
};

struct DataC: Data
{
        double fValueC;
        virtual ~DataC() {}
};

struct DataD: Data
{
        double fValueD;
        virtual ~DataD() {}
};

struct DataE: Data
{
        double fValueE;
        virtual ~DataE() {}
};

struct DataF: Data
{
        double fValueF;
        virtual ~DataF() {}
};

//***********************
// String-Keyed Bundle
//***********************

struct StringKeyedBundle
{
        typedef unordered_map< string, Data*> MapType;
        MapType fDataMap;
        MapType::iterator fCachedIterator;
        template< typename XDataType >
        XDataType* GetData(const string& name)
        {
            fCachedIterator = fDataMap.find(name);
            if (fCachedIterator == fDataMap.end()) return NULL;
            return dynamic_cast< XDataType* >(fCachedIterator->second);
        }
};

//********************************
// Type-and-String-Keyed Bundle
//********************************

struct DataMap
{
        virtual ~DataMap() {}
        virtual Data* GetData(const string& name) = 0;
};

template< typename XDataType >
struct SpecDataMap: DataMap
{
    typedef unordered_map< string, XDataType* > MapType;
    MapType fDataMap;
    typename MapType::iterator fCachedIterator;
    XDataType* GetData(const string& name)
    {
        fCachedIterator = fDataMap.find(name);
        if (fCachedIterator == fDataMap.end()) return NULL;
        return fCachedIterator->second;
    }
};

struct TypeAndStringKeyedBundle
{
        typedef unordered_map< const type_info*, DataMap* > MapOfDataMaps;
        MapOfDataMaps fMapOfMaps;
        MapOfDataMaps::iterator fCachedIterator;
        TypeAndStringKeyedBundle()
        {
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataA>), new SpecDataMap<DataA>()));
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataB>), new SpecDataMap<DataB>()));
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataC>), new SpecDataMap<DataC>()));
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataD>), new SpecDataMap<DataD>()));
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataE>), new SpecDataMap<DataE>()));
            fMapOfMaps.insert(pair<const type_info*, DataMap*>(&typeid(SpecDataMap<DataF>), new SpecDataMap<DataF>()));
        }
        template< typename XDataType >
        void AddData(const string& name, XDataType* data)
        {
            fCachedIterator = fMapOfMaps.find(&typeid(XDataType));
            if (fCachedIterator == fMapOfMaps.end()) return;
            static_cast<SpecDataMap<XDataType>* >(fCachedIterator->second)->fDataMap.insert(pair<string, XDataType*>(name, data));
            return;
        }
        template< typename XDataType >
        XDataType* GetData(const string& name)
        {
            fCachedIterator = fMapOfMaps.find(&typeid(XDataType));
            if (fCachedIterator == fMapOfMaps.end()) return NULL;
            return static_cast< XDataType* >(fCachedIterator->second->GetData(name));
        }
};

//*****************************
// Extensible Struct
// Author: Sanshiro Enomonto
//*****************************

// Implementation is below

template<class TGroup=void>
struct KDExtensibleStructCore {
    public:
        KDExtensibleStructCore(void);
        KDExtensibleStructCore(const KDExtensibleStructCore&);
        virtual ~KDExtensibleStructCore();
        KDExtensibleStructCore& operator=(const KDExtensibleStructCore&);
        virtual void Clear(void);
        template<class TStruct> inline TStruct& Of(void);
        template<class TStruct> inline const TStruct& Of(void) const;
        template<class TStruct> inline bool Has(void) const;
        virtual KDExtensibleStructCore* Clone(void) const = 0;
        virtual void Pull(const KDExtensibleStructCore<TGroup>& object) = 0;
    protected:
        mutable KDExtensibleStructCore* fNext;
};

template<class TInstance, class TGroup=void>
struct KDExtensibleStruct: KDExtensibleStructCore<TGroup> {
    KDExtensibleStruct(void);
    KDExtensibleStruct(const KDExtensibleStruct& object);
    virtual ~KDExtensibleStruct();
    KDExtensibleStruct& operator=(const KDExtensibleStruct& object);
    virtual KDExtensibleStructCore<TGroup>* Clone(void) const;
    virtual void Pull(const KDExtensibleStructCore<TGroup>& object);
    private:
    bool fIsCopyDisabled;
};

struct ESData
{
    string fMyData;
    virtual ~ESData() {}
};

struct ESDataA: KDExtensibleStruct<ESDataA>
{
    int fValueA;
    virtual ~ESDataA() {}
};

struct ESDataB: KDExtensibleStruct<ESDataB>
{
    double fValueB;
    virtual ~ESDataB() {}
};

struct ESDataC: KDExtensibleStruct<ESDataC>
{
    double fValueC;
    virtual ~ESDataC() {}
};

struct ESDataD: KDExtensibleStruct<ESDataD>
{
    double fValueD;
    virtual ~ESDataD() {}
};

struct ESDataE: KDExtensibleStruct<ESDataE>
{
    double fValueE;
    virtual ~ESDataE() {}
};

struct ESDataF: KDExtensibleStruct<ESDataF>
{
    double fValueF;
    virtual ~ESDataF() {}
};

//*****************************
// Mapped Extensible Struct
// Author: Sanshiro Enomonto
// Mods by: Noah Oblath
//*****************************

// Implementation is below
/*
template<class TGroup=void>
struct KDMappedExtensibleStructCore
{
    public:
        KDMappedExtensibleStructCore(void);
        //KDMappedExtensibleStructCore(const KDMappedExtensibleStructCore&);
        virtual ~KDMappedExtensibleStructCore();
        //KDMappedExtensibleStructCore& operator=(const KDMappedExtensibleStructCore&);
        //virtual void Clear(void);
        template<class TStruct> inline TStruct& Of(void);
        template<class TStruct> inline const TStruct& Of(void) const;
        template<class TStruct> inline bool Has(void) const;
        //virtual KDExtensibleStructCore* Clone(void) const = 0;
        //virtual void Pull(const KDMappedExtensibleStructCore<TGroup>& object) = 0;
    protected:
        //typedef boost::shared_ptr< KDMappedExtensibleStructCore > MESCorePtr;
        //typedef unordered_map< const type_info*, MESCorePtr > StructMap;
        //mutable boost::shared_ptr< StructMap > fStructs;
        typedef unordered_map< const type_info*, KDMappedExtensibleStructCore* >StructMap;
        mutable StructMap* fStructs;
};

template<class TInstance, class TGroup=void>
struct KDMappedExtensibleStruct: KDMappedExtensibleStructCore<TGroup>
{
    public:
        KDMappedExtensibleStruct(void);
        KDMappedExtensibleStruct(const KDMappedExtensibleStruct& object);
        virtual ~KDMappedExtensibleStruct();
        //KDExtensibleStruct& operator=(const KDExtensibleStruct& object);
        //virtual KDMappedExtensibleStructCore<TGroup>* Clone(void) const;
        //virtual void Pull(const KDMappedExtensibleStructCore<TGroup>& object);
    private:
        bool fIsCopyDisabled;
};

struct MESData
{
        string fMyData;
        virtual ~MESData() {}
};

struct MESDataA: KDMappedExtensibleStruct<ESDataA>
{
    int fValueA;
    virtual ~MESDataA() {}
};

struct MESDataB: KDMappedExtensibleStruct<ESDataB>
{
    double fValueB;
    virtual ~MESDataB() {}
};

struct MESDataC: KDMappedExtensibleStruct<ESDataC>
{
    double fValueC;
    virtual ~MESDataC() {}
};

struct MESDataD: KDMappedExtensibleStruct<ESDataD>
{
    double fValueD;
    virtual ~MESDataD() {}
};

struct MESDataE: KDMappedExtensibleStruct<ESDataE>
{
    double fValueE;
    virtual ~MESDataE() {}
};

struct MESDataF: KDMappedExtensibleStruct<ESDataF>
{
    double fValueF;
    virtual ~MESDataF() {}
};
*/


int main()
{
    unsigned nNames = 6;
    vector<string> names(nNames);
    names[0] = "data1";
    names[1] = "data2";
    names[2] = "data3";
    names[3] = "data4";
    names[4] = "data5";
    names[5] = "data6";

    unsigned nIterations = 2000000;

    Nymph::KTThroughputProfiler prof;

    //*******************
    // Prepare Bundles
    //*******************

    // String-Keyed Bundle
    StringKeyedBundle skb;
    skb.fDataMap.insert(pair< string, Data* >(names[0], new DataA()));
    skb.fDataMap.insert(pair< string, Data* >(names[1], new DataB()));
    skb.fDataMap.insert(pair< string, Data* >(names[2], new DataC()));
    skb.fDataMap.insert(pair< string, Data* >(names[3], new DataD()));
    skb.fDataMap.insert(pair< string, Data* >(names[4], new DataE()));
    skb.fDataMap.insert(pair< string, Data* >(names[5], new DataF()));

    // Type-and-String-Keyed Bundle
    TypeAndStringKeyedBundle taskb;
    taskb.AddData(names[0], new DataA());
    taskb.AddData(names[1], new DataB());
    taskb.AddData(names[2], new DataC());
    taskb.AddData(names[3], new DataD());
    taskb.AddData(names[4], new DataE());
    taskb.AddData(names[5], new DataF());

    // Extensible Struct
    ESDataA esdata;
    esdata.Of<ESDataB>();
    esdata.Of<ESDataC>();
    esdata.Of<ESDataD>();
    esdata.Of<ESDataE>();
    esdata.Of<ESDataF>();
/*
    // Mapped Extensible Struct
    MESDataA mesdata;
    mesdata.Of<MESDataB>();
    mesdata.Of<MESDataC>();
    mesdata.Of<MESDataD>();
    mesdata.Of<MESDataE>();
    mesdata.Of<MESDataF>();
*/
    //*************
    // Run tests
    //*************
    DataA* dataPtrA;
    DataB* dataPtrB;
    DataC* dataPtrC;
    DataD* dataPtrD;
    DataE* dataPtrE;
    DataF* dataPtrF;

    // Test string-keyed bundle
    LINFO(proflog, "Testing the String-Keyed Bundle");
    prof.Start();
    for (unsigned iIteration = 0; iIteration < nIterations; iIteration++)
    {
        dataPtrA = skb.GetData<DataA>(names[0]);
        dataPtrB = skb.GetData<DataB>(names[1]);
        dataPtrC = skb.GetData<DataC>(names[2]);
        dataPtrD = skb.GetData<DataD>(names[3]);
        dataPtrE = skb.GetData<DataE>(names[4]);
        dataPtrF = skb.GetData<DataF>(names[5]);
    }
    prof.Stop();
    timespec elapsed = prof.Elapsed();
    double elapsed_sec = elapsed.tv_sec + elapsed.tv_nsec * 1.e-9;
    LINFO(proflog, "Elapsed time: " << elapsed_sec << " s");
    LINFO(proflog, "Time per access: " << elapsed_sec / double(nNames * nIterations))

    // Test type-and-string-keyed bundle
    LINFO(proflog, "Testing the Type-and-String-Keyed Bundle");
    prof.Start();
    for (unsigned iIteration = 0; iIteration < nIterations; iIteration++)
    {
        dataPtrA = taskb.GetData<DataA>(names[0]);
        dataPtrB = taskb.GetData<DataB>(names[1]);
        dataPtrC = taskb.GetData<DataC>(names[2]);
        dataPtrD = taskb.GetData<DataD>(names[3]);
        dataPtrE = taskb.GetData<DataE>(names[4]);
        dataPtrF = taskb.GetData<DataF>(names[5]);
    }
    prof.Stop();
    elapsed = prof.Elapsed();
    elapsed_sec = elapsed.tv_sec + elapsed.tv_nsec * 1.e-9;
    LINFO(proflog, "Elapsed time: " << elapsed_sec << " s");
    LINFO(proflog, "Time per access: " << elapsed_sec / double(nNames * nIterations))

    // Test extensible struct
    LINFO(proflog, "Testing the Extensible Struct");
    prof.Start();
    for (unsigned iIteration = 0; iIteration < nIterations; iIteration++)
    {
        esdata.Of<ESDataA>();
        esdata.Of<ESDataB>();
        esdata.Of<ESDataC>();
        esdata.Of<ESDataD>();
        esdata.Of<ESDataE>();
        esdata.Of<ESDataF>();
    }
    prof.Stop();
    elapsed = prof.Elapsed();
    elapsed_sec = elapsed.tv_sec + elapsed.tv_nsec * 1.e-9;
    LINFO(proflog, "Elapsed time: " << elapsed_sec << " s");
    LINFO(proflog, "Time per access: " << elapsed_sec / double(nNames * nIterations))
/*
    // Test mapped extensible struct
    LINFO(proflog, "Testing the Mapped Extensible Struct");
    prof.Start();
    for (unsigned iIteration = 0; iIteration < nIterations; iIteration++)
    {
        mesdata.Of<MESDataA>();
        mesdata.Of<MESDataB>();
        mesdata.Of<MESDataC>();
        mesdata.Of<MESDataD>();
        mesdata.Of<MESDataE>();
        mesdata.Of<MESDataF>();
    }
    prof.Stop();
    elapsed = prof.Elapsed();
    elapsed_sec = elapsed.tv_sec + elapsed.tv_nsec * 1.e-9;
    LINFO(proflog, "Elapsed time: " << elapsed_sec << " s");
    LINFO(proflog, "Time per access: " << elapsed_sec / double(nNames * nIterations))
*/

    return 0;
}

//************************************
// Extensible struct implementation
//************************************

template<class TGroup>
KDExtensibleStructCore<TGroup>::KDExtensibleStructCore(void)
{
        fNext = 0;
}

template<class TGroup>
KDExtensibleStructCore<TGroup>::KDExtensibleStructCore(const KDExtensibleStructCore&)
{
        fNext = 0;
}

template<class TGroup>
KDExtensibleStructCore<TGroup>::~KDExtensibleStructCore()
{
        delete fNext;
}

template<class TGroup>
KDExtensibleStructCore<TGroup>& KDExtensibleStructCore<TGroup>::operator=(const KDExtensibleStructCore&)
{
        fNext = 0;
        return *this;
}

template<class TGroup>
void KDExtensibleStructCore<TGroup>::Clear(void)
{
        delete fNext;
        fNext = 0;
}

template<class TGroup>
template<class TStruct>
inline TStruct& KDExtensibleStructCore<TGroup>::Of(void)
{
        TStruct* target = dynamic_cast<TStruct*>(this);
        if (target) {
            return *target;
        }

        if (! fNext) {
            fNext = new TStruct();
        }

        return fNext->Of<TStruct>();
}

template<class TGroup>
template<class TStruct>
inline const TStruct& KDExtensibleStructCore<TGroup>::Of(void) const
{
        const TStruct* target = dynamic_cast<const TStruct*>(this);
        if (target) {
            return *target;
        }

        if (fNext == 0) {
            fNext = new TStruct();
        }

        return fNext->Of<TStruct>();
}



template<class TGroup>
template<class TStruct>
inline bool KDExtensibleStructCore<TGroup>::Has(void) const
{
        if (dynamic_cast<const TStruct*>(this)) {
            return true;
        }
        if (fNext) {
            return fNext->Has<TStruct>();
        }

        return false;
}



template<class TInstance, class TGroup>
KDExtensibleStruct<TInstance, TGroup>::KDExtensibleStruct(void)
{
        fIsCopyDisabled = false;
}

template<class TInstance, class TGroup>
KDExtensibleStruct<TInstance, TGroup>::~KDExtensibleStruct()
{
        fIsCopyDisabled = false;
}

template<class TInstance, class TGroup>
KDExtensibleStruct<TInstance, TGroup>::KDExtensibleStruct(const KDExtensibleStruct<TInstance, TGroup>& object)
: KDExtensibleStructCore<TGroup>(object)
  {
        fIsCopyDisabled = false;

        if (object.fNext) {
            this->fNext = object.fNext->Clone();
        }
  }

template<class TInstance, class TGroup>
KDExtensibleStruct<TInstance, TGroup>& KDExtensibleStruct<TInstance, TGroup>::operator=(const KDExtensibleStruct<TInstance, TGroup>& object)
{
        if ((&object == this) || fIsCopyDisabled) {
            return *this;
        }

        delete this->fNext;
        this->fNext = 0;

        if (object.fNext) {
            this->fNext = object.fNext->Clone();
        }

        return *this;
}

template<class TInstance, class TGroup>
KDExtensibleStructCore<TGroup>* KDExtensibleStruct<TInstance, TGroup>::Clone(void) const
{
        // assume CRTP is used properly,
        // otherwise compiling fails here (intended behavior)
        TInstance* instance = new TInstance(dynamic_cast<const TInstance&>(*this));
        if (this->fNext) {
            instance->fNext = this->fNext->Clone();
        }
        return instance;
}

template<class TInstance, class TGroup>
void KDExtensibleStruct<TInstance, TGroup>::Pull(const KDExtensibleStructCore<TGroup>& object)
{
        if (&object == this) {
            return;
        }

        fIsCopyDisabled = true;
        TInstance* instance = dynamic_cast<TInstance*>(this);
        if (object.template Has<TInstance>()) {
            instance->operator=(object.template Of<TInstance>());
        }
        else {
            instance->operator=(TInstance());
        }
        fIsCopyDisabled = false;

        if (this->fNext) {
            this->fNext->Pull(object);
        }
}


//*******************************************
// Mapped extensible struct implementation
//*******************************************
#ifdef blah
template<class TGroup>
KDMappedExtensibleStructCore<TGroup>::KDMappedExtensibleStructCore(void) :
        fStructs(new StructMap())
{
}
/*
template<class TGroup>
KDMappedExtensibleStructCore<TGroup>::KDMappedExtensibleStructCore(const KDMappedExtensibleStructCore&) :
        fStructs()
{
}
*/
template<class TGroup>
KDMappedExtensibleStructCore<TGroup>::~KDMappedExtensibleStructCore()
{
    if (fStructs->size() == 0)
        delete fStructs;
}
/*
template<class TGroup>
KDMappedExtensibleStructCore<TGroup>& KDMappedExtensibleStructCore<TGroup>::operator=(const KDMappedExtensibleStructCore&)
{
        return *this;
}

template<class TGroup>
void KDMappedExtensibleStructCore<TGroup>::Clear(void)
{
        delete fNext;
        fNext = 0;
}
*/
template<class TGroup>
template<class TStruct>
inline TStruct& KDMappedExtensibleStructCore<TGroup>::Of(void)
{
    StructMap::iterator it = fStructs->find(&typeid(TStruct));
    if (it != fStructs->end())
    {
        return *(static_cast<TStruct*>(it->second));
        //return *static_cast<TStruct*>(it->second.get());
    }

    TStruct* newObj = new TStruct();
    fStructs->insert(StructMap::value_type(&typeid(TStruct), newObj));

    delete newObj->fStructs;
    newObj->fStructs = fStructs;

    return *(static_cast<TStruct*>(newObj));
    /*
    MESCorePtr newPtr(new TStruct());
    fStructs->insert(StructMap::value_type(&typeid(TStruct), newPtr));
    newPtr->fStructs = fStructs;

    return *static_cast<TStruct*>(newPtr.get());
    */
}

template<class TGroup>
template<class TStruct>
inline const TStruct& KDMappedExtensibleStructCore<TGroup>::Of(void) const
{
    StructMap::const_iterator it = fStructs->find(&typeid(TStruct));
    if (it != fStructs->end())
    {
        return *(static_cast<TStruct*>(it->second));
        //return *static_cast<TStruct*>(it->second.get());
    }

    TStruct* newObj = new TStruct();
    fStructs->insert(StructMap::value_type(&typeid(TStruct), newObj));

    delete newObj->fStructs;
    newObj->fStructs = fStructs;

    return *(static_cast<TStruct*>(newObj));
    /*
    MESCorePtr newPtr(new TStruct());
    fStructs->insert(StructMap::value_type(&typeid(TStruct), newPtr));
    newPtr->fStructs = fStructs;

    return *static_cast<TStruct*>(newPtr.get());
    */
}



template<class TGroup>
template<class TStruct>
inline bool KDMappedExtensibleStructCore<TGroup>::Has(void) const
{
    StructMap::const_iterator it = fStructs->find(&typeid(TStruct));
    if (it != fStructs->end())
    {
        return true;
    }
    return false;
}



template<class TInstance, class TGroup>
KDMappedExtensibleStruct<TInstance, TGroup>::KDMappedExtensibleStruct(void)
{
    //MESCorePtr ptr(this);
    //fStructs.insert(StructMap::value_type(&typeid(TInstance), ptr));
    fStructs.insert(StructMap::value_type(&typeid(TInstance), this));
    fIsCopyDisabled = false;
}

template<class TInstance, class TGroup>
KDMappedExtensibleStruct<TInstance, TGroup>::~KDMappedExtensibleStruct()
{
    fStructs.erase(&typeid(TInstance));
    fIsCopyDisabled = false;
}
/*
template<class TInstance, class TGroup>
KDMappedExtensibleStruct<TInstance, TGroup>::KDMappedExtensibleStruct(const KDMappedExtensibleStruct<TInstance, TGroup>& object) :
        KDMappedExtensibleStructCore<TGroup>(object)
{
    if (! object.fIsCopyDisabled)
    {
        fIsCopyDisabled = false;

        for (StructMap::const_iterator objectIt = fStructs.begin(); objectIt != fStructs.end(); objectIt++)
        {
            fStructs.insert(object.fStructs::value_type(objectIt->first, objectIt->second->Clone()));
        }
    }
}

template<class TInstance, class TGroup>
KDMappedExtensibleStruct<TInstance, TGroup>& KDMappedExtensibleStruct<TInstance, TGroup>::operator=(const KDMappedExtensibleStruct<TInstance, TGroup>& object)
{
    if ((&object == this) || fIsCopyDisabled) {
        return *this;
    }

    for (StructMap::const_iterator objectIt = fStructs.begin(); objectIt != fStructs.end(); objectIt++)
    {
        fStructs.insert(object.fStructs::value_type(objectIt->first, objectIt->second->Clone()));
    }

    return *this;
}

template<class TInstance, class TGroup>
KDMappedExtensibleStructCore<TGroup>* KDMappedExtensibleStruct<TInstance, TGroup>::Clone(void) const
{
        // assume CRTP is used properly,
        // otherwise compiling fails here (intended behavior)
        TInstance* instance = new TInstance(dynamic_cast<const TInstance&>(*this));
        if (this->fNext) {
            instance->fNext = this->fNext->Clone();
        }
        return instance;
}

template<class TInstance, class TGroup>
void KDMappedExtensibleStruct<TInstance, TGroup>::Pull(const KDMappedExtensibleStructCore<TGroup>& object)
{
        if (&object == this) {
            return;
        }

        fIsCopyDisabled = true;
        TInstance* instance = dynamic_cast<TInstance*>(this);
        if (object.template Has<TInstance>()) {
            instance->operator=(object.template Of<TInstance>());
        }
        else {
            instance->operator=(TInstance());
        }
        fIsCopyDisabled = false;

        if (this->fNext) {
            this->fNext->Pull(object);
        }
}
*/
#endif
