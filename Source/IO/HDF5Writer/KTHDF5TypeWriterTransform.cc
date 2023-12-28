/*
 * KTHDF5TypeWriterTransform.cc
 *
 *  Created on: 9/12/2014
 *      Author: J.N. Kofron
 */

#include <string>
#include <sstream>

#include "KTTIFactory.hh"
#include "logger.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTHDF5TypeWriterTransform.hh"
#include "KTSliceHeader.hh"
#include "KTMultiFSDataPolar.hh"
#include "KTMultiFSDataFFTW.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTTimeFrequencyDataPolar.hh"
#include "KTTimeFrequencyPolar.hh"


namespace Katydid {
    LOGGER(publog, "KTHDF5TypeWriterTransform");

    static Nymph::KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterTransform> sH5TWFFTReg;

    KTHDF5TypeWriterTransform::KTHDF5TypeWriterTransform() :
            KTHDF5TypeWriter(),
            fFFTBuffer(NULL),
            fFFTFreqArrayBuffer(NULL),
            fFFTDataSpace(NULL),
            fFFTFreqArrayDataSpace(NULL),
            fSpectraGroup(NULL),
            fFFTGroup(NULL),
            fDSet(NULL),
            fDSet_FreqArray(NULL),
            fFirstSliceHasBeenWritten(false),
            fUseCompressionFlag(false)
    {}

    KTHDF5TypeWriterTransform::~KTHDF5TypeWriterTransform()
    {
        if(fFFTBuffer) delete fFFTBuffer;
        if(fFFTFreqArrayBuffer) delete fFFTFreqArrayBuffer;
        if(fFFTDataSpace) delete fFFTDataSpace;
        if(fFFTFreqArrayDataSpace) delete fFFTFreqArrayDataSpace;
    }

    void KTHDF5TypeWriterTransform::RegisterSlots()
    {
        fWriter->RegisterSlot("fs-polar", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTW);
        //fWriter->RegisterSlot("fs-polar-phase", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPhase);
        //fWriter->RegisterSlot("fs-fftw-phase", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPower);
        //fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution);
        //fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution);
        //fWriter->RegisterSlot("fs-polar-power-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution);
        //fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("ps", this, &KTHDF5TypeWriterTransform::WritePowerSpectrum);
        fWriter->RegisterSlot("psd", this, &KTHDF5TypeWriterTransform::WritePowerSpectralDensity);
        //fWriter->RegisterSlot("ps-dist", this, &KTHDF5TypeWriterTransform::WritePowerSpectrumDistribution);
        //fWriter->RegisterSlot("psd-dist", this, &KTHDF5TypeWriterTransform::WritePowerSpectralDensityDistribution);
        //fWriter->RegisterSlot("tf-polar", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolar);
        //fWriter->RegisterSlot("tf-polar-phase", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPhase);
        //fWriter->RegisterSlot("tf-polar-power", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPower);
        //fWriter->RegisterSlot("multi-fs-polar", this, &KTHDF5TypeWriterTransform::WriteMultiFSDataPolar);
        //fWriter->RegisterSlot("multi-fs-fftw", this, &KTHDF5TypeWriterTransform::WriteMultiFSDataFFTW);
    }


    void KTHDF5TypeWriterTransform::ProcessEggHeader()
    {
        KTEggHeader* header = fWriter->GetHeader();
        if(header != NULL)
        {
            LDEBUG(publog, "Configuring from Egg header...");
            fNChannels = header->GetNChannels();
            LDEBUG(publog, "Header Number of Channels: " << header->GetNChannels());
            fSliceSize = header->GetChannelHeader(0)->GetSliceSize();
            //fNumberOfSlices = (int) ceil ( (double) (header->GetChannelHeader(0)->GetRecordSize()) / (double) (fSliceSize));
            fNumberOfSlices = header->GetChannelHeader(0)->GetRecordSize()/fSliceSize + (header->GetChannelHeader(0)->GetRecordSize() % fSliceSize != 0);
            LDEBUG(publog, "Number of Slices: " << fNumberOfSlices);
        }
        LDEBUG(publog, "Done.");
        if ( fWriter->GetUseCompressionFlag() )
        {
            SetUseCompressionFlag(true);
        }
        fSpectraGroup = fWriter->AddGroup("/spectra");
    }

    void KTHDF5TypeWriterTransform::PrepareHDF5File()
    {

        if( !fWriter->OpenAndVerifyFile() ) return;

        /*
        A Buffer is the memory array where the data is copied to right before being written to file
        A Dataspace is what gets written to the HDF5 file.
        Buffers and Dataspaces will have 4 dimensions:
        1 - Number of channels (sometimes called fNComponents in other modules)
        2 - Number of slices
        3 - Number of Parts (real-only: 1;  real and imaginary: 2;  polar amplitude and angle: 2)
        4 - Number of samples in slice
        */

        /*
         * Defining Buffer and Dataset Size
         *
         * There are 2 choices to make:  Polar vs Complex ;  FFT Amplitude vs Power
         *
         * The choices are made in the functions called by the slot
         * i.e.: WriteFrequencySpectrumDataFFTW, WriteFrequencySpectrumDataPolar, etc...
         * There are two variables that need to be defined in theses functions before calling PrepareHDF5File:
         * (1) fFFTSize  and  (2) fNParts
         *
         * (1) FFTSize:  Number of Samples in the FFT in each Slice
         * Polar FFT -> Each row is fSliceSize/2 + 1 long.
         * Complex FFT -> Each row is fSliceSize long.
         * PS and PSD spectrum -> Power is only calculated for positive frequencies and DC. The buffer is therefore the same shape as the polar power spectrum.
         *
         * (2) fNParts: number of parts in each sample: Complex FFT has 2 parts (real and imag), Polar FFT has 2 parts (magnitude and angle), power has 1 part (real)
         *
         */


        //////////////////////////////////////////////////////////////////////////////////
        // Create Buffers
        fFFTBuffer = new fft_buffer(boost::extents[fNChannels][1][fNParts][fFFTSize]);
        // Create arrays to store the frequency bins used in the FFT. The arrays are the same size as the Polar and FFTW spectra
        fFFTFreqArrayBuffer = new freq_buffer(boost::extents[fFFTSize]);


        //////////////////////////////////////////////////////////////////////////////////
        // Create Data Space

        LDEBUG(publog, "Creating HDF5 Dataspace...");

        if(fFFTDataSpace == NULL)
        {
            ds_dims[0] = fNChannels;
            ds_dims[1] = fNumberOfSlices;
            ds_dims[2] = fNParts;
            ds_dims[3] = fFFTSize;
            ds_maxdims[0] = fNChannels;
            ds_maxdims[1] = H5S_UNLIMITED;
            ds_maxdims[2] = fNParts;
            ds_maxdims[3] = fFFTSize;
            fFFTDataSpace = new H5::DataSpace(4, ds_dims,ds_maxdims);
        }
        if(fFFTFreqArrayDataSpace == NULL)
        {
            hsize_t freqarray_dims[] = {fFFTSize};
            fFFTFreqArrayDataSpace = new H5::DataSpace(1, freqarray_dims);
        }


        //////////////////////////////////////////////////////////////////////////////////
        // Create Dataset

        LDEBUG(publog, "Creating Spectrum Dataset in HDF5 file...");
        std::string FreqArray_name ("FreqArray");
        if (fFFTGroup == NULL)       fFFTGroup = new H5::Group(fSpectraGroup->createGroup("/spectra/spectrum"));
        if (fDSet_FreqArray == NULL) fDSet_FreqArray = CreateDSet(FreqArray_name, fFFTGroup, *fFFTFreqArrayDataSpace);
        if (fDSet == NULL)           fDSet = CreateDSet(fSpectrumName, fFFTGroup, *fFFTDataSpace);


        //////////////////////////////////////////////////////////////////////////////////
        // Resize Dataset

        // If sliceN is greater than the number of Slices per Record, then extend the dataset
        // This probably happens because there is more than 1 record in the Egg file being processed
        if ( fSliceNumber >= ds_dims[1] )
        {
            ds_dims[1] = ds_dims[1] + fNumberOfSlices;
            fDSet->extend( ds_dims );
        }

    }

    H5::DataSet* KTHDF5TypeWriterTransform::CreateDSet(const std::string& name, const H5::Group* grp, const H5::DataSpace& ds)
    {
        herr_t   hstatus;
        H5::DSetCreatPropList plist;
        unsigned default_value = 0;
        plist.setFillValue(H5::PredType::NATIVE_DOUBLE, &default_value);
        int rank = ds.getSimpleExtentNdims();
        if (rank==4)
        {
            // Uses 10 slices per chunk; we want a high number to improve compression, but not too high so that it's still fast
            hsize_t ChunkSize[4] = {ds_dims[0], 10, ds_dims[2], ds_dims[3]};
            plist.setChunk(4, ChunkSize);
        }
        if (rank==1)
        {
            hsize_t ChunkSize[1] = {fFFTSize};
            plist.setChunk(1, ChunkSize);
        }
        if ( fUseCompressionFlag )
        {
          plist.setDeflate(6);
          LDEBUG(publog, "Creating compressed HDF5 dataset.");
        }
        LDEBUG(publog, "Creating HDF5 dataset " << name);
        LDEBUG(publog, grp);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(), H5::PredType::NATIVE_DOUBLE, ds, plist));
        LDEBUG("Done.");
        return dset;
    }


    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTW(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = fSliceSize;
            fNParts = 2;
            fSpectrumName = "complexFS";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0, fSliceNumber, 0, 0};
        hsize_t dims1[4] = { ds_dims[0],1, ds_dims[2], ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (Complex FFT) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; ++iC)
        {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL)
            {
                for(int f=0; f < spec[0].size(); f++)
                {
                    (*fFFTBuffer)[iC][0][0][f] = spec[0].GetReal(f);
                    (*fFFTBuffer)[iC][0][1][f] = spec[0].GetImag(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (Complex FFT) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolar(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = (fSliceSize >> 1) + 1;
            fNParts = 2;
            fSpectrumName = "polarFS";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,fSliceNumber,0,0};
        hsize_t dims1[4] = { ds_dims[0],1,ds_dims[2],ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (Polar FFT) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++)
        {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL)
            {
                for(int f=0; f < spec[0].size(); f++)
                {
                    (*fFFTBuffer)[iC][0][0][f] = spec[0].GetAbs(f);
                    (*fFFTBuffer)[iC][0][1][f] = spec[0].GetArg(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (Polar FFT) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten)
        {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1, filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");

    }

/*
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
*/

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = (fSliceSize >> 1) + 1;
            fNParts = 1;
            fSpectrumName = "polarPS";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0, fSliceNumber, 0, 0};
        hsize_t dims1[4] = { ds_dims[0], 1, ds_dims[2], ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (Polar PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; ++iC)
        {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL)
            {
                for(int f=0; f < spec[0].size(); ++f)
                {
                    (*fFFTBuffer)[iC][0][0][f] = sqrt(spec[0].GetAbs(f)*spec[0].GetAbs(f) + spec[0].GetArg(f)*spec[0].GetArg(f));
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (Polar PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten)
        {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1, filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = fSliceSize;
            fNParts = 1;
            fSpectrumName = "complexPS";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0, fSliceNumber, 0, 0};
        hsize_t dims1[4] = { ds_dims[0], 1, ds_dims[2], ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (Complex PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; ++iC)
        {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL)
            {
                for(int f=0; f < spec[0].size(); ++f)
                {
                    (*fFFTBuffer)[iC][0][0][f] = sqrt(spec[0].GetReal(f)*spec[0].GetReal(f) + spec[0].GetImag(f)*spec[0].GetImag(f));
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (Complex PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten)
        {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1, filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");
    }

/*
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
*/

    void KTHDF5TypeWriterTransform::WritePowerSpectrum(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = (fSliceSize >> 1) + 1;
            fNParts = 1;
            fSpectrumName = "PS";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0, fSliceNumber, 0, 0};
        hsize_t dims1[4] = { ds_dims[0], 1, ds_dims[2], ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; ++iC)
        {
            KTPowerSpectrum* spec = fsData.GetSpectrum(iC);
            if (spec != NULL)
            {
                spec->ConvertToPowerSpectrum();
                for(int f=0; f < spec[0].size(); ++f)
                {
                    (*fFFTBuffer)[iC][0][0][f] = (*spec)(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1, filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WritePowerSpectralDensity(Nymph::KTDataPtr data)
    {
        if (!data) return;

        // Get Data and Slice Number
        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        LDEBUG(publog, "Writing Slice " << fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() )
        {
            ProcessEggHeader();
            fFFTSize = (fSliceSize >> 1) + 1;
            fNParts = 1;
            fSpectrumName = "PSD";
            PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0, fSliceNumber, 0, 0};
        hsize_t dims1[4] = { ds_dims[0], 1, ds_dims[2], ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Parts].
        // The dataset is [...][Parts][Samples].
        LDEBUG(publog, "Copying Spectrum Dataset (PSD) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; ++iC)
        {
            KTPowerSpectrum* spec = fsData.GetSpectrum(iC);
            if (spec != NULL)
            {
                spec->ConvertToPowerSpectralDensity();
                for(int f=0; f < spec[0].size(); ++f)
                {
                    (*fFFTBuffer)[iC][0][0][f] = (*spec)(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        LDEBUG(publog, "Writing Spectrum Dataset (PSD) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten)
        {
            fDSet_FreqArray->write(fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        fDSet->write(fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1, filespace1);
        LDEBUG(publog, "Done Writing Slice to File.");
    }

/*
    void KTHDF5TypeWriterTransform::WritePowerSpectrumDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WritePowerSpectralDensityDistribution(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolar(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPower(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteMultiFSDataPolar(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteMultiFSDataFFTW(Nymph::KTDataPtr data)
    {
        LDEBUG(publog, "NOT IMPLEMENTED");
    }
*/
}


