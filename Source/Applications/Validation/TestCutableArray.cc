/*
 * TestCutableArray.cc
 *
 * Tests KTCutableArray and KTMaskedArray
 *
 *  Created on: Jan 3, 2012
 *      Author: nsoblath
 */


#include "KTCutableArray.hh"
#include "KTMaskedArray.hh"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <utility>
#include <string>

using namespace Katydid;
using namespace std;

int main()
{
    //********************
    // Test KTCutableArray
    //********************
    cout << "Testing KTCutableArray" << endl;

    //*******************
    // test with an array
    //*******************
    cout << "Testing with an array" << endl;

    const unsigned int testArraySize = 30;
    char* testArray = new char [testArraySize];
    strncpy(testArray, "The t#es##t was suc#c#essful#!", testArraySize);
    const unsigned int nCuts = 6;
    int cutPositions [nCuts] = {5,8,9,19,21,28};
    string testArrayOriginal(testArray, testArraySize);
    string testArrayIntendedResult("The test was successful!");

    KTCutableArray< char, char* > cutableArray1(testArray, testArraySize);
    cout << "  Before the cut:" << endl;
    for (unsigned int pos=0; pos != cutableArray1.size(); pos++)
    {
        cout << cutableArray1[pos];
    }
    cout << endl;

    // cut out the characters in the cut positions
    for (unsigned int iCut=0; iCut<nCuts; iCut++)
    {
        cutableArray1.Cut(cutPositions[iCut]);
    }

    cout << "  After the cut:" << endl;
    string testArrayResult;
    bool testPassed = false;
    for (unsigned int pos=0; pos != cutableArray1.size(); pos++)
    {
        cout << cutableArray1[pos];
        testArrayResult += cutableArray1[pos];
    }
    cout << endl;

    if (testArrayResult == testArrayIntendedResult) testPassed = true;

    cout << "  Checking \"CutAll\" and \"UnCutAll\"" << endl;
    cutableArray1.CutAll();
    cout << "  After CutAll:" << endl;
    cout << "size = " << cutableArray1.size() << endl;
    if (! testPassed || cutableArray1.size() != 0) testPassed = false;

    cutableArray1.UnCutAll();
    testArrayResult = string();
    cout << "  After UnCutAll:" << endl;
    for (unsigned int pos=0; pos != cutableArray1.size(); pos++)
    {
        cout << cutableArray1[pos];
        testArrayResult += cutableArray1[pos];
    }
    cout << endl;
    if (! testPassed || testArrayResult != testArrayOriginal) testPassed = false;

    if (testPassed)
    {
        cout << "Array test completed\n" << endl;
    }
    else
    {
        cout << "Array test failed" << endl;
        return -1;
    }


    //*****************
    // test with a list
    //*****************
    cout << "Testing with a list" << endl;

    const unsigned int testListSize = 33;
    char* testListCharacters = new char [testListSize];
    strncpy(testListCharacters, "The list tes[@#*%&$]t passed too!", testListSize);
    string testListOriginal(testListCharacters, testListSize);
    list< char > testList;
    for (unsigned int iPos=0; iPos<testListSize; iPos++)
    {
        testList.push_back(testListCharacters[iPos]);
    }

    unsigned int startCut = 12;
    unsigned int endCut = 20;

    unsigned int cut2=32;

    KTCutableArray< char, list< char >::iterator > cutableArray2(testList.begin(), testList.end());
    cout << "  Before the first cut:" << endl;
    for (unsigned int pos=0; pos != cutableArray2.size(); pos++)
    {
        cout << cutableArray2[pos];
    }
    cout << endl;

    cutableArray2.Cut(startCut, endCut);
    bool testListPassed = true;
    string testListResult;

    cout << "  After the first cut:" << endl;
    string testListIntendedResult("The list test passed too!");
    for (unsigned int pos=0; pos != cutableArray2.size(); pos++)
    {
        cout << cutableArray2[pos];
        testListResult += cutableArray2[pos];
    }
    cout << endl;
    if (testListResult != testListIntendedResult) testListPassed = false;

    cutableArray2.Cut(cut2);

    cout << "  After the second cut:" << endl;
    testListResult = string();
    testListIntendedResult = string("The list test passed too");
    for (unsigned int pos=0; pos != cutableArray2.size(); pos++)
    {
        cout << cutableArray2[pos];
        testListResult += cutableArray2[pos];
    }
    cout << endl;
    if (! testListPassed || testListResult != testListIntendedResult) testListPassed = false;

    cutableArray2.UnCut(startCut, endCut);
    cutableArray2.UnCut(cut2);

    cout << "  After the un-cut:" << endl;
    testListResult = string();
    for (unsigned int pos=0; pos != cutableArray2.size(); pos++)
    {
        cout << cutableArray2[pos];
        testListResult += cutableArray2[pos];
    }
    cout << endl;
    if (! testListPassed || testListResult != testListOriginal) testListPassed = false;

    if (testListPassed)
    {
        cout << "List test completed\n" << endl;
    }
    else
    {
        cout << "List test failed" << endl;
        return -1;
    }


    //*******************
    // Test KTMaskedArray
    //*******************
    cout << "Test KTMaskedArray" << endl;

    const unsigned int testMaskedArraySize = 10;
    int testMaskedArray[testArraySize] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    const unsigned int cut1Size = 1;
    unsigned int cut1Pos = 2;
    int cut1Check[testArraySize-cut1Size] = {1, 2, 4, 5, 6, 7, 8, 9, 10};

    const unsigned int cut2Size = 3;
    unsigned int cut2StartPos = 5;
    int cut2Check[testArraySize-cut1Size-cut2Size] = {1, 2, 4, 5, 9, 10};

    KTMaskedArray< int*, int > maskedArray(testMaskedArray, testMaskedArraySize);
    cout << "Before the cut:" << endl;
    for (unsigned int iPos=0; iPos<maskedArray.size(); iPos++)
    {
        cout << "  " << maskedArray[iPos];
    }
    cout << endl;

    bool testMaskedArrayPassed = true;

    maskedArray.Cut(cut1Pos);
    cout << "Test 1:" << endl;
    for (unsigned int iPos=0; iPos<maskedArray.size(); iPos++)
    {
        cout << "  " << maskedArray[iPos];
        if (maskedArray[iPos] != cut1Check[iPos]) testMaskedArrayPassed = false;
    }
    cout << endl;
    if (testMaskedArrayPassed == false)
    {
        cout << "Test 1 failed" << endl;
        return -1;
    }
    cout << "Passed!" << endl;

    maskedArray.Cut(cut2StartPos, cut2Size);
    cout << "Test 2:" << endl;
    for (unsigned int iPos=0; iPos<maskedArray.size(); iPos++)
    {
        cout << "  " << maskedArray[iPos];
        if (maskedArray[iPos] != cut2Check[iPos]) testMaskedArrayPassed = false;
    }
    cout << endl;
    if (testMaskedArrayPassed == false)
    {
        cout << "Test 2 failed" << endl;
        return -1;
    }
    cout << "Passed!" << endl;

    maskedArray.UnCut(cut2StartPos, cut2Size);
    cout << "Undo Test2:" << endl;
    for (unsigned int iPos=0; iPos<maskedArray.size(); iPos++)
    {
        cout << "  " << maskedArray[iPos];
        if (maskedArray[iPos] != cut1Check[iPos]) testMaskedArrayPassed = false;
    }
    cout << endl;
    if (testMaskedArrayPassed == false)
    {
        cout << "Undo test 2 failed" << endl;
        return -1;
    }
    cout << "Passed!" << endl;

    cout << "Masked array test completed" << endl;

    return 0;
}


