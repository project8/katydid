/*
 * MonarchPP.cc
 *
 *  Created on: Sep 10, 2012
 *      Author: nsoblath
 */

#include "MonarchPP.hh"

MonarchPP::MonarchPP() :
        Monarch()
{
    fGRFArray[0] = &MonarchPP::GetRecordOne;
    fGRFArray[1] = &MonarchPP::GetRecordTwo;
}

MonarchPP::~MonarchPP()
{
}

unsigned MonarchPP::GetMaxChannels()
{
    return fMaxChannels;
}

const MonarchPP* MonarchPP::OpenForReading( const string& aFilename )
{
    MonarchPP* tMonarch = new MonarchPP();

    tMonarch->fIO = new MonarchIO( sReadMode );
    if( tMonarch->fIO->Open( aFilename ) == false )
    {
        cout << "could not open <" << aFilename << "> for reading" << endl;
        delete tMonarch;
        return NULL;
    }

    tMonarch->fHeader = new MonarchHeader();
    tMonarch->fHeader->SetFilename( aFilename );

    tMonarch->fState = eOpen;
    return tMonarch;
}

