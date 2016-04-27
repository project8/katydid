/*
 * TestLinearDensityProbe.cc
 *
 *  Created on: Apr 12, 2016
 *      Author: ezayas 
 */

#include "KTLinearDensityProbeFit.hh"
#include "KTDiscriminatedPoints2DData.hh"
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

bool lineIntersects( double x1, double y1, double x2, double y2, double xx1, double yy1, double xx2, double yy2 )
{
	if( x2 <=  xx1 || x1 >= xx2 )
		return false;

	double m = (y2 - y1) / (x2 - x1);
	double yLeft = y1 + (y2 - y1) * (xx1 - x1) / (x2 - x1);
	double yRight = y1 + (y2 - y1) * (xx2 - x1) / (x2 - x1);

	if( m > 0 )
	{
		if( yLeft >= yy2 || yRight <= yy1 )
			return false;
		else
			return true;
	}
	else
	{
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
	TRandom3 r;

	tr.SetComponent( 1 );
	tr.SetStartTimeInRunC( 15e-5 );
	tr.SetEndTimeInRunC( 85e-5 );
	tr.SetStartFrequency( 80e6 );
	tr.SetEndFrequency( 90e6 );
	
	KTDiscriminatedPoints2DData threshPts;

	KTINFO(testlog, "Creating 2D thresholded points");
	int iBin = 0, jBin = 0;
	for( int i = 50e6; i < 150e6; i += 0.1e6 )
	{
		for( int j = 0; j <= 1e-3; j += t_bin )
		{
			if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j, i, j + t_bin, i + 0.1e6 ) )
			{
				threshPts.AddPoint( jBin, iBin, KTDiscriminatedPoints2DData::Point( j, i, 1, 1e-11 ), 0 );
			}
			if( lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j, i + sideband_separation - sideband_width / 2, j + t_bin, i + sideband_separation - sideband_width / 2 ) ||
				lineIntersects( tr.GetStartTimeInRunC(), tr.GetStartFrequency(), tr.GetEndTimeInRunC(), tr.GetEndFrequency(), j, i - sideband_separation - sideband_width / 2, j + t_bin, i - sideband_separation - sideband_width / 2 ) )
			{
				if( r.Uniform( 1 ) < sideband_pwrFraction )
				{
					threshPts.AddPoint( jBin, iBin, KTDiscriminatedPoints2DData::Point( j, i, 1, 1e-11 ), 0 );
				}
			}
			jBin++;
		}
		iBin++;
	}

	KTLinearDensityProbeFit lineFitter;
	lineFitter.SetMinFrequency( 60e6 );
	lineFitter.SetMaxFrequency( 140e6 );
	lineFitter.SetProbeWidthBig( 1e6 );
	lineFitter.SetProbeWidthSmall( 0.02e6 );
	lineFitter.SetStepSizeBig( 0.2e6 );
	lineFitter.SetStepSizeSmall( 0.004e6 );

	KTINFO(testlog, "Performing density fit");
	
	if( !lineFitter.Calculate( tr, threshPts ) )
		KTERROR(testlog, "Something went wrong in the fit");

	KTLinearFitResult& result = tr.Of< KTLinearFitResult >();

	double fitSlope = result.GetSlope();
	double fitStartFrequency = result.GetIntercept() + tr.GetStartTimeInRunC() * result.GetSlope();

	double q = (tr.GetEndFrequency() - tr.GetStartFrequency())/(tr.GetEndTimeInRunC() - tr.GetStartTimeInRunC());

	cout << "Actual slope: " << q << "\nFit slope: " << fitSlope << "\nActual starting frequency: " << tr.GetStartFrequency() << "\nFit starting frequency: " << fitStartFrequency << "\nSideband separation: " << sideband_separation << endl;

	return 0;
}