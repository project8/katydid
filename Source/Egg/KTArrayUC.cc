/*
 * KKTArrayUC.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: Rene Brun
 *      Copied from ROOT's TArrayC.cxx by N. Oblath
 */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// KTArrayUC                                                            //
//                                                                      //
// Array of unsigned chars or bytes (8 bits per element).               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "KTArrayUC.hh"
#include "TBuffer.h"


ClassImp(/*Katydid::*/KTArrayUC)

//namespace Katydid
//{
    //______________________________________________________________________________
    KTArrayUC::KTArrayUC()
    {
       // Default KTArrayUC ctor.

       fArray = 0;
    }

    //______________________________________________________________________________
    KTArrayUC::KTArrayUC(Int_t n)
    {
       // Create KTArrayUC object and set array size to n unsigned chars.

       fArray = 0;
       if (n > 0) Set(n);
    }

    //______________________________________________________________________________
    KTArrayUC::KTArrayUC(Int_t n, const UChar_t *array)
    {
       // Create KTArrayUC object and initialize it with values of array.

       fArray = 0;
       Set(n, array);
    }

    //______________________________________________________________________________
    KTArrayUC::KTArrayUC(const KTArrayUC &array) : TArray(array)
    {
       // Copy constructor.

       fArray = 0;
       Set(array.fN, array.fArray);
    }

    //______________________________________________________________________________
    KTArrayUC &KTArrayUC::operator=(const KTArrayUC &rhs)
    {
       // KTArrayUC assignment operator.

       if (this != &rhs)
          Set(rhs.fN, rhs.fArray);
       return *this;
    }

    //______________________________________________________________________________
    KTArrayUC::~KTArrayUC()
    {
       // Delete KTArrayUC object.

       delete [] fArray;
       fArray = 0;
    }

    //______________________________________________________________________________
    void KTArrayUC::Adopt(Int_t n, UChar_t *arr)
    {
       // Adopt array arr into KTArrayUC, i.e. don't copy arr but use it directly
       // in KTArrayUC. User may not delete arr, KTArrayUC dtor will do it.

       if (fArray)
          delete [] fArray;

       fN     = n;
       fArray = arr;
    }

    //______________________________________________________________________________
    void KTArrayUC::AddAt(UChar_t c, Int_t i)
    {
       // Add unsigned char c at position i. Check for out of bounds.

       if (!BoundsOk("KTArrayUC::AddAt", i)) return;
       fArray[i] = c;
    }

    //______________________________________________________________________________
    void KTArrayUC::Set(Int_t n)
    {
       // Set size of this array to n unsigned chars.
       // A new array is created, the old contents copied to the new array,
       // then the old array is deleted.
       // This function should not be called if the array was declared via Adopt.

       if (n < 0) return;
       if (n != fN) {
          UChar_t *temp = fArray;
          if (n != 0) {
             fArray = new UChar_t[n];
             if (n < fN) memcpy(fArray,temp, n*sizeof(UChar_t));
             else {
                memcpy(fArray,temp,fN*sizeof(UChar_t));
                memset(&fArray[fN],0,(n-fN)*sizeof(UChar_t));
             }
          } else {
             fArray = 0;
          }
          if (fN) delete [] temp;
          fN = n;
       }
    }

    //______________________________________________________________________________
    void KTArrayUC::Set(Int_t n, const UChar_t *array)
    {
       // Set size of this array to n unsigned chars and set the contents.
       // This function should not be called if the array was declared via Adopt.

       if (fArray && fN != n) {
          delete [] fArray;
          fArray = 0;
       }
       fN = n;
       if (fN == 0) return;
       if (array == 0) return;
       if (!fArray) fArray = new UChar_t[fN];
       memcpy(fArray,array, n*sizeof(UChar_t));
    }
/*
    //_______________________________________________________________________
    void KTArrayUC::Streamer(TBuffer &b)
    {
       // Stream a KTArrayUC object.

       if (b.IsReading()) {
          Int_t n;
          b >> n;
          Set(n);
          b.ReadFastArray(fArray,n);
       } else {
          b << fN;
          b.WriteFastArray(fArray, fN);
       }
    }
*/
//} /* namespace Katydid */
