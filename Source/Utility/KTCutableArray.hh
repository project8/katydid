/**
 @file KTCutableArray.hh
 @brief Contains KTCutableArray
 @details KTCutableArray is a convenient adaptation of KTCutable to fixed-length arrays
 @author: N. S. Oblath
 @date: Jan 3, 2012
 */

#ifndef KTCUTABLEARRAY_HH_
#define KTCUTABLEARRAY_HH_

#include "KTCutable.hh"

#include <vector>

namespace Katydid
{
    //using namespace Nymph;
    /*!
     @class KTCutableArray
     @author N. S. Oblath

     @brief A convenient adaptation of KTCutable to fixed-length arrays.

     @details
     KTCutableArray adapts KTCutable to fixed-length arrays and vectors.  The API is similar to that of arrays and vectors, using operator[] to dereference;
     Other containers can be used as the underlying data storage.
     As with KTCutable, KTCutableArray is a pseudo-container, which doesn't actually own the data to which it refers.
     If the underlying container class changes or moves, any existing KTCutableArray will need to be reset.
    */
    template< class XArrayObjectType, class XArrayIteratorType >
    class KTCutableArray
    {
        private:
            typedef typename std::vector< XArrayIteratorType > IntermediateVector;
            typedef KTCutable< XArrayIteratorType, typename IntermediateVector::iterator > Cutable;
            typedef typename std::vector< typename Cutable::iterator > CutIteratorVector;

        public:
            KTCutableArray();
            KTCutableArray(XArrayIteratorType begin, unsigned int size); /// construct with the pointer to the beginning and the size
            KTCutableArray(XArrayIteratorType begin, XArrayIteratorType end); /// construct with begin and end iterators
            KTCutableArray(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig); /// copy constructor
            ~KTCutableArray();

        public:
            const KTCutableArray< XArrayObjectType, XArrayIteratorType >& operator=(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig);

        public:
            XArrayObjectType& operator[](unsigned int pos)
                {return *(*(fCutIterators[pos]));}
            const XArrayObjectType& operator[](unsigned int pos) const
                {return *(*(fCutIterators[pos]));}

            const XArrayIteratorType& GetArrayBegin() const
                {return fBegin;}
            const XArrayIteratorType& GetArrayEnd() const
                {return fEnd;}

            void Clear();
            void ResetArray(XArrayIteratorType begin, unsigned int size);
            void ResetArray(XArrayIteratorType begin, XArrayIteratorType end);
            void ResetArray(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig);

            void ResetCutable();
            void ResetCutIterators();

        private:
            IntermediateVector fIntermediateData;
            Cutable fCutable;
            CutIteratorVector fCutIterators;

            XArrayIteratorType fBegin;
            XArrayIteratorType fEnd;

        public:
            // iterator-based cuts will be implemented if an efficient way to do so is found
            // probably it could be done with a hash table if the methods are found to be useful
            //void Cut(XArrayIteratorType toCut);
            //void UnCut(XArrayIteratorType toUnCut);

            //void Cut(XArrayIteratorType toBeginCut, XArrayIteratorType toEndCut);
            //void UnCut(XArrayIteratorType toBeginCut, XArrayIteratorType toEndCut);

            void Cut(unsigned int toCut, bool resetCutIters=true);
            void UnCut(unsigned int toUnCut, bool resetCutIters=true);

            void Cut(unsigned int toBeginCut, unsigned int toEndCut, bool resetCutIters=true);
            void UnCut(unsigned int toBeginUnCut, unsigned int toEndUnCut, bool resetCutIters=true);

            void CutAll(bool resetCutIters=true);
            void UnCutAll(bool resetCutIters=true);

        public:
            std::size_t size() const
                {return fCutable.size();}
    };

    template< class XArrayObjectType, class XArrayIteratorType >
    KTCutableArray< XArrayObjectType, XArrayIteratorType >::KTCutableArray() :
            fIntermediateData(),
            fCutable(),
            fCutIterators(),
            fBegin(),
            fEnd()
    {
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    KTCutableArray< XArrayObjectType, XArrayIteratorType >::KTCutableArray(XArrayIteratorType begin, unsigned int size) :
            fIntermediateData(),
            fCutable(),
            fCutIterators(),
            fBegin(begin),
            fEnd()
    {
        XArrayIteratorType iter = begin;
        for (unsigned int count=0; count != size; count++)
        {
            fIntermediateData.push_back(iter);
            iter++;
        }
        fEnd = iter;
        ResetCutable();
        ResetCutIterators();
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    KTCutableArray< XArrayObjectType, XArrayIteratorType >::KTCutableArray(XArrayIteratorType begin, XArrayIteratorType end) :
            fIntermediateData(),
            fCutable(),
            fCutIterators(),
            fBegin(begin),
            fEnd(end)
    {
        for (XArrayIteratorType iter=begin; iter != end; iter++)
        {
            fIntermediateData.push_back(iter);
        }
        ResetCutable();
        ResetCutIterators();
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    KTCutableArray< XArrayObjectType, XArrayIteratorType >::KTCutableArray(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig) :
            fIntermediateData(orig.fIntermediateData),
            fCutable(orig.fCutable),
            fCutIterators(),
            fBegin(orig.fBegin),
            fEnd(orig.fEnd)
    {
        ResetCutIterators();
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    KTCutableArray< XArrayObjectType, XArrayIteratorType >::~KTCutableArray()
    {
    }


    template< class XArrayObjectType, class XArrayIteratorType >
    const KTCutableArray< XArrayObjectType, XArrayIteratorType >& KTCutableArray< XArrayObjectType, XArrayIteratorType >::operator=(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig)
    {
        fIntermediateData = orig.fIntermediateData;
        fCutable = orig.fCutable;
        fBegin = orig.fBegin;
        fEnd = orig.fEnd;
        ResetCutIterators();
        return *this;
    }


    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::Clear()
    {
        fIntermediateData.clear();
        fCutable.Clear();
        fBegin = XArrayIteratorType();
        fEnd = XArrayIteratorType();
        fCutIterators.clear();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::ResetArray(XArrayIteratorType begin, unsigned int size)
    {
        fBegin = begin;
        XArrayIteratorType iter = begin;
        for (unsigned int count=0; count != size; count++)
        {
            fIntermediateData.push_back(iter);
            iter++;
        }
        fEnd = iter;
        ResetCutable();
        ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::ResetArray(XArrayIteratorType begin, XArrayIteratorType end)
    {
        fBegin = begin;
        fEnd = end;
        for (XArrayIteratorType iter=fBegin; iter != fEnd; iter++)
        {
            fIntermediateData.push_back(iter);
        }
        ResetCutable();
        ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::ResetArray(const KTCutableArray< XArrayObjectType, XArrayIteratorType >& orig)
    {
        fIntermediateData = orig.fIntermediateData;
        fCutable.ResetRange(fIntermediateData.begin(), fIntermediateData.end());
        fBegin = orig.fBegin;
        fEnd = orig.fEnd;
        ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::ResetCutable()
    {
        fCutable.ResetRange(fIntermediateData.begin(), fIntermediateData.end());
        return;
    }
    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::ResetCutIterators()
    {
        fCutIterators.clear();
        for (typename Cutable::iterator iter=fCutable.begin(); iter != fCutable.end(); iter++)
        {
            fCutIterators.push_back(iter);
        }
        return;
    }


    /*
    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::Cut(XArrayIteratorType toCut)
    {
        IntermediateVector::iterator vBegin = fIntermediateData.begin();
        IntermediateVector::iterator vIter = vBegin;
        for (vIter = vBegin; *vIter != toCut && vIter-vBegin < fIntermediateData.max_size(); vIter++);
        if (*vIter == toCut) fCutable.Cut(vIter);
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::UnCut(XArrayIteratorType toUnCut)
    {
    }


    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::Cut(XArrayIteratorType toBeginCut, XArrayIteratorType toEndCut)
    {
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::UnCut(XArrayIteratorType toBeginCut, XArrayIteratorType toEndCut)
    {
    }
    */

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::Cut(unsigned int toCut, bool resetCutIters)
    {
        fCutable.Cut(fIntermediateData.begin()+toCut);
        if (resetCutIters) ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::UnCut(unsigned int toUnCut, bool resetCutIters)
    {
        fCutable.UnCut(fIntermediateData.begin()+toUnCut);
        if (resetCutIters) ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::Cut(unsigned int toBeginCut, unsigned int toEndCut, bool resetCutIters)
    {
        fCutable.Cut(fIntermediateData.begin()+toBeginCut, fIntermediateData.begin()+toEndCut);
        if (resetCutIters) ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::UnCut(unsigned int toBeginUnCut, unsigned int toEndUnCut, bool resetCutIters)
    {
        fCutable.UnCut(fIntermediateData.begin()+toBeginUnCut, fIntermediateData.begin()+toEndUnCut);
        if (resetCutIters) ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::CutAll(bool resetCutIters)
    {
        fCutable.CutAll();
        if (resetCutIters) ResetCutIterators();
        return;
    }

    template< class XArrayObjectType, class XArrayIteratorType >
    void KTCutableArray< XArrayObjectType, XArrayIteratorType >::UnCutAll(bool resetCutIters)
    {
        fCutable.UnCutAll();
        if (resetCutIters) ResetCutIterators();
        return;
    }


} /* namespace Katydid */
#endif /* KTCUTABLEARRAY_HH_ */
