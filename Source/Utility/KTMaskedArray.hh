/**
 @file KTMaskedArray.hh
 @brief Contains KTMaskedArray
 @details KTMaskedArray is an array wrapper that lets you cut out bins temporarily
 @author: N. S. Oblath
 @date: Mar 13, 2012
 */


#ifndef KTMASKEDARRAY_HH_
#define KTMASKEDARRAY_HH_

#include <algorithm>
#include <vector>

namespace Katydid
{
    using namespace Nymph;
    /*!
     @class KTMaskedArray
     @author N. S. Oblath

     @brief An array wrapper that lets you cut out bins temporarily.

     @details
     KTMaskedArray wraps an array and allows you to temporarily cut out bins.  In other words, you can place a cut on the
     array without affecting the array itself.  You can then iterate over the un-cut array positions.

     There are two template arrays: the array type and the type of object stored in the array.
     It is assumed that the array has a member function operator[](unsigned) to access the elements.

     As with KTCutable and KTCutableArray, KTMaskedArray is a pseudo-container, which doesn't actually own the data to which it refers.
     If the underlying container class changes or moves, any existing KTCutableArray will need to be reset.

     The API is similar to KTCutable and KTCutableArray, though the classes are not actually related.
    */
    template< typename XArrayType, typename XArrayObjectType >
    class KTMaskedArray
    {
        private:
            typedef std::vector< unsigned int > PositionStore;
            typedef PositionStore::iterator PositionStoreIt;

        public:
            KTMaskedArray();
            KTMaskedArray(XArrayType array, unsigned int size);
            KTMaskedArray(const KTMaskedArray< XArrayType, XArrayObjectType >& orig);
            ~KTMaskedArray();

            const KTMaskedArray< XArrayType, XArrayObjectType > operator=(const KTMaskedArray< XArrayType, XArrayObjectType >& orig);

        public:
            XArrayObjectType& operator[](unsigned int pos)
                {return fArray[fPositions[pos]];}
            const XArrayObjectType& operator[](unsigned int pos) const
                {return fArray[fPositions[pos]];}

            void Cut(unsigned int toCut);
            void UnCut(unsigned int toUnCut);

            void Cut(unsigned int toBeginCut, unsigned int nToCut);
            void UnCut(unsigned int toBeginUnCut, unsigned int nToUnCut);

            void CutAll();
            void UnCutAll();

            void ChangeArray(XArrayType array)
                {fArray = array;}

            const XArrayObjectType* GetArray() const
                {return fArray;}
            unsigned int GetArraySize() const
                {return fArraySize;}

            unsigned int size() const
                {return fPositions.size();}

            unsigned int GetArrayPosition(unsigned int unCutPosition)
                {return fPositions[unCutPosition];}
            unsigned int FindCutPosition(unsigned int arrayPosition);
            unsigned int FindCutPositionOrNext(unsigned int arrayPosition);

        private:
            XArrayType fArray;
            unsigned int fArraySize;

            PositionStore fPositions;
    };


    template< typename XArrayType, typename XArrayObjectType >
    KTMaskedArray< XArrayType, XArrayObjectType >::KTMaskedArray() :
        fArray(NULL),
        fArraySize(0),
        fPositions()
    {
    }

    template< typename XArrayType, typename XArrayObjectType >
    KTMaskedArray< XArrayType, XArrayObjectType >::KTMaskedArray(XArrayType array, unsigned int size) :
        fArray(array),
        fArraySize(size),
        fPositions(size)
    {
        for (unsigned int iPos=0; iPos<size; iPos++)
        {
            fPositions[iPos] = iPos;
        }
    }

    template< typename XArrayType, typename XArrayObjectType >
    KTMaskedArray< XArrayType, XArrayObjectType >::KTMaskedArray(const KTMaskedArray< XArrayType, XArrayObjectType >& orig) :
        fArray(orig.fArray),
        fArraySize(orig.fArraySize),
        fPositions(orig.fPositions)
    {
    }

    template< typename XArrayType, typename XArrayObjectType >
    KTMaskedArray< XArrayType, XArrayObjectType >::~KTMaskedArray()
    {
    }

    template< typename XArrayType, typename XArrayObjectType >
    const KTMaskedArray< XArrayType, XArrayObjectType > KTMaskedArray< XArrayType, XArrayObjectType >::operator=(const KTMaskedArray< XArrayType, XArrayObjectType >& orig)
    {
        fArray = orig.fArray;
        fArraySize = orig.fArraySize;
        fPositions = orig.fPositions;
    }

    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::Cut(unsigned int toCut)
    {
        PositionStoreIt posInPositions = std::lower_bound(fPositions.begin(), fPositions.end(), toCut);
        if (posInPositions == fPositions.end()) return;
        if (*posInPositions == toCut) fPositions.erase(posInPositions);
        return;
    }
    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::UnCut(unsigned int toUnCut)
    {
        PositionStoreIt posInPositions = std::lower_bound(fPositions.begin(), fPositions.end(), toUnCut);
        if (posInPositions == fPositions.end())
        {
            fPositions.insert(posInPositions, toUnCut);
            return;
        }
        if (*posInPositions != toUnCut) fPositions.insert(posInPositions, toUnCut);
        return;
    }

    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::Cut(unsigned int toBeginCut, unsigned int nToCut)
    {
        // cache the start position for the search to save time
        PositionStoreIt startSearch = fPositions.begin();
        // the condition for staying in the loop is that we haven't yet reached nToCut AND that the requested toCut is less than the array size
        for (unsigned int toCut=toBeginCut; toCut<fArraySize && toCut-toBeginCut<nToCut; toCut++)
        {
            // look for the first position in fPositions that is >= toCut
            PositionStoreIt posInPositions = std::lower_bound(startSearch, fPositions.end(), toCut);
            // if we've reached the end of fPositions, then we're done
            if (posInPositions == fPositions.end()) break;
            // if the position matches the requested cut (this will be false if the requested cut has already been cut), perform the cut
            // the return value, which is an iterator to the new location of the element that followed the erased element, is set as the new startSearch
            if (*posInPositions == toCut) startSearch = fPositions.erase(posInPositions);
        }
        return;
    }
    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::UnCut(unsigned int toBeginUnCut, unsigned int nToUnCut)
    {
        // if we might need to reallocate the vector because the number of requested uncut positions would result in
        // fPositions exceeding its capacity, do the reallocation first so we don't invalidate iterators
        if (fPositions.size() + nToUnCut > fPositions.capacity()) fPositions.reserve(fPositions.size() + nToUnCut);

        // cache the start position for the search to save time
        PositionStoreIt startSearch = fPositions.begin();
        // the condition for staying in the loop is that we haven't yet reached nToUnCut AND that the requested toUnCut is less than the array size
        for (unsigned int toUnCut=toBeginUnCut; toUnCut<fArraySize && toUnCut-toBeginUnCut<nToUnCut; toUnCut++)
        {
            // look for the first position in fPositions that is >= toUnCut
            PositionStoreIt posInPositions = std::lower_bound(startSearch, fPositions.end(), toUnCut);
            // if we've reached the end of fPositions, we can add toUnCut;
            // startSearch is set to the end of fPositions because this is guaranteed since fPositions is ordered
            if (posInPositions == fPositions.end())
            {
                fPositions.insert(posInPositions, toUnCut);
                startSearch = fPositions.end();
                continue;
            }
            // if the requested toUnCut is not in fPositions already, add it
            // if the insertion is made, the position of the inserted element is the new startSearch
            // otherwise posInPositions is the new startSearch
            if (*posInPositions != toUnCut) startSearch = fPositions.insert(posInPositions, toUnCut);
            else startSearch = posInPositions;
        }
        return;
    }

    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::CutAll()
    {
        fPositions.clear();
        return;
    }

    template< typename XArrayType, typename XArrayObjectType >
    void KTMaskedArray< XArrayType, XArrayObjectType >::UnCutAll()
    {
        fPositions.clear();
        fPositions.resize(fArraySize);
        for (unsigned int iPos=0; iPos<fArraySize; iPos++)
        {
            fPositions[iPos] = iPos;
        }
        return;
    }

    template< typename XArrayType, typename XArrayObjectType >
    unsigned int KTMaskedArray< XArrayType, XArrayObjectType >::FindCutPosition(unsigned int arrayPosition)
    {
        PositionStoreIt posInPositions = std::lower_bound(fPositions.begin(), fPositions.end(), arrayPosition);
        if (posInPositions == fPositions.end()) return fArraySize;
        if (*posInPositions != arrayPosition) return fArraySize;
        return (unsigned int)(posInPositions - fPositions.begin());
    }

    template< typename XArrayType, typename XArrayObjectType >
    unsigned int KTMaskedArray< XArrayType, XArrayObjectType >::FindCutPositionOrNext(unsigned int arrayPosition)
    {
        PositionStoreIt posInPositions = std::lower_bound(fPositions.begin(), fPositions.end(), arrayPosition);
        return (unsigned int)(posInPositions - fPositions.begin());
    }



} /* namespace Katydid */
#endif /* KTMASKEDARRAY_HH_ */
