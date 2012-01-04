/*
 * TestCutableArray.cc
 *
 *  Created on: Jan 3, 2012
 *      Author: nsoblath
 */


#include "KTCutableArray.hh"

#include <cstdio>
#include <cstring>s
#include <iostream>
#include <list>
#include <utility>
#include <string>

using namespace Katydid;
using namespace std;

int main(int argc, char** argv)
{
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
        cout << "List test completed" << endl;
    }
    else
    {
        cout << "List test failed" << endl;
        return -1;
    }

    return 0;
}


