/*
 * KTConvolution2D.cc
 *
 *  Created on: Oct 20, 2017
 *      Author: ezayas
 */

#include "KTConvolution2D.hh"

#include "KTConvolvedSpectrumData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTPowerSpectrum.hh"

#include "param_codec.hh"
#include "param_json.hh"
#include "param.hh"
#include "KTConfigurator.hh"

using std::string;
using std::vector;

namespace Katydid
{
    KTLOGGER(sdlog, "KTConvolution2D");

    KT_REGISTER_PROCESSOR(KTConvolution2D, "convolution2D");

    KTConvolution2D::KTConvolution2D(const std::string& name) :
            KTProcessor(name),
            fKernel("placeholder.json"),
            fBlockSizeTime(0),
            fBlockSizeFreq(0),
            fNormalizeKernel(false),
            fTransformType("convolution"),
            fTransformFlag("ESTIMATE"),
            fRegularSizeTime(0),
            fRegularSizeFreq(0),
            fShortSizeTime(0),
            fShortSizeFreq(0),
            fTransformFlagMap(),
            fComplexToRealPlan(),
            fRealToComplexPlan(),
            fC2CForwardPlan(),
            fC2CReversePlan(),
            fComplexToRealPlanShort(),
            fRealToComplexPlanShort(),
            fC2CForwardPlanShort(),
            fC2CReversePlanShort(),
            fInputArrayReal(NULL),
            fOutputArrayReal(NULL),
            fInputArrayComplex(NULL),
            fOutputArrayComplex(NULL),
            fTransformedInputArray(NULL),
            fTransformedOutputArray(NULL),
            fTransformedInputArrayFromReal(NULL),
            fTransformedOutputArrayFromReal(NULL),
            fTransformedKernelXAsReal(NULL),
            fTransformedKernelXAsComplex(NULL),
            fGeneralTransformedInputArray(NULL),
            fGeneralTransformedOutputArray(NULL),
            fGeneralTransformedKernelArray(NULL),
            nBinLimitRegularTime(0),
            nBinLimitRegularFreq(0),
            nBinLimitShortTime(0),
            nBinLimitShortFreq(0),
            fGeneralForwardPlan(),
            fGeneralReversePlan(),
            fGeneralForwardPlanShort(),
            fGeneralReversePlanShort(),
            fTransformFlagUnsigned(FFTW_ESTIMATE),
            fKernelSizeX(0),
            fKernelSizeY(0),
            fInitialized(false),
            fMultiPSSignal("conv-multi-ps", this),
            fMultiFSFFTWSignal("conv-multi-fs-fftw", this),
            fMultiFSPolarSignal("conv-multi-fs-polar", this),
            fMultiPSSlot("multi-ps", this, &KTConvolution2D::Convolve2D, &fMultiPSSignal),
            fMultiFSFFTWSlot("multi-fs-fftw", this, &KTConvolution2D::Convolve2D, &fMultiFSFFTWSignal),
            fMultiFSPolarSlot("multi-fs-polar", this, &KTConvolution2D::Convolve2D, &fMultiFSPolarSignal)
    {
        SetupInternalMaps();
    }

    KTConvolution2D::~KTConvolution2D()
    {
        FreeArrays();
    }

    bool KTConvolution2D::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetKernel(node->get_value< std::string >("kernel", GetKernel()));
        SetBlockSizeTime(node->get_value< unsigned >("block-size-time", GetBlockSizeTime()));
        SetBlockSizeFreq(node->get_value< unsigned >("block-size-freq", GetBlockSizeFreq()));
        SetTransformType(node->get_value< std::string >("transform-type", GetTransformType()));
        SetTransformFlag(node->get_value< std::string >("transform-flag", GetTransformFlag()));

        return FinishSetup();
    }

    bool KTConvolution2D::FinishSetup()
    {
        // Find transform type
        if( GetTransformType() == "convolution" )
        {
            KTINFO(sdlog, "Transform type is ordinary convolution. Proceeding with kernel");
        }
        else if( GetTransformType() == "cross-correlation" )
        {
            KTINFO(sdlog, "Transform type is cross-correlation. Will conjugate and reverse input data first");
        }
        else
        {
            KTWARN(sdlog, "Transform type not recognized. Please make sure you've typed it correctly. Aborting");
            return false;
        }

        // Convert transform flag string to unsigned
        TransformFlagMap::const_iterator iter = fTransformFlagMap.find( fTransformFlag );
        fTransformFlagUnsigned = iter->second;

        // Read in the kernel
        if( ! ParseKernel() )
        {
            KTERROR(sdlog, "Failed to parse kernel json. Aborting");
            return false;
        }

        return true;
    }

    void KTConvolution2D::AllocateArrays( int nSizeRegularX, int nSizeShortX, int nSizeRegularY, int nSizeShortY )
    {
        KTDEBUG(sdlog, "DFT initialization started");
        KTDEBUG(sdlog, "Regular size X = " << nSizeRegularX);
        KTDEBUG(sdlog, "Regular size Y = " << nSizeRegularY);
        KTDEBUG(sdlog, "Short size X = " << nSizeShortX);
        KTDEBUG(sdlog, "Short size Y = " << nSizeShortY);

        if( nSizeShortX > nSizeRegularX || nSizeShortY > nSizeRegularY )
        {
            KTWARN(sdlog, "Short size is larger than regular size; something weird happened. Aborting DFT initialization");
            return;
        }

        if( fInitialized )
        {
            KTDEBUG(sdlog, "Already initialized! Freeing arrays first");
            FreeArrays();
        }

        // Input/Output arrays
        fInputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegularX * nSizeRegularY );
        fOutputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegularX * nSizeRegularY );
        fInputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fOutputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );

        // Intermediate (fourier space) arrays
        fTransformedInputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fTransformedOutputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * nSizeRegularY );
        fTransformedInputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * (nSizeRegularY/2 + 1) );
        fTransformedOutputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegularX * (nSizeRegularY/2 + 1) );

        // DFT plans
        fRealToComplexPlan = fftw_plan_dft_r2c_2d( nSizeRegularX, nSizeRegularY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlan = fftw_plan_dft_c2r_2d( nSizeRegularX, nSizeRegularY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlan = fftw_plan_dft_2d( nSizeRegularX, nSizeRegularY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlan = fftw_plan_dft_2d( nSizeRegularX, nSizeRegularY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        fRealToComplexPlanShort = fftw_plan_dft_r2c_2d( nSizeShortX, nSizeShortY, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlanShort = fftw_plan_dft_c2r_2d( nSizeShortX, nSizeShortY, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlanShort = fftw_plan_dft_2d( nSizeShortX, nSizeShortY, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlanShort = fftw_plan_dft_2d( nSizeShortX, nSizeShortY, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        // Store these guys
        fRegularSizeTime = nSizeRegularX;
        fRegularSizeFreq = nSizeRegularY;
        fShortSizeTime = nSizeShortX;
        fShortSizeFreq = nSizeShortY;

        // Make sure we don't come back here unless something weird happens
        fInitialized = true;

        return;
    }  

    void KTConvolution2D::FreeArrays()
    {
        if( fInputArrayReal != nullptr )
        {
            fftw_free( fInputArrayReal );
            fInputArrayReal = nullptr;
        }
        if( fOutputArrayReal != nullptr )
        {
            fftw_free( fOutputArrayReal );
            fOutputArrayReal = nullptr;
        }
        if( fInputArrayComplex != nullptr )
        {
            fftw_free( fInputArrayComplex );
            fInputArrayComplex = nullptr;
        }
        if( fOutputArrayComplex != nullptr )
        {
            fftw_free( fOutputArrayComplex );
            fOutputArrayComplex = nullptr;
        }

        if( fTransformedInputArray != nullptr )
        {
            fftw_free( fTransformedInputArray );
            fTransformedInputArray = nullptr;
        }

        if( fTransformedOutputArray != nullptr )
        {
            fftw_free( fTransformedOutputArray );
            fTransformedOutputArray = nullptr;
        }

        if( fRealToComplexPlan != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlan );
            fRealToComplexPlan = nullptr;
        }
        if( fComplexToRealPlan != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlan );
            fComplexToRealPlan = nullptr;
        }
        if( fC2CForwardPlan != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlan );
            fC2CForwardPlan = nullptr;
        }
        if( fC2CReversePlan != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlan );
            fC2CReversePlan = nullptr;
        }

        if( fRealToComplexPlanShort != nullptr )
        {
            fftw_destroy_plan( fRealToComplexPlanShort );
            fRealToComplexPlanShort = nullptr;
        }
        if( fComplexToRealPlanShort != nullptr )
        {
            fftw_destroy_plan( fComplexToRealPlanShort );
            fComplexToRealPlanShort = nullptr;
        }
        if( fC2CForwardPlanShort != nullptr )
        {
            fftw_destroy_plan( fC2CForwardPlanShort );
            fC2CForwardPlanShort = nullptr;
        }
        if( fC2CReversePlanShort != nullptr )
        {
            fftw_destroy_plan( fC2CReversePlanShort );
            fC2CReversePlanShort = nullptr;
        }

        fInitialized = false;

        return;
    }

    bool KTConvolution2D::ParseKernel()
    {
        KTINFO(sdlog, "Attempting to parse kernel");
        // Read in json with scarab::param

        scarab::path kernelFilePath = scarab::expand_path( GetKernel() );
        scarab::param_translator translator;
        scarab::param* kernelFromFile = translator.read_file( kernelFilePath.native() );
        scarab::param_node& kernelNode = kernelFromFile->as_node();

        if( ! kernelNode.has( "kernel" ) )
        {
            KTERROR( sdlog, "Kernel configuration file is not properly written. Aboring" );
            return false;
        }

        // Get kernel as array
        scarab::param_array& kernel2DArray = kernelNode["kernel"].as_array();
        KTDEBUG(sdlog, "Obtained kernel as array");

        // We need to store this because we're about to periodically extend it
        fKernelSizeY = kernel2DArray.size();
        fKernelSizeX = sizeof( kernel2DArray.get_value< double* >(0) ) / sizeof( double );
        KTDEBUG(sdlog, "Kernel size X = " << fKernelSizeX);
        KTDEBUG(sdlog, "Kernel size Y = " << fKernelSizeY);

        // Fill kernel vector
        // Also calculate the norm in case we need that

        double norm = 0.;
        std::vector< double > kernelRow;
        for( int iValue = 0; iValue < fKernelSizeY; ++iValue )
        {
            kernelRow.clear();
            for( int jValue = 0; jValue < fKernelSizeX; ++jValue )
            {
                kernelRow.push_back( (kernel2DArray.get_value< double* >(iValue))[jValue] );
                norm += kernelRow[jValue];
            }
            
            kernelXY.push_back( kernelRow );
        }
        KTDEBUG(sdlog, "Kernel norm = " << norm);

        // Here is where we set block size if left unspecified
        // We need it to periodically extend the kernel
        if( GetBlockSizeTime() == 0 )
        {
            SetBlockSizeTime( 8 * fKernelSizeX );
            int power = log2( GetBlockSizeTime() ); // int will take the floor of the log
            SetBlockSizeTime( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set time block size: " << GetBlockSizeTime());

        // Check that kernel size is not more than block size
        if( GetBlockSizeTime() < fKernelSizeX )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        if( GetBlockSizeFreq() == 0 )
        {
            SetBlockSizeFreq( 8 * fKernelSizeY );
            int power = log2( GetBlockSizeFreq() ); // int will take the floor of the log
            SetBlockSizeFreq( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set freq block size: " << GetBlockSizeFreq());

        // Check that kernel size is not more than block size
        if( GetBlockSizeFreq() < fKernelSizeY )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        // Periodically extend kernel up to block size
        for( int iPosition = 0; iPosition < GetBlockSizeTime(); ++iPosition )
        {
            if( iPosition < fKernelSizeX )
            {
                for( int jPosition = fKernelSizeY; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelXY[iPosition].push_back( 0.0 );
                }
            }
            else
            {
                kernelRow.clear();
                for( int jPosition = 0; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelRow.push_back( 0.0 );
                }

                kernelXY.push_back( kernelRow );
            }
        }

        if( GetNormalizeKernel() )
        {
            KTDEBUG(sdlog, "Normalizing kernel");
            for( int iPosition = 0; iPosition < GetBlockSizeTime(); ++iPosition )
            {
                for( int jPosition = 0; jPosition < GetBlockSizeFreq(); ++jPosition )
                {
                    kernelXY[iPosition][jPosition] = kernelXY[iPosition][jPosition] / norm;
                }
            }
        }

        KTINFO(sdlog, "Successfully parsed kernel!");
        return true;
    }

    bool KTConvolution2D::Convolve1D( KTPowerSpectrumData& data )
    {
        KTINFO(sdlog, "Received power spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedPowerSpectrumData& newData = data.Of< KTConvolvedPowerSpectrumData >();
        
        return CoreConvolve1D( static_cast< KTPowerSpectrumDataCore& >(data), newData );
    }

    bool KTConvolution2D::Convolve1D( KTFrequencySpectrumDataFFTW& data )
    {
        KTINFO(sdlog, "Received frequency spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedFrequencySpectrumDataFFTW& newData = data.Of< KTConvolvedFrequencySpectrumDataFFTW >();
        
        return CoreConvolve1D( static_cast< KTFrequencySpectrumDataFFTWCore& >(data), newData );
    }

    bool KTConvolution2D::Convolve1D( KTFrequencySpectrumDataPolar& data )
    {
        KTINFO(sdlog, "Received frequency spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedFrequencySpectrumDataPolar& newData = data.Of< KTConvolvedFrequencySpectrumDataPolar >();
        
        return CoreConvolve1D( static_cast< KTFrequencySpectrumDataPolarCore& >(data), newData );
    }

    template< class XSpectrumDataCore, class XConvolvedSpectrumTypeData >
    bool KTConvolution2D::CoreConvolve1D( XSpectrumDataCore& data, XConvolvedSpectrumTypeData& newData )
    {
        newData.SetNComponents( data.GetNComponents() );

        // Set overlap-and-save method parameters
        // These do not change, but I want to group them together like this so it's easy to follow
        int block = GetBlockSize();
        int overlap = fKernelSize - 1;
        int step = block - overlap;

        KTINFO(sdlog, "Block size: " << block);
        KTINFO(sdlog, "Overlap: " << overlap);
        KTINFO(sdlog, "Step size: " << step);

        int nBinsTotal = GetSpectrum( data, 0 )->GetNFrequencyBins();
        KTINFO(sdlog, "nBinsTotal = " << nBinsTotal);

        // Now that nBinsTotal is determined, we can initialize the DFTs
        // The following conditional should only be true on the first slice
        if( ! fInitialized )
        {
            Initialize( nBinsTotal, block, step, overlap );
        }

        // First loop over components
        for( unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent )
        {
            KTINFO(sdlog, "Starting component: " << iComponent);

            // Get power spectrum and initialize convolved spectrum for this component
            typename XSpectrumDataCore::spectrum_type* transformedSpectrum = DoConvolution( GetSpectrum( data, iComponent ), block, step, overlap );

            if( transformedSpectrum == nullptr )
            {
                KTERROR( sdlog, "Convolution was unsuccessful. Aborting." );
                return false;
            }

            // Set power spectrum
            newData.SetSpectrum( transformedSpectrum, iComponent );
            KTDEBUG(sdlog, "Filled new spectrum");
        }

        KTINFO(sdlog, "All components finished successfully!");

        return true;
    }

    template< class XSpectraType >
    XSpectraType* KTConvolution2D::DoConvolution( const XSpectraType* myInitialSpectrum, const int block, const int step, const int overlap )
    {
        // Block loop parameters
        int nBin = 0;
        int nBinsTotal = myInitialSpectrum->GetNFrequencyBins();
        int blockNumber = 0;
        int position = 0;

        // non-const version
        XSpectraType* initialSpectrum = new XSpectraType( *myInitialSpectrum );

        // If we're doing cross-correlation, first we need to conjugate and reverse the input spectrum
        if( fTransformType == "cross-correlation" )
        {
            ConjugateAndReverse( *initialSpectrum );
        }
        
        XSpectraType* transformedSpectrum = new XSpectraType( nBinsTotal, initialSpectrum->GetRangeMin(), initialSpectrum->GetRangeMax() );

        if( ! SetUpGeneralVars< XSpectraType* >() )
        {
            KTERROR(sdlog, "Spectrum type unknown. Returning blank spectrum");
            return transformedSpectrum;
        }

        // Loop over block numbers
        while( (blockNumber+1) * step <= nBinsTotal )
        {
            KTDEBUG(sdlog, "Block number: " << blockNumber);
            KTDEBUG(sdlog, "Starting position: " << blockNumber * step - overlap);
            KTDEBUG(sdlog, "nBinsTotal: " << nBinsTotal);

            // Fill input array
            for( nBin = 0; nBin < block; ++nBin )
            {
                position = nBin + blockNumber * step - overlap;
                SetInputArray( position, nBin, initialSpectrum );
            }

            // FFT of input block
            KTDEBUG(sdlog, "Performing DFT");
            fftw_execute( fGeneralForwardPlan );

            // Bin multiplication in fourier space
            KTDEBUG(sdlog, "Multiplying arrays in fourier space");

            for( int nBin = 0; nBin < nBinLimitRegular; ++nBin )
            {
                fGeneralTransformedOutputArray[nBin][0] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][0] - fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][1];
                fGeneralTransformedOutputArray[nBin][1] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][1] + fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][0];
            }

            // Reverse FFT of output block
            KTDEBUG(sdlog, "Performing reverse DFT");
            fftw_execute( fGeneralReversePlan );
            
            // Loop over bins in the output block and fill the convolved spectrum
            for( nBin = overlap; nBin < block; ++nBin )
            {
                SetOutputArray( nBin - overlap + blockNumber * step, nBin, *transformedSpectrum, block );
            }

            // Increment block number
            ++blockNumber;
        }

        if( blockNumber * step == nBinsTotal )
        {
            KTINFO(sdlog, "Reached end of input data");
            return transformedSpectrum;
        }

        KTINFO(sdlog, "Reached final block");
        KTINFO(sdlog, "Starting position: " << blockNumber * step - overlap);

        // Same procedure as above, this time with a shorter final block

        for( nBin = 0; position+1 < nBinsTotal; ++nBin )
        {
            position = nBin + blockNumber * step - overlap;
            SetInputArray( position, nBin, initialSpectrum );
        }

        KTINFO(sdlog, "Short array length = " << nBin);
        KTDEBUG(sdlog, "Initialized short array length = " << fShortSize);

        // FFT of input block
        fftw_execute( fGeneralForwardPlanShort );

        for( int nBin = 0; nBin < nBinLimitShort; ++nBin )
        {
            fGeneralTransformedOutputArray[nBin][0] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][0] - fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][1];
            fGeneralTransformedOutputArray[nBin][1] = fGeneralTransformedInputArray[nBin][0] * fGeneralTransformedKernelArray[nBin][1] + fGeneralTransformedInputArray[nBin][1] * fGeneralTransformedKernelArray[nBin][0];
        }

        // Reverse FFT of output block
        fftw_execute( fGeneralReversePlanShort );
        
        // Loop over bins in the output block and fill the convolved spectrum
        for( nBin = overlap; nBin < fShortSize; ++nBin )
        {
            SetOutputArray( nBin - overlap + blockNumber * step, nBin, *transformedSpectrum, fShortSize );
        }

        KTINFO(sdlog, "Component finished!");

        return transformedSpectrum;
    }

    template< class XSpectraType >
    bool KTConvolution2D::SetUpGeneralVars()
    {
        return false;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTPowerSpectrum* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArrayFromReal;
        fGeneralTransformedOutputArray = fTransformedOutputArrayFromReal;
        fGeneralTransformedKernelArray = fTransformedKernelXAsReal;
        fGeneralForwardPlan = fRealToComplexPlan;
        fGeneralForwardPlanShort = fRealToComplexPlanShort;
        fGeneralReversePlan = fComplexToRealPlan;
        fGeneralReversePlanShort = fComplexToRealPlanShort;
        nBinLimitRegular = fRegularSize/2 + 1;
        nBinLimitShort = fShortSize/2 + 1;

        return true;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTFrequencySpectrumFFTW* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArray;
        fGeneralTransformedOutputArray = fTransformedOutputArray;
        fGeneralTransformedKernelArray = fTransformedKernelXAsComplex;
        fGeneralForwardPlan = fC2CForwardPlan;
        fGeneralForwardPlanShort = fC2CForwardPlanShort;
        fGeneralReversePlan = fC2CReversePlan;
        fGeneralReversePlanShort = fC2CReversePlanShort;
        nBinLimitRegular = fRegularSize;
        nBinLimitShort = fShortSize;

        return true;
    }

    template<>
    bool KTConvolution2D::SetUpGeneralVars< KTFrequencySpectrumPolar* >()
    {
        fGeneralTransformedInputArray = fTransformedInputArray;
        fGeneralTransformedOutputArray = fTransformedOutputArray;
        fGeneralTransformedKernelArray = fTransformedKernelXAsComplex;
        fGeneralForwardPlan = fC2CForwardPlan;
        fGeneralForwardPlanShort = fC2CForwardPlanShort;
        fGeneralReversePlan = fC2CReversePlan;
        fGeneralReversePlanShort = fC2CReversePlanShort;
        nBinLimitRegular = fRegularSize;
        nBinLimitShort = fShortSize;

        return true;
    }

    void KTConvolution2D::ConjugateAndReverse( KTPowerSpectrum& spectrum )
    {
        double temp;
        int nBins = spectrum.GetNFrequencyBins();
        for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
        {
            temp = spectrum(iBin);
            spectrum(iBin) = spectrum(nBins - iBin - 1);
            spectrum(nBins - iBin - 1) = temp;
        }

        return;
    }

    void KTConvolution2D::ConjugateAndReverse( KTFrequencySpectrumFFTW& spectrum )
    {
        fftw_complex temp;
        int nBins = spectrum.GetNFrequencyBins();
        for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
        {
            temp[0] = spectrum(iBin)[0];
            temp[1] = spectrum(iBin)[1];
            spectrum(iBin)[0] = spectrum(nBins - iBin - 1)[0];
            spectrum(iBin)[1] = -1. * spectrum(nBins - iBin - 1)[1];
            spectrum(nBins - iBin - 1)[0] = temp[0];
            spectrum(nBins - iBin - 1)[1] = -1. * temp[1];
        }

        return;
    }

    void KTConvolution2D::ConjugateAndReverse( KTFrequencySpectrumPolar& spectrum )
    {
        fftw_complex temp;
        int nBins = spectrum.GetNFrequencyBins();
        for( int iBin = 0; iBin < (nBins+1)/2; ++iBin )
        {
            temp[0] = spectrum.GetReal( iBin );
            temp[1] = spectrum.GetImag( iBin );
            spectrum.SetRect( iBin, spectrum.GetReal(nBins - iBin - 1), -1. * spectrum.GetImag(nBins - iBin - 1) );
            spectrum.SetRect( nBins - iBin - 1, temp[0], -1. * temp[1] );
        }

        return;
    }

    void KTConvolution2D::SetInputArray( int position, int nBin, const KTPowerSpectrum* initialSpectrum )
    {

        if( position < 0 ) { fInputArrayReal[nBin] = 0.0; }
        else { fInputArrayReal[nBin] = (*initialSpectrum)(position); }

        return;
    }

    void KTConvolution2D::SetInputArray( int position, int nBin, const KTFrequencySpectrumFFTW* initialSpectrum )
    {
        if( position < 0 )
        {
            fInputArrayComplex[nBin][0] = 0.0;
            fInputArrayComplex[nBin][1] = 0.0;
        }
        else
        {
            fInputArrayComplex[nBin][0] = (*initialSpectrum)(position)[0];
            fInputArrayComplex[nBin][1] = (*initialSpectrum)(position)[1];
        }

        return;
    }

    void KTConvolution2D::SetInputArray( int position, int nBin, const KTFrequencySpectrumPolar* initialSpectrum )
    {
        if( position < 0 )
        {
            fInputArrayComplex[nBin][0] = 0.0;
            fInputArrayComplex[nBin][1] = 0.0;
        }
        else
        {
            fInputArrayComplex[nBin][0] = initialSpectrum->GetReal(position);
            fInputArrayComplex[nBin][1] = initialSpectrum->GetImag(position);
        }

        return;
    }

    void KTConvolution2D::SetOutputArray( int position, int nBin, KTPowerSpectrum& transformedPS, double norm )
    {
        transformedPS(position) = fOutputArrayReal[nBin] / (double)norm;

        return;
    }

    void KTConvolution2D::SetOutputArray( int position, int nBin, KTFrequencySpectrumFFTW& transformedFSFFTW, double norm )
    {
        transformedFSFFTW(position)[0] = fOutputArrayComplex[nBin][0] / (double)norm;
        transformedFSFFTW(position)[1] = fOutputArrayComplex[nBin][1] / (double)norm;

        return;
    }

    void KTConvolution2D::SetOutputArray( int position, int nBin, KTFrequencySpectrumPolar& transformedFSPolar, double norm )
    {
        transformedFSPolar.SetRect( position, fOutputArrayComplex[nBin][0] / (double)norm, fOutputArrayComplex[nBin][1] / (double)norm );

        return;
    }

    void KTConvolution2D::SetupInternalMaps()
    {
        KTDEBUG(sdlog, "Setting up internal maps");

        // transform flag map
        fTransformFlagMap.clear();
        fTransformFlagMap["ESTIMATE"] = FFTW_ESTIMATE;
        fTransformFlagMap["MEASURE"] = FFTW_MEASURE;
        fTransformFlagMap["PATIENT"] = FFTW_PATIENT;
        fTransformFlagMap["EXHAUSTIVE"] = FFTW_EXHAUSTIVE;
        return;
    }

    void KTConvolution2D::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(sdlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        fTransformFlag = flag;

        return;
    }

    void KTConvolution2D::Initialize( int nBinsTotal, int block, int step, int overlap )
    {
        KTINFO(sdlog, "DFTs are not yet initialized; doing so now");

        // Slightly obnoxious conditionals to determine the sizes
        // Ordinary block is given, but the shorter block at the end is harder
        // Maybe there is a simpler way to determine this, I'm not sure

        if( (nBinsTotal + overlap) % step >= overlap )
        {
            AllocateArrays( block, (nBinsTotal + overlap) % step );
        }
        else
        {
            AllocateArrays( block, ((nBinsTotal + overlap) % step) + step );
        }

        // Also transform the kernel, we only need to do this once
        KTDEBUG(sdlog, "Transforming kernel");

        for( int iBin = 0; iBin < block; ++iBin )
        {
            fInputArrayReal[iBin] = kernelX[iBin];
            fInputArrayComplex[iBin][0] = kernelX[iBin];
            fInputArrayComplex[iBin][1] = 0.;
        }

        fftw_execute( fRealToComplexPlan );
        fftw_execute( fC2CForwardPlan );
        
        fTransformedKernelXAsReal = new fftw_complex[block];
        fTransformedKernelXAsComplex = new fftw_complex[block];
        for( int iBin = 0; iBin < block; ++iBin )
        {
            fTransformedKernelXAsReal[iBin][0] = fTransformedInputArrayFromReal[iBin][0];
            fTransformedKernelXAsReal[iBin][1] = fTransformedInputArrayFromReal[iBin][1];
            fTransformedKernelXAsComplex[iBin][0] = fTransformedInputArray[iBin][0];
            fTransformedKernelXAsComplex[iBin][1] = fTransformedInputArray[iBin][1];
        }
    }


} /* namespace Katydid */
