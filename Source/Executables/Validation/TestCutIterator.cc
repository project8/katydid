/*
 * TestCutIterator.cc
 *
 *  Created on: Dec 28, 2011
 *      Author: nsoblath
 */

#include "KTCutable.hh"

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

    KTCutable< char, char* > cutable1(testArray, testArray+testArraySize);
    KTCutable< char, char* >::iterator cutable1Iter;
    cout << "  Before the cut:" << endl;
    for (cutable1Iter = cutable1.begin(); cutable1Iter != cutable1.end(); cutable1Iter++)
    {
        cout << *cutable1Iter;
    }
    cout << endl;

    // cut out the characters in the cut positions
    for (unsigned int iCut=0; iCut<nCuts; iCut++)
    {
        cutable1.Cut(testArray+cutPositions[iCut]);
    }

    cout << "  After the cut:" << endl;
    string testArrayResult;
    bool testPassed = false;
    for (cutable1Iter = cutable1.begin(); cutable1Iter != cutable1.end(); cutable1Iter++)
    {
        cout << *cutable1Iter;
        testArrayResult += *cutable1Iter;
    }
    cout << endl;

    if (testArrayResult == testArrayIntendedResult) testPassed = true;

    cout << "  Checking \"CutAll\" and \"UnCutAll\"" << endl;
    cutable1.CutAll();
    cout << "  After CutAll:" << endl;
    cout << "size = " << cutable1.size() << endl;
    if (! testPassed || cutable1.size() != 0) testPassed = false;

    cutable1.UnCutAll();
    testArrayResult = string();
    cout << "  After UnCutAll:" << endl;
    for (cutable1Iter = cutable1.begin(); cutable1Iter != cutable1.end(); cutable1Iter++)
    {
        cout << *cutable1Iter;
        testArrayResult += *cutable1Iter;
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

    list< char >::iterator startCut = testList.begin();
    list< char >::iterator endCut = testList.begin();
    for (unsigned int iPos=0; iPos<12; iPos++) startCut++;
    for (unsigned int iPos=0; iPos<20; iPos++) endCut++;

    list< char >::iterator cut2 = testList.begin();
    for (unsigned int iPos=0; iPos<32; iPos++) cut2++;

    KTCutable< char, list< char >::iterator > cutable2(testList.begin(), testList.end());
    KTCutable< char, list< char >::iterator >::iterator cutable2Iter;
    cout << "  Before the first cut:" << endl;
    for (cutable2Iter = cutable2.begin(); cutable2Iter != cutable2.end(); cutable2Iter++)
    {
        cout << *cutable2Iter;
    }
    cout << endl;

    cutable2.Cut(startCut, endCut);
    bool testListPassed = true;
    string testListResult;

    cout << "  After the first cut:" << endl;
    string testListIntendedResult("The list test passed too!");
    for (cutable2Iter = cutable2.begin(); cutable2Iter != cutable2.end(); cutable2Iter++)
    {
        cout << *cutable2Iter;
        testListResult += *cutable2Iter;
    }
    cout << endl;
    if (testListResult != testListIntendedResult) testListPassed = false;

    cutable2.Cut(cut2);

    cout << "  After the second cut:" << endl;
    testListResult = string();
    testListIntendedResult = string("The list test passed too");
    for (cutable2Iter = cutable2.begin(); cutable2Iter != cutable2.end(); cutable2Iter++)
    {
        cout << *cutable2Iter;
        testListResult += *cutable2Iter;
    }
    cout << endl;
    if (! testListPassed || testListResult != testListIntendedResult) testListPassed = false;

    cutable2.UnCut(startCut, endCut);
    cutable2.UnCut(cut2);

    cout << "  After the un-cut:" << endl;
    testListResult = string();
    for (cutable2Iter = cutable2.begin(); cutable2Iter != cutable2.end(); cutable2Iter++)
    {
        cout << *cutable2Iter;
        testListResult += *cutable2Iter;
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
