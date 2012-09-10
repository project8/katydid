/*
 * MonarchPP.hh
 *
 *  Created on: Sep 10, 2012
 *      Author: nsoblath
 */

#ifndef MONARCHPP_HH_
#define MONARCHPP_HH_

#include "Monarch.hpp"

class MonarchPP : public Monarch
{
    private:
        typedef const MonarchRecord* (MonarchPP::*GetRecordFunction)() const;

    protected:
        MonarchPP();

    public:
        virtual ~MonarchPP();

    public:
        static unsigned GetMaxChannels();

    protected:
        static const unsigned fMaxChannels = 2;

        // Read Interface

    public:
        //this static method opens the file for reading.
        //if the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
        //upon successful return monarch is in the eOpen state.
        static const MonarchPP* OpenForReading(const string& filename);

        //get the pointer to the channelNum'th channel record.
        const MonarchRecord* GetRecord(unsigned channelNum) const;

    protected:
        GetRecordFunction fGRFArray[fMaxChannels];



        // Write Interface

    public:
        // Not implemented, as it's not currently used in Katydid.
};

inline const MonarchRecord* MonarchPP::GetRecord(unsigned channelNum) const
{
    return (this->*fGRFArray[channelNum])();
}

#endif /* MONARCHPP_HH_ */
