/*
 * TestNTracksNPointsNUPCut.cc
 *
 *  Created on: May 16, 2019
 *      Author: N.S. Oblath
 */

#include "KTNTracksNPointsNUPCut.hh"

using namespace Katydid;

using scarab::param_node;
using scarab::param_array;
using scarab::param_value;

int main()
{
    // First, test the rather complex configuration procedure

    // Create the config
    param_node tConfig;

    param_array tCutParams;

    param_node tOneCut;
    tOneCut.add("ft-npoints", param_value(3));
    tOneCut.add("ntracks", param_value(1));
    tOneCut.add("min-total-nup", param_value(0.));
    tOneCut.add("min-average-nup", param_value(13.));
    tOneCut.add("min-max-track-nup", param_value(0.));
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(3);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(11.);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(3);
    tOneCut.value_at("ntracks")->set(3);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(4);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(10.);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(4);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(8.5);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(5);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(5);
    tOneCut.value_at("ntracks")->set(2);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.8);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(6);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(8.5);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tOneCut.value_at("ft-npoints")->set(7);
    tOneCut.value_at("ntracks")->set(1);
    tOneCut.value_at("min-total-nup")->set(0.);
    tOneCut.value_at("min-average-nup")->set(7.3);
    tOneCut.value_at("min-max-track-nup")->set(0.);
    tCutParams.push_back(tOneCut);

    tConfig.add("parameters", tCutParams);
/*
            parameters:
              - ft-npoints: 3
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 13
                min-max-track-nup: 0
              - ft-npoints: 3
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 11
                min-max-track-nup: 0
              - ft-npoints: 3
                ntracks: 3
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-track-nup: 0
              - ft-npoints: 4
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 10
                min-max-track-nup: 0
              - ft-npoints: 4
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 8.5
                min-max-track-nup: 0
              - ft-npoints: 5
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-track-nup: 0
              - ft-npoints: 5
                ntracks: 2
                min-total-nup: 0
                min-average-nup: 7.8
                min-max-track-nup: 0
              - ft-npoints: 6
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 8.5
                min-max-track-nup: 0
              - ft-npoints: 7
                ntracks: 1
                min-total-nup: 0
                min-average-nup: 7.3
                min-max-track-nup: 0
*/

    KTNTracksNPointsNUPCut tCut;

    tCut.Configure(&tConfig);

    return 0;
}
