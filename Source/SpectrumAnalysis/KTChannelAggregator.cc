/*
 * KTChannelAggregator.cc
 *
 *  Created on: Jan 25, 2019
 *      Author: P. T. Surukuchi
 */

#include "KTChannelAggregator.hh"
#include "KTLogger.hh"

#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

namespace Katydid
{
    KTLOGGER(agglog, "KTChannelAggregator");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTChannelAggregator, "channel-aggregator");

    KTChannelAggregator::KTChannelAggregator(const std::string& name) :
        KTProcessor(name),
        fSummedFrequencyData("agg-fft", this),
        fPhaseChFrequencySumSlot("fft", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedFrequencyData),
        fAxialSumSlot("ax-agg-fft", this, &KTChannelAggregator::SumChannelVoltageWithPhase, &fSummedFrequencyData),
        fActiveRadius(0.0516),
        fNGrid(30),
        fWavelength(0.0115),
        fIsGridDefined(false),
        fIsUserDefinedGrid(false),
        fIsPartialRing(false),
        fPartialRingMultiplicity(),
        fSummationMinFreq(0e6),
        fSummationMaxFreq(200e6),
        fUseAntiSpiralPhaseShifts(true),
        fApplyGradBDopplerPhaseShifts(true),
        fApplyGradBNormalPhaseShifts(true),
        fAntiSpiralPhaseShifts(),
        fNRings(1)
    {
    }

    KTChannelAggregator::~KTChannelAggregator()
    {
    }

    bool KTChannelAggregator::Configure(const scarab::param_node* node)
    {
        if (node != NULL)
        {
            fNGrid = node->get_value< signed int >("grid-size", fNGrid);
            fIsUserDefinedGrid = node->get_value< bool >("use-grid-text-file", fIsUserDefinedGrid);
            fIsPartialRing= node->get_value< bool >("partial-ring", fIsPartialRing);
            fPartialRingMultiplicity= node->get_value< int >("partial-ring-Multiplicity", fPartialRingMultiplicity);
            fUserDefinedGridFile = node->get_value< >("grid-text-file", fUserDefinedGridFile);
            fActiveRadius = node->get_value< double >("active-radius", fActiveRadius);
            fWavelength = node->get_value< double >("wavelength", fWavelength);
            fSummationMinFreq= node->get_value< double >("min-freq", fSummationMinFreq);
            fSummationMaxFreq= node->get_value< double >("max-freq", fSummationMaxFreq);
            fNRings = node->get_value< signed int >("n-rings", fNRings);
            fUseAntiSpiralPhaseShifts = node->get_value< bool>("use-antispiral-phase-shifts", fUseAntiSpiralPhaseShifts);
        }
        return true;
    }

    bool KTChannelAggregator::ApplyPhaseShift(double &realVal, double &imagVal, double phase)
    {
        double tempRealVal = realVal;
        double tempImagVal = imagVal;
        realVal = tempRealVal * cos(phase) - tempImagVal * sin(phase);
        imagVal = tempRealVal * sin(phase) + tempImagVal * cos(phase);
        return true;
    }

    double KTChannelAggregator::GetPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle) const
    {
        // X position based on the angle of the channel
        double xChannel = fActiveRadius * cos(channelAngle);
        // X position based on the angle of the channel
        double yChannel = fActiveRadius * sin(channelAngle);
        // Distance of the input point from the input channel
        double pointDistance = pow(pow(xChannel - xPosition, 2) + pow(yChannel - yPosition, 2), 0.5);
        // Phase of the input signal based on the input point, channel location and the wavelength
        double phaseShift=2.0 * KTMath::Pi() * pointDistance / wavelength;
        if(fApplyGradBDopplerPhaseShifts) phaseShift-=0;
        if(fApplyGradBNormalPhaseShifts) phaseShift-=0;
        return phaseShift;
    }

    double KTChannelAggregator::GetAntiSpiralPhaseShift(double xPosition, double yPosition, double wavelength, double channelAngle) const
    {
        // X position based on the angle of the channel
        double xChannel = fActiveRadius * cos(channelAngle);
        // X position based on the angle of the channel
        double yChannel = fActiveRadius * sin(channelAngle);
        // Angle based on the deltaX and deltaY
        return atan2(yChannel-yPosition,xChannel-xPosition);
    }

    bool KTChannelAggregator::GetGridLocation(int gridNumber, int gridSize, double &gridLocation)
    {
        if (gridNumber >= gridSize) return false;
        gridLocation = fActiveRadius * (((2.0 * gridNumber + 1.0) / gridSize) - 1);
        return true;
    }

    bool KTChannelAggregator::GenerateAntiSpiralPhaseShifts(int channelCount)
    {
        for(int i=0;i<channelCount;++i)
        {
            double phaseShift=0.0;
            if(fUseAntiSpiralPhaseShifts) 
            {
                phaseShift=i*2*KTMath::Pi()/channelCount;
            }
            std::pair<int,double> channelPhaseShift=std::make_pair(i,phaseShift);
            fAntiSpiralPhaseShifts.insert(channelPhaseShift);
        }
        return true;
    }

    bool KTChannelAggregator::SumChannelVoltageWithPhase(KTFrequencySpectrumDataFFTW& fftwData)
    {
        KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData = fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(0);
        return PerformPhaseSummation(fftwData,newAggFreqData);
    }

    bool KTChannelAggregator::SumChannelVoltageWithPhase(KTAxialAggregatedFrequencySpectrumDataFFTW& fftwData)
    {
        KTAggregatedFrequencySpectrumDataFFTW& newAggFreqData = fftwData.Of< KTAggregatedFrequencySpectrumDataFFTW >().SetNComponents(0);
        return PerformPhaseSummation(fftwData,newAggFreqData);
    }

    int KTChannelAggregator::DefineGrid(KTAggregatedFrequencySpectrumDataFFTW &newAggFreqData)
    {
        int nTotalGridPoints=0;
        for (int iRing = 0; iRing < fNRings; ++iRing)
        {
            if(fIsUserDefinedGrid)
            {
                if(!ends_with(fUserDefinedGridFile,".txt"))
                {
                    //if(!ends_with(fTextFileName.c_str(),".txt"))
                    KTDEBUG(agglog,"`grid-text-file` file must end in .txt");
                    return -1;
                }
                double gridLocationX;
                double gridLocationY;
                std::fstream textFile(fUserDefinedGridFile.c_str(),std::ios::in);
                if (textFile.fail())
                {
                    KTDEBUG(agglog,"`grid-text-file` cannot be opened");
                    return -1;
                }
                while(!textFile.eof())
                {
                    std::string lineContent;
                    while(std::getline(textFile,lineContent))
                    {
                        if (lineContent.find('#')!=std::string::npos) continue;
                        std::string token;
                        std::stringstream ss(lineContent);
                        int wordCount=0;
                        while (ss >> token)
                        {
                            if(wordCount==0)gridLocationX=std::stod(token);
                            else if(wordCount==1)gridLocationY=std::stod(token);
                            else
                            {
                                KTDEBUG(agglog,"`grid-text-file` cannot have more than 2 columns"); 
                                return -1;
                            }
                            ++wordCount;
                        }
                        // Check to make sure that the grid point is within the active detector volume, skip otherwise
                        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(fActiveRadius,2)) continue;
                        newAggFreqData.SetNComponents(nTotalGridPoints+1);
                        newAggFreqData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY,iRing);
                        ++nTotalGridPoints;
                    }
                }
            }
            else
            {
                // Loop over the grid points and fill the values
                for (int iGridX = 0; iGridX < fNGrid; ++iGridX)
                {
                    double gridLocationX = 0;
                    GetGridLocation(iGridX, fNGrid, gridLocationX);
                    for (int iGridY = 0; iGridY < fNGrid; ++iGridY)
                    {
                        double gridLocationY = 0;
                        GetGridLocation(iGridY, fNGrid, gridLocationY);
                        // Check to make sure that the grid point is within the active detector volume, skip otherwise
                        if((pow(gridLocationX,2)+pow(gridLocationY,2))>pow(fActiveRadius,2)) continue;
                        newAggFreqData.SetNComponents(nTotalGridPoints+1);
                        newAggFreqData.SetGridPoint(nTotalGridPoints, gridLocationX, gridLocationY,iRing);
                        ++nTotalGridPoints;
                    }
                }
            }
        }
        return nTotalGridPoints;
    }

    bool KTChannelAggregator::PerformPhaseSummation(KTFrequencySpectrumDataFFTWCore& fftwData,KTAggregatedFrequencySpectrumDataFFTW &newAggFreqData)
    {
        const KTFrequencySpectrumFFTW* freqSpectrum = fftwData.GetSpectrumFFTW(0);
        int nTimeBins = freqSpectrum->GetNTimeBins();
        // Get the number of frequency bins from the first component of fftwData
        int nFreqBins = freqSpectrum->GetNFrequencyBins();
        int nTotalComponents = fftwData.GetNComponents(); // Get number of components
        if(fIsPartialRing)
        {
            if(fPartialRingMultiplicity%2!=0 || fPartialRingMultiplicity<=0) return false; // The partial ring case should only work if the multiplicity is even
            nTotalComponents*=fPartialRingMultiplicity;
        }
        if(nTotalComponents%fNRings!=0)
        {
            KTERROR(agglog,"The number of rings has to be an integer multiple of total components");
        }
        int nComponents = nTotalComponents/fNRings;// Get number of components

        GenerateAntiSpiralPhaseShifts(nComponents);
        double maxValue = 0.0;
        double maxGridLocationX = 0.0;
        double maxGridLocationY = 0.0;

        // Setting up the active radius of the KTAggregatedFrequencySpectrumDataFFTW object to maintain consistency
        // This doesn't need to be done if there is a way to provide config values to data objects
        newAggFreqData.SetActiveRadius(fActiveRadius);
        // Set the number of rings present
        newAggFreqData.SetNAxialPositions(fNRings);

        int nTotalGridPoints = DefineGrid(newAggFreqData);
        if(nTotalGridPoints<=0) return false;
        int  gridPointsPerRing=nTotalGridPoints/fNRings;
        // Loop over the grid points and rings and fill the values
        for (unsigned iRing = 0; iRing < fNRings; ++iRing)
        {
            // Loop over all grid points and find the one that gives the highest value
            for (int iGrid = 0; iGrid < gridPointsPerRing; ++iGrid)
            { // Loop over the grid points
                int gridPointNumber=iGrid+gridPointsPerRing*iRing;
                KTFrequencySpectrumFFTW* newFreqSpectrum = new KTFrequencySpectrumFFTW(nFreqBins, freqSpectrum->GetRangeMin(), freqSpectrum->GetRangeMax());
                // Empty values in the frequency spectrum, not sure if this is needed but there were some issues when this was not done for the power spectrum
                NullFreqSpectrum(*newFreqSpectrum);
                double gridLocationX = 0;
                double gridLocationY = 0;
                double gridLocationZ = 0;
                newAggFreqData.GetGridPoint(gridPointNumber, gridLocationX, gridLocationY,gridLocationZ);
                for (unsigned iComponent = 0; iComponent < nComponents; ++iComponent)
                {
                    // Arbitarily assign 0 to the first channel and progresively add 2pi/N for the rest of the channels in increasing order
                    double channelAngle = 2 * KTMath::Pi() * iComponent / nComponents;
                    double phaseShift = GetPhaseShift(gridLocationX, gridLocationY, fWavelength, channelAngle);
                    // Just being redundantly cautious, the phaseShifts are already zerors but checking to make sure anyway
                    if(fUseAntiSpiralPhaseShifts)
                    {
                        phaseShift-=GetAntiSpiralPhaseShift(gridLocationX, gridLocationY, fWavelength, channelAngle);
                    }
                    // Get the frequency spectrum for that specific component
                    if(fIsPartialRing) 
                    { 
                        int partialNComponents=nComponents/fPartialRingMultiplicity;
                        // Estimate the channel that needs to be used as a copy for the non-existent iComponent in case of partial rings
                        int partialComponent=((int)(iComponent/partialNComponents)%2)?(partialNComponents-(iComponent%partialNComponents)-1):(iComponent%partialNComponents);
                        freqSpectrum = fftwData.GetSpectrumFFTW(partialComponent+iRing*partialNComponents);
                    }
                    else freqSpectrum = fftwData.GetSpectrumFFTW(iComponent+iRing*nComponents);
                    double maxVoltage = 0.0;
                    int maxFrequencyBin = 0;
                    //Loop over the frequency bins
                    for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
                    {
                        if(newFreqSpectrum->GetBinCenter(iFreqBin)<fSummationMinFreq || newFreqSpectrum->GetBinCenter(iFreqBin)>fSummationMaxFreq) continue;
                        double realVal = freqSpectrum->GetReal(iFreqBin);
                        double imagVal = freqSpectrum->GetImag(iFreqBin);
                        ApplyPhaseShift(realVal, imagVal, phaseShift);
                        double summedRealVal = realVal + newFreqSpectrum->GetReal(iFreqBin);
                        double summedImagVal = imagVal + newFreqSpectrum->GetImag(iFreqBin);
                        (*newFreqSpectrum)(iFreqBin)[0] = summedRealVal;
                        (*newFreqSpectrum)(iFreqBin)[1] = summedImagVal;
                    } // End of loop over freq bins
                } // End of loop over all comps
                newFreqSpectrum->SetNTimeBins(nTimeBins);
                newAggFreqData.SetSpectrum(newFreqSpectrum,gridPointNumber);

                double maxVoltageFreq = 0.0;
                //Loop over all the freq bins and get the highest value and save to the aggregated frequency data
                for (unsigned iFreqBin = 0; iFreqBin < nFreqBins; ++iFreqBin)
                {
                    if (newFreqSpectrum->GetAbs(iFreqBin) > maxVoltageFreq)
                    {
                        maxVoltageFreq = newFreqSpectrum->GetAbs(iFreqBin);
                    }
                } // end of freqeuncy bin loops
                newAggFreqData.SetSummedGridVoltage(gridPointNumber, maxVoltageFreq);
            } // End of grid
        }// End of loop over all rings
        KTDEBUG(agglog,"Channel summation performed over "<< fNRings<<" rings and "<<gridPointsPerRing<<" grid points per ring in the range of frequencies ("<< fSummationMinFreq<< "," <<fSummationMaxFreq<<")");
        return true;
    }
}

