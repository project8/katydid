/*
 * TestLinearDensityProbe.cc
 *
 *  Created on: Apr 12, 2016
 *      Author: ezayas 
 */

#include "KTLinearDensityProbeFit.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTPowerSpectrum.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"
#include "KTLogger.hh"
#include <vector>

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TFile.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestLinearDensityProbe");

// Method to determine whether a line intersects a bin with finite size
// The line extends over a finite segment from (x1,y1) to (x2,y2)
// The bin has lower-left corner (xx1,yy1) and upper-right corner (xx2,yy2)
// It follows that the lower-right corner is (xx2,yy1) and the upper-left corner is (xx1,yy2)
bool lineIntersects( double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2 )
{
    // If the x or y coordinates of the line are entirely outside the bin range, we can immediately return false
    if( x2 <= xx1 || x1 >= xx2 )
        return false;
    if( y2 <= yy1 || y1 >= yy2 )
        return false;

    double m = (y2 - y1) / (x2 - x1);       // slope
    double yLeft = y1 + m * (xx1 - x1);     // y-position of the line at xx1
    double yRight = y1 + m * (xx2 - x1);    // y-position of the line at xx2

    if( m > 0 )
    {
        // Positive slope
        // We check the upper-left corner and the lower-right corner
        if( yLeft >= yy2 || yRight <= yy1 )
            return false;
        else
            return true;
    }
    else
    {
        // Negative slope
        // We check the lower-left corner and the upper-right corner
        if( yLeft <= yy1 || yRight >= yy2 )
            return false;
        else
            return true;
    }
}

int main()
{
    double t_bin = 10e-6;
    KTProcessedTrackData tr;
    double sideband_separation = 20e6;
    double sideband_width = 1e6;
    double sideband_pwrFraction = 0.5;
    TRandom3 rand(0);

    tr.SetComponent( 0 );
    tr.SetStartTimeInRunC( 25e-5 );
    tr.SetEndTimeInRunC( 80e-5 );
    tr.SetStartFrequency( 100e6 );
    tr.SetEndFrequency( 118e6 );
    tr.SetSlope( 18e6/55e-5 );
    
    KTDiscriminatedPoints2DData threshPts;

    KTINFO(testlog, "Creating 2D thresholded points");
    int iBin = 0, jBin = 0;
    for( int i = 50e6; i < 150e6; i += 0.1e6 )
    {
        for( double j = 0; j <= 1e-3; j += t_bin )
        {
            if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, i - 0.05e6, j + 0.5 * t_bin, i + 0.05e6 ) )
            {
                threshPts.AddPoint( jBin, iBin, KTDiscriminatedPoints2DData::Point( j, i, 1, 1e-11 ), 0 );
            }
            else if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, i + sideband_separation - sideband_width / 2, j + 0.5 * t_bin, i + sideband_separation + sideband_width / 2 ) ||
                lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, i - sideband_separation - sideband_width / 2, j + 0.5 * t_bin, i - sideband_separation + sideband_width / 2 ) )
            {
                if( rand.Uniform( 1 ) < sideband_pwrFraction )
                {
                    threshPts.AddPoint( jBin, iBin, KTDiscriminatedPoints2DData::Point( j, i, 1, 1e-11 ), 0 );
                }
            }
            else if( rand.Uniform( 1 ) < 0.01 )
                threshPts.AddPoint( jBin, iBin, KTDiscriminatedPoints2DData::Point( j, i, 1, 1e-11 ), 0 );
            jBin++;
        }
        iBin++;
    }

    // Add them to vectors so that we can graph them
    vector< double > xx;
    vector< double > yy;
    int n = 0;
    
    for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = threshPts.GetSetOfPoints(0).begin(); it != threshPts.GetSetOfPoints(0).end(); ++it )
    {
        xx.push_back( it->second.fAbscissa );
        yy.push_back( it->second.fOrdinate );
        n++;
    }

    // Actually we need physical arrays lol
    double* xArray = &xx[0];
    double* yArray = &yy[0];

    KTINFO(testlog, "Writing to file");

    // Write to file
#ifdef ROOT_FOUND
    TFile* file = new TFile( "linear-density-fit-test.root", "recreate" );
    TGraph* plot;
    plot = new TGraph( n, xArray, yArray );
    file->Append( plot );
    plot->Write( "thresholded-points" );
    file->Close();
#endif

    KTLinearDensityProbeFit lineFitter;
    lineFitter.SetMinFrequency( 60e6 );
    lineFitter.SetMaxFrequency( 140e6 );
    lineFitter.SetProbeWidthBig( 1e6 );
    lineFitter.SetProbeWidthSmall( 0.02e6 );
    lineFitter.SetStepSize( 0.2e6 );
    lineFitter.SetStepSize( 0.004e6 );

    KTINFO(testlog, "Performing density fit");
    
    // Make a stupid PS collection data
    // All that matters are the time range and the frequency range of the spectrum
    
    KTPowerSpectrum* ps;

    KTPSCollectionData psColl;
    psColl.SetStartTime(0.);
    psColl.SetEndTime(0.001);
    psColl.SetMinFreq(55.e6);
    psColl.SetMaxFreq(145.e6);

    for( double j = 0; j <= 1e-3; j += t_bin )
    {
        ps = new KTPowerSpectrum(100, 50e6, 150e6);
        for( int iBin = 0; iBin < 100; ++iBin )
        {
            if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, 50e6 + 0.1e6*iBin - 0.05e6, j + 0.5 * t_bin, 50e6 + 0.1e6*iBin + 0.05e6 ) )
            {
                (*ps)(iBin) = 1e-11;
            }
            else if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, 50e6 + 0.1e6*iBin + sideband_separation - sideband_width / 2, j + 0.5 * t_bin, 50e6 + 0.1e6*iBin + sideband_separation + sideband_width / 2 ) ||
                lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j - 0.5 * t_bin, 50e6 + 0.1e6*iBin - sideband_separation - sideband_width / 2, j + 0.5 * t_bin, 50e6 + 0.1e6*iBin - sideband_separation + sideband_width / 2 ) )
            {
                if( rand.Uniform( 1 ) < sideband_pwrFraction )
                {
                    (*ps)(iBin) = 1e-11;
                }
            }
            else if( rand.Uniform( 1 ) < 0.01 )
                (*ps)(iBin) = 1e-11;
            else
                (*ps)(iBin) = 1e-12;
        }

        psColl.AddSpectrum( j, ps );
    }
    
    if( !lineFitter.DensityMaximization( tr, threshPts, psColl ) )
    {
        KTERROR(testlog, "Something went wrong in the fit");
    }

    KTLinearFitResult& result = tr.Of< KTLinearFitResult >();

    double fitSlope = result.GetSlope(1);
    double fitStartFrequency = result.GetIntercept(1) + tr.GetStartTimeInRunC() * result.GetSlope(1);
    double fitSidebandSeparation = abs(result.GetIntercept(1) - result.GetIntercept(0));

    double actualSlope = (tr.GetEndFrequency() - tr.GetStartFrequency())/(tr.GetEndTimeInRunC() - tr.GetStartTimeInRunC());

    KTINFO(testlog, "Actual slope: " << actualSlope << "\nFit slope: " << fitSlope);
    KTINFO(testlog, "Actual starting frequency: " << tr.GetStartFrequency() << "\nFit starting frequency: " << fitStartFrequency);
    KTINFO(testlog, "Actual sideband separation: " << sideband_separation << "\nFit sideband separation: " << fitSidebandSeparation);

    return 0;
}
