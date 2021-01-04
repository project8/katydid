/*
 * KTFSCDCRESUtils.cc
 *
 *  Created on: Dec 4, 2020
 *      Author: P. T. Surukuchi
 */

#include "KTFSCDCRESUtils.hh"

#include "KTSpline.hh"
#include "KTLogger.hh"
#include "KTMath.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(creslog, "KTFSCDCRESUtils");

    KTFSCDCRESUtils::KTFSCDCRESUtils(const string& name):
        fWavelength(0.0115),
        fPitchAngle(90),
        fGradBFrequency(0),
        fArrayRadius(0.0516),
        fApplyAntiSpiralPhaseShifts(true),
        fApplyGradBDopplerFreqShifts(true),
        fApplyGradBNormalFreqShifts(true),
        fApplyFreqShifts(true)
    {
    }

    KTFSCDCRESUtils::~KTFSCDCRESUtils()
    {
    }

    bool KTFSCDCRESUtils::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            fWavelength = node->get_value< double >("wavelength", fWavelength);
            fPitchAngle = node->get_value< double >("pitch-angle", fPitchAngle);
            fGradBFrequency= node->get_value< double >("gradb-frequency", fGradBFrequency);
            fArrayRadius = node->get_value< double >("active-radius", fArrayRadius);
            fApplyGradBDopplerFreqShifts= node->get_value< bool>("use-gradb-doppler-freq-shifts", fApplyGradBDopplerFreqShifts);
            fApplyGradBNormalFreqShifts= node->get_value< bool>("use-gradb-normal-freq-shifts", fApplyGradBNormalFreqShifts);
            fApplyAntiSpiralPhaseShifts = node->get_value< bool>("use-antispiral-phase-shifts", fApplyAntiSpiralPhaseShifts);
        }
        fApplyFreqShifts=(fApplyGradBDopplerFreqShifts || fApplyGradBNormalFreqShifts);
        return true;
    }

    double KTFSCDCRESUtils::ConvertWavelengthToFrequency(double wavelength) const
    {
        return C/wavelength;
    }

    bool KTFSCDCRESUtils::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
    {
        if (gridNumber >= gridSize) return false;
        gridLocation = fArrayRadius * (((2.0 * gridNumber + 1.0) / gridSize) - 1);
        return true;
    }


    double KTFSCDCRESUtils::GetPhaseShift(double xPosition, double yPosition, double channelAngle) const
    {
        // X position based on the angle of the channel
        double xChannel = fArrayRadius * cos(channelAngle);
        // X position based on the angle of the channel
        double yChannel = fArrayRadius * sin(channelAngle);
        // Distance of the input point from the input channel
        double pointDistance = pow(pow(xChannel - xPosition, 2) + pow(yChannel - yPosition, 2), 0.5);
        // Phase of the input signal based on the input point, channel location and the wavelength
        double phaseShift=KTMath::TwoPi() * pointDistance / fWavelength;
        if(fApplyAntiSpiralPhaseShifts)
        {
            phaseShift-=GetAntiSpiralPhaseShift( xPosition, yPosition, channelAngle);
        }
        return phaseShift;
    }

    bool KTFSCDCRESUtils::ApplyPhaseShift(double &realVal, double &imagVal, double phase)
    {
        double tempRealVal = realVal;
        double tempImagVal = imagVal;
        realVal = tempRealVal * cos(phase) - tempImagVal * sin(phase);
        imagVal = tempRealVal * sin(phase) + tempImagVal * cos(phase);
        return true;
    }

    double KTFSCDCRESUtils::GetAntiSpiralPhaseShift(double xPosition, double yPosition, double channelAngle) const
    {
        // X position based on the angle of the channel
        double xChannel = fArrayRadius* cos(channelAngle);
        // X position based on the angle of the channel
        double yChannel = fArrayRadius* sin(channelAngle);
        // Angle based on the deltaX and deltaY
        return atan2(yChannel-yPosition,xChannel-xPosition);
    }

    // β= |β|cos(theta)
    //where theta=arccos(((x1-x)*(-y)+(y1-y)*x)/(sqrt(x^2+y^2)*sqrt((x1-x)^2+(y1-y)^2))) = arccos(y1*x-x1*y)/(sqrt(x^2+y^2)*sqrt((x1-x)^2+(y1-y)^2))
    //In other words theta=arccos((channelY*xPosition-channelX*yPosition)/(sqrt(xPosition^2+yPosition^2)*sqrt((channelX-xPosition)^2+(channelY-yPosition)^2)))
    double KTFSCDCRESUtils::GetGradBBeta(double xPosition, double yPosition, double channelX, double channelY) const
    {
        double numerator=channelY*xPosition-channelX*yPosition;
        double denominator=sqrt(pow(xPosition,2)+pow(yPosition,2))*sqrt(pow(channelX-xPosition,2)+pow(channelY-yPosition,2));
        double electronRadius=sqrt(pow(xPosition,2)+pow(yPosition,2));
        double gradBBeta=fGradBFrequency*electronRadius/C;
        return gradBBeta*numerator/denominator; 
    }

    double KTFSCDCRESUtils::GetGradBDopplerFreqShift(double xPosition, double yPosition, double channelX, double channelY) const
    {
        double beta=GetGradBBeta(xPosition,yPosition,channelX,channelY);
        double omega=KTMath::TwoPi()*ConvertWavelengthToFrequency(fWavelength);
        double freqShift=beta*omega/KTMath::TwoPi(); 
        return freqShift; 
    }

    double KTFSCDCRESUtils::GetGradBNormalFreqShift(double xPosition, double yPosition, double channelX, double channelY) const
    {
        double beta=GetGradBBeta(xPosition,yPosition,channelX,channelY);
        double channelElectronDistance=sqrt(pow(yPosition-channelY,2)+pow(xPosition-channelX,2));
        //PTS:: This is wrong, need to correct it
        double freqShift=beta*C/channelElectronDistance;
        return freqShift; 
    }

    // PTS:There's got to be a better way to do this without using a spline  
    // This function will eventually need to be moved to a more general location
    bool KTFSCDCRESUtils::ApplyFrequencyShift(const KTFrequencySpectrumFFTW &freqSpectrum,KTFrequencySpectrumFFTW &shiftedFreqSpectrum ,double freqShift, double minFreq, double maxFreq) const
    {
        shiftedFreqSpectrum=freqSpectrum;// Copy the input spectrum into the new one before modifying it
        int nFreqBins = freqSpectrum.GetNFrequencyBins();
        double* axisValues = new double[nFreqBins];
        double* realVals = new double[nFreqBins];
        double* imagVals = new double[nFreqBins];
        // Get the bin centers and the real and imaginary frequency spectrum values
        for (unsigned int i= 0; i< nFreqBins; ++i)
        {
            axisValues[i]=freqSpectrum.GetBinCenter(i);
            realVals[i]=freqSpectrum.GetReal(i);
            imagVals[i]=freqSpectrum.GetImag(i);
        }
        // Create splines for the real and imaginary values based on the corresponding frequency spectrum values
        KTSpline realSpline = KTSpline(axisValues,realVals,nFreqBins);
        KTSpline imagSpline = KTSpline(axisValues,imagVals,nFreqBins);
        // Loop through the range of 
        for (unsigned int i= 0; i< nFreqBins; ++i)//PTS: Needs more efficient iteration method perhaps
        {
            // Skip if out of range of the freqSpectrum
            if((freqSpectrum.GetBinCenter(i)+freqShift) <realSpline.GetXMin() ||  (freqSpectrum.GetBinCenter(i)+freqShift)>realSpline.GetXMax()) continue;
            // Skip if out of range of the minimum and maximum frequency spectrum range 
            if(freqSpectrum.GetBinCenter(i)<minFreq || freqSpectrum.GetBinCenter(i)>maxFreq) continue;
            // Evaluate the new real and imag values based on the spline and save them in shiftedFreqSpectrum
            double newRealVal=realSpline.Evaluate(freqSpectrum.GetBinCenter(i)+freqShift);
            double newImagVal=imagSpline.Evaluate(freqSpectrum.GetBinCenter(i)+freqShift);
            shiftedFreqSpectrum.SetRect(i,newRealVal,newImagVal);
        }
        delete [] axisValues;
        delete [] realVals;
        delete [] imagVals;
        return true;
    }

    bool KTFSCDCRESUtils::ApplyFrequencyShift(KTFrequencySpectrumFFTW &freqSpectrum,double freqShift, double minFreq, double maxFreq) const
    {
        KTFrequencySpectrumFFTW newFreqSpectrum = freqSpectrum;
        bool retVal=ApplyFrequencyShift(freqSpectrum,newFreqSpectrum,freqShift,minFreq,maxFreq);
        freqSpectrum=newFreqSpectrum;
        return retVal; 
    }

    bool KTFSCDCRESUtils::ApplyFrequencyShifts(const KTFrequencySpectrumFFTW &freqSpectrum,KTFrequencySpectrumFFTW &shiftedFreqSpectrum,double xPosition, double yPosition, double channelAngle, double minFreq, double maxFreq) const
    {
        if(!fApplyFreqShifts) return fApplyFreqShifts;
        bool anyFreqShiftsApplied=false;
        double xChannel = fArrayRadius* cos(channelAngle);
        double yChannel = fArrayRadius * sin(channelAngle);
        double gradBDopShift=0;
        double gradBNormShift=0;
        if(fApplyGradBDopplerFreqShifts) 
        {
            gradBDopShift=GetGradBDopplerFreqShift(xPosition, yPosition, xChannel, yChannel);
            anyFreqShiftsApplied=true;
        }
        if(fApplyGradBNormalFreqShifts) 
        {
            gradBNormShift=GetGradBNormalFreqShift(xPosition, yPosition, xChannel, yChannel);
            anyFreqShiftsApplied=true;
        }
        return ApplyFrequencyShift(freqSpectrum,shiftedFreqSpectrum,-gradBDopShift-gradBNormShift,minFreq,maxFreq);
    }

    bool KTFSCDCRESUtils::ApplyFrequencyShifts(KTFrequencySpectrumFFTW &freqSpectrum,double xPosition, double yPosition, double channelAngle, double minFreq, double maxFreq) const
    {
        KTFrequencySpectrumFFTW newFreqSpectrum = freqSpectrum;
        bool retVal = ApplyFrequencyShifts(freqSpectrum,newFreqSpectrum,xPosition,yPosition,channelAngle,minFreq,maxFreq);
        freqSpectrum=newFreqSpectrum;
        return retVal;
    }


} /* namespace Katydid */
