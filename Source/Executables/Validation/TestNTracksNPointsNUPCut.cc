/*
 * TestNTracksNPointsNUPCut.cc
 *
 *  Created on: May 16, 2019
 *      Author: N.S. Oblath
 */

#include "KTNTracksNPointsNUPCut.hh"

#include "KTMultiTrackEventData.hh"

#include "logger.hh"

using namespace Katydid;

using scarab::param_node;
using scarab::param_array;
using scarab::param_value;

using Nymph::KTDataPtr;
using Nymph::KTData;

LOGGER(testlog, "TestNTracksNPointsNUPCut");

int main()
{
    // First, test the rather complex configuration procedure

    // Create the config
    param_node tConfig;

    param_node tDefaultParams;
    tDefaultParams.add("min-total-nup", param_value(0.));
    tDefaultParams.add("min-average-nup", param_value(2.));
    tDefaultParams.add("min-max-nup", param_value(0.));

    tConfig.add("default-parameters", tDefaultParams);

    param_array tCutParams;

    param_node tOneCut;
    tOneCut.add("ft-npoints", param_value(3));
    tOneCut.add("ntracks", param_value(1));
    tOneCut.add("min-total-nup", param_value(0.));
    tOneCut.add("min-average-nup", param_value(13.));
    tOneCut.add("min-max-nup", param_value(0.));
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(3);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(11.);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(3);
    tOneCut.value_at("ntracks")->set(3);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(4);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(10.);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(4);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(8.5);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(5);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(5);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(6);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(8.5);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(7);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.3);
    tOneCut.value_at("min-max-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tConfig.add("parameters", tCutParams);
/*
            default-parameters:
                min-total-nup: 0
                min-average-nup: 2
                min-max-nup: 0

            parameters:
              - ft-npoints: 3
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 13
                min-max-nup: 0
              - ft-npoints: 3
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 11
                min-max-nup: 0
              - ft-npoints: 3
                ntracks: 3
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-nup: 0
              - ft-npoints: 4
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 10
                min-max-nup: 0
              - ft-npoints: 4
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 8.5
                min-max-nup: 0
              - ft-npoints: 5
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-nup: 0
              - ft-npoints: 5
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-nup: 0
              - ft-npoints: 6
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 8.5
                min-max-nup: 0
              - ft-npoints: 7
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 7.3
                min-max-nup: 0
*/

    // Create and configure the cut
    KTPROG(testlog, "Creating and configuring the processor");
    KTNTracksNPointsNUPCut tCut;
    tCut.Configure(&tConfig);

    // Second, test the application of the cut to some data

    // Create some data
    KTDataPtr dataPtr(new KTData());
    KTData& data = dataPtr->Of< KTData >();
    KTMultiTrackEventData& mteData = dataPtr->Of< KTMultiTrackEventData >();

    // Minimal set of data needed for the cut
    KTPROG(testlog, "Testing data inside the parameter array; should be cut by average NUP");
    mteData.SetTotalEventSequences(1);
    mteData.SetFirstTrackNTrackBins(4);
    mteData.SetFirstTrackTotalNUP(1.);
    mteData.SetFirstTrackTotalWideNUP(1.);
    mteData.SetFirstTrackTimeLength(1.);
    mteData.SetFirstTrackMaxNUP(5.);

    tCut.Apply(data, mteData);

    KTPROG(testlog, "Testing data inside the parameter array; should pass cuts");
    mteData.SetTotalEventSequences(3);
    mteData.SetFirstTrackNTrackBins(1);
    mteData.SetFirstTrackTotalNUP(20.);
    mteData.SetFirstTrackTotalWideNUP(20.);

    tCut.Apply(data, mteData);

    KTPROG(testlog, "Testing data outside the parameter array (ntracks too large); should pass cuts");
    mteData.SetTotalEventSequences(4);
    mteData.SetFirstTrackNTrackBins(1);
    mteData.SetFirstTrackTotalNUP(20.);
    mteData.SetFirstTrackTotalWideNUP(20.);

    tCut.Apply(data, mteData);

    KTPROG(testlog, "Testing data outside the parameter array (npoints too large); should pass cuts");
    mteData.SetTotalEventSequences(3);
    mteData.SetFirstTrackNTrackBins(10);
    mteData.SetFirstTrackTotalNUP(20.);
    mteData.SetFirstTrackTotalWideNUP(20.);

    tCut.Apply(data, mteData);

    KTPROG(testlog, "Testing data outside the parameter array (npoints too large); should be cut by average NUP");
    mteData.SetTotalEventSequences(80);
    mteData.SetFirstTrackNTrackBins(100);
    mteData.SetFirstTrackTotalNUP(1.);
    mteData.SetFirstTrackTotalWideNUP(1.);

    tCut.Apply(data, mteData);

    return 0;
}
