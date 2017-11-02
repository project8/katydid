/*
 * KTConvolution1D.cc
 *
 *  Created on: Aug 25, 2017
 *      Author: ezayas
 */

#include "KTConvolution.hh"

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
    KTLOGGER(sdlog, "KTConvolution1D");

    KT_REGISTER_PROCESSOR(KTConvolution1D, "convolution");

    KTConvolution1D::KTConvolution1D(const std::string& name) :
            KTProcessor(name),
            fKernel("placeholder.json"),
            fBlockSize(0),
            fNormalizeKernel(false),
            fTransformType("convolution"),
            fTransformFlag("ESTIMATE"),
            fRegularSize(0),
            fShortSize(0),
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
            nBinLimitRegular(0),
            nBinLimitShort(0),
            fGeneralForwardPlan(),
            fGeneralReversePlan(),
            fGeneralForwardPlanShort(),
            fGeneralReversePlanShort(),
            fTransformFlagUnsigned(FFTW_ESTIMATE),
            fKernelSize(0),
            fInitialized(false),
            fPSSignal("ps", this),
            fFSFFTWSignal("fs-fftw", this),
            fFSPolarSignal("fs-polar", this),
            fPSSlot("ps", this, &KTConvolution1D::Convolve1D, &fPSSignal),
            fFSFFTWSlot("fs-fftw", this, &KTConvolution1D::Convolve1D, &fFSFFTWSignal),
            fFSPolarSlot("fs-polar", this, &KTConvolution1D::Convolve1D, &fFSPolarSignal)
    {
        SetupInternalMaps();
    }

    KTConvolution1D::~KTConvolution1D()
    {
        FreeArrays();
    }

    bool KTConvolution1D::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetKernel(node->get_value< std::string >("kernel", GetKernel()));
        SetBlockSize(node->get_value< unsigned >("block-size", GetBlockSize()));
        SetTransformType(node->get_value< std::string >("transform-type", GetTransformType()));
        SetTransformFlag(node->get_value< std::string >("transform-flag", GetTransformFlag()));

        return FinishSetup();
    }

    bool KTConvolution1D::FinishSetup()
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

    void KTConvolution1D::AllocateArrays( int nSizeRegular, int nSizeShort )
    {
        KTDEBUG(sdlog, "DFT initialization started");
        KTDEBUG(sdlog, "Regular size = " << nSizeRegular);
        KTDEBUG(sdlog, "Short size = " << nSizeShort);

        if( nSizeShort > nSizeRegular )
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
        fInputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegular );
        fOutputArrayReal = (double*) fftw_malloc( sizeof( double ) * nSizeRegular );
        fInputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );
        fOutputArrayComplex = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );

        // Intermediate (fourier space) arrays
        fTransformedInputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );
        fTransformedOutputArray = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * nSizeRegular );
        fTransformedInputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * (nSizeRegular/2 + 1) );
        fTransformedOutputArrayFromReal = (fftw_complex*) fftw_malloc( sizeof( fftw_complex ) * (nSizeRegular/2 + 1) );

        // DFT plans
        fRealToComplexPlan = fftw_plan_dft_r2c_1d( nSizeRegular, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlan = fftw_plan_dft_c2r_1d( nSizeRegular, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlan = fftw_plan_dft_1d( nSizeRegular, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlan = fftw_plan_dft_1d( nSizeRegular, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        fRealToComplexPlanShort = fftw_plan_dft_r2c_1d( nSizeShort, fInputArrayReal, fTransformedInputArrayFromReal, fTransformFlagUnsigned );
        fComplexToRealPlanShort = fftw_plan_dft_c2r_1d( nSizeShort, fTransformedOutputArrayFromReal, fOutputArrayReal, fTransformFlagUnsigned );
        fC2CForwardPlanShort = fftw_plan_dft_1d( nSizeShort, fInputArrayComplex, fTransformedInputArray, FFTW_FORWARD, fTransformFlagUnsigned );
        fC2CReversePlanShort = fftw_plan_dft_1d( nSizeShort, fTransformedOutputArray, fOutputArrayComplex, FFTW_BACKWARD, fTransformFlagUnsigned );

        // All the same for short size
        // But we can skip this if the size is chosen right

        if( nSizeShort == fKernelSize - 1 )
        {
            KTINFO(sdlog, "Input blocks are divided evenly! I will only initialize one group of FFTW plans");

            fRegularSize = nSizeRegular;
            fShortSize = nSizeShort;
            fInitialized = true;

            return;
        }

        // Store these guys
        fRegularSize = nSizeRegular;
        fShortSize = nSizeShort;

        // Make sure we don't come back here unless something weird happens
        fInitialized = true;

        return;
    }  

    void KTConvolution1D::FreeArrays()
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

    bool KTConvolution1D::ParseKernel()
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
        scarab::param_array& kernel1DArray = kernelNode["kernel"].as_array();
        KTDEBUG(sdlog, "Obtained kernel as array");

        // We need to store this because we're about to periodically extend it
        fKernelSize = kernel1DArray.size();
        KTDEBUG(sdlog, "Kernel size = " << fKernelSize);

        // Fill kernel vector
        // Also calculate the norm in case we need that

        double norm = 0.;
        for( int iValue = 0; iValue < fKernelSize; ++iValue )
        {
            kernelX.push_back( kernel1DArray.get_value< double >(iValue) );
            norm += kernelX[iValue];
        }
        KTDEBUG(sdlog, "Kernel norm = " << norm);

        // Here is where we set block size if left unspecified
        // We need it to periodically extend the kernel
        if( GetBlockSize() == 0 )
        {
            SetBlockSize( 8 * kernelX.size() );
            int power = log2( GetBlockSize() ); // int will take the floor of the log
            SetBlockSize( pow( 2, power ) );    // largest power of 2 which is <= 8 * kernel size
        }
        KTDEBUG(sdlog, "Set block size: " << GetBlockSize());

        // Check that kernel size is not more than block size
        if( GetBlockSize() < kernelX.size() )
        {
            KTERROR( sdlog, "Block size is smaller than kernel length. Aborting." );
            return false;
        }

        // Periodically extend kernel up to block size
        for( int iPosition = fKernelSize; iPosition < GetBlockSize(); ++iPosition )
        {
            kernelX.push_back( 0.0 );
        }

        if( GetNormalizeKernel() )
        {
            KTDEBUG(sdlog, "Normalizing kernel");
            for( int iPosition = 0; iPosition < GetBlockSize(); ++iPosition )
            {
                kernelX[iPosition] = kernelX[iPosition] / norm;
            }
        }

        KTINFO(sdlog, "Successfully parsed kernel!");
        return true;
    }

    bool KTConvolution1D::Convolve1D( KTPowerSpectrumData& data )
    {
        KTINFO(sdlog, "Received power spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedPowerSpectrumData& newData = data.Of< KTConvolvedPowerSpectrumData >();
        
        return CoreConvolve1D( static_cast< KTPowerSpectrumDataCore& >(data), newData );
    }

    bool KTConvolution1D::Convolve1D( KTFrequencySpectrumDataFFTW& data )
    {
        KTINFO(sdlog, "Received frequency spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedFrequencySpectrumDataFFTW& newData = data.Of< KTConvolvedFrequencySpectrumDataFFTW >();
        
        return CoreConvolve1D( static_cast< KTFrequencySpectrumDataFFTWCore& >(data), newData );
    }

    bool KTConvolution1D::Convolve1D( KTFrequencySpectrumDataPolar& data )
    {
        KTINFO(sdlog, "Received frequency spectrum. Performing 1D convolution");
        // New data object
        KTConvolvedFrequencySpectrumDataPolar& newData = data.Of< KTConvolvedFrequencySpectrumDataPolar >();
        
        return CoreConvolve1D( static_cast< KTFrequencySpectrumDataPolarCore& >(data), newData );
    }

    template< class XSpectrumDataCore, class XConvolvedSpectrumTypeData >
    bool KTConvolution1D::CoreConvolve1D( XSpectrumDataCore& data, XConvolvedSpectrumTypeData& newData )
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
    XSpectraType* KTConvolution1D::DoConvolution( const XSpectraType* myInitialSpectrum, const int block, const int step, const int overlap )
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
    bool KTConvolution1D::SetUpGeneralVars()
    {
        return false;
    }

    template<>
    bool KTConvolution1D::SetUpGeneralVars< KTPowerSpectrum* >()
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
    bool KTConvolution1D::SetUpGeneralVars< KTFrequencySpectrumFFTW* >()
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
    bool KTConvolution1D::SetUpGeneralVars< KTFrequencySpectrumPolar* >()
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

    void KTConvolution1D::ConjugateAndReverse( KTPowerSpectrum& spectrum )
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

    void KTConvolution1D::ConjugateAndReverse( KTFrequencySpectrumFFTW& spectrum )
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

    void KTConvolution1D::ConjugateAndReverse( KTFrequencySpectrumPolar& spectrum )
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

    void KTConvolution1D::SetInputArray( int position, int nBin, const KTPowerSpectrum* initialSpectrum )
    {

        if( position < 0 ) { fInputArrayReal[nBin] = 0.0; }
        else { fInputArrayReal[nBin] = (*initialSpectrum)(position); }

        return;
    }

    void KTConvolution1D::SetInputArray( int position, int nBin, const KTFrequencySpectrumFFTW* initialSpectrum )
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

    void KTConvolution1D::SetInputArray( int position, int nBin, const KTFrequencySpectrumPolar* initialSpectrum )
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

    void KTConvolution1D::SetOutputArray( int position, int nBin, KTPowerSpectrum& transformedPS, double norm )
    {
        transformedPS(position) = fOutputArrayReal[nBin] / (double)norm;

        return;
    }

    void KTConvolution1D::SetOutputArray( int position, int nBin, KTFrequencySpectrumFFTW& transformedFSFFTW, double norm )
    {
        transformedFSFFTW(position)[0] = fOutputArrayComplex[nBin][0] / (double)norm;
        transformedFSFFTW(position)[1] = fOutputArrayComplex[nBin][1] / (double)norm;

        return;
    }

    void KTConvolution1D::SetOutputArray( int position, int nBin, KTFrequencySpectrumPolar& transformedFSPolar, double norm )
    {
        transformedFSPolar.SetRect( position, fOutputArrayComplex[nBin][0] / (double)norm, fOutputArrayComplex[nBin][1] / (double)norm );

        return;
    }

    void KTConvolution1D::SetupInternalMaps()
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

    void KTConvolution1D::SetTransformFlag(const std::string& flag)
    {
        if (fTransformFlagMap.find(flag) == fTransformFlagMap.end())
        {
            KTWARN(sdlog, "Invalid transform flag requested: " << flag << "\n\tNo change was made.");
            return;
        }

        fTransformFlag = flag;

        return;
    }

    void KTConvolution1D::Initialize( int nBinsTotal, int block, int step, int overlap )
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
