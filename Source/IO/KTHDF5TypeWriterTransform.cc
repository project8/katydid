/*
 * KTHDF5TypeWriterTransform.cc
 *
 *  Created on: 9/12/2014
 *      Author: J.N. Kofron
 */

#include <string>
#include <sstream>

#include "KTTIFactory.hh"
#include "KTLogger.hh"
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
    KTLOGGER(publog, "KTHDF5TypeWriterTransform");

    static KTTIRegistrar<KTHDF5TypeWriter, KTHDF5TypeWriterTransform> sH5TWFFTReg;

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
        fCompressFFTFlag(false)
    {}

    KTHDF5TypeWriterTransform::~KTHDF5TypeWriterTransform()
    {
      if(fFFTBuffer) delete fFFTBuffer;
      if(fFFTFreqArrayBuffer) delete fFFTFreqArrayBuffer;
      if(fFFTDataSpace) delete fFFTDataSpace;
      if(fFFTFreqArrayDataSpace) delete fFFTFreqArrayDataSpace;
    }

    void KTHDF5TypeWriterTransform::RegisterSlots() {
        fWriter->RegisterSlot("fs-polar", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolar);
        fWriter->RegisterSlot("fs-fftw", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTW);
        fWriter->RegisterSlot("fs-polar-phase", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPhase);
        fWriter->RegisterSlot("fs-fftw-phase", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase);
        fWriter->RegisterSlot("fs-polar-power", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPower);
        fWriter->RegisterSlot("fs-fftw-power", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPower);
        fWriter->RegisterSlot("fs-polar-mag-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution);
        fWriter->RegisterSlot("fs-fftw-mag-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution);
        fWriter->RegisterSlot("fs-polar-power-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution);
        fWriter->RegisterSlot("fs-fftw-power-dist", this, &KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution);
        fWriter->RegisterSlot("ps", this, &KTHDF5TypeWriterTransform::WritePowerSpectrum);
        fWriter->RegisterSlot("psd", this, &KTHDF5TypeWriterTransform::WritePowerSpectralDensity);
        fWriter->RegisterSlot("ps-dist", this, &KTHDF5TypeWriterTransform::WritePowerSpectrumDistribution);
        fWriter->RegisterSlot("psd-dist", this, &KTHDF5TypeWriterTransform::WritePowerSpectralDensityDistribution);
        fWriter->RegisterSlot("tf-polar", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolar);
        fWriter->RegisterSlot("tf-polar-phase", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPhase);
        fWriter->RegisterSlot("tf-polar-power", this, &KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPower);
        fWriter->RegisterSlot("multi-fs-polar", this, &KTHDF5TypeWriterTransform::WriteMultiFSDataPolar);
        fWriter->RegisterSlot("multi-fs-fftw", this, &KTHDF5TypeWriterTransform::WriteMultiFSDataFFTW);
    }


    void KTHDF5TypeWriterTransform::ProcessEggHeader() {
        KTEggHeader* header = fWriter->GetHeader();
        if(header != NULL) {
            KTDEBUG(publog, "Configuring from Egg header...");
            this->fNChannels = (header->GetNChannels());
            KTDEBUG(publog, "Header Number of Channels: " << header->GetNChannels());
            this->fSliceSize = (header->GetChannelHeader(0)->GetSliceSize());
            //this->fNumberOfSlices = (int) ceil ( (double) (header->GetChannelHeader(0)->GetRecordSize()) / (double) (this->fSliceSize));
            this->fNumberOfSlices = (header->GetChannelHeader(0)->GetRecordSize())/(this->fSliceSize) + ((header->GetChannelHeader(0)->GetRecordSize()) % (this->fSliceSize) != 0);
            KTDEBUG(publog, "Number of Slices: " << this->fNumberOfSlices);
        }
        KTDEBUG(publog, "Done.");
        std::string fFileFlag = fWriter->GetFileFlag();
        if ( fFileFlag.compare("compressfft") == 0 ) {
          SetCompressFFTFlag(true);
          KTDEBUG(publog, "fFileFlag=compressfft");
        }
        this->fSpectraGroup = fWriter->AddGroup("/spectra");
    }

    void KTHDF5TypeWriterTransform::PrepareHDF5File() {

        if( !fWriter->OpenAndVerifyFile() ) return;

        /*
        A Buffer is the memory array where the data is copied to right before being written to file
        A Dataspace is what gets written to the HDF5 file.
        Buffers and Dataspaces will have 4 dimensions:
        1 - Number of channels (sometimes called fNComponents in other modules)
        2 - Number of slices
        3 - Number of components (real-only: 1;  real and imaginary: 2;  polar amplitude and angle: 2)
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
         * (1) fFFTSize  and  (2) fNComponents
         *
         * (1) FFTSize:  Number of Samples in the FFT in each Slice
         * Polar FFT -> Each row is fSliceSize/2 + 1 long.
         * Complex FFT -> Each row is fSliceSize long.
         * PS and PSD spectrum -> Power is only calculated for positive frequencies and DC. The buffer is therefore the same shape as the polar power spectrum.
         *
         * (2) fNComponents: number of components in each sample: Complex FFT has 2 components (real and imag), Polar FFT has 2 components (magnitude and angle), power has 1 component (real)
         *
         */


        //////////////////////////////////////////////////////////////////////////////////
        // Create Buffers
        this->fFFTBuffer = new fft_buffer(boost::extents[fNChannels][1][this->fNComponents][this->fFFTSize]);
        // Create arrays to store the frequency bins used in the FFT. The arrays are the same size as the Polar and FFTW spectra
        this->fFFTFreqArrayBuffer = new freq_buffer(boost::extents[this->fFFTSize]);


        //////////////////////////////////////////////////////////////////////////////////
        // Create Data Space

        KTDEBUG(publog, "Creating HDF5 Dataspace...");

        if(this->fFFTDataSpace == NULL) {
            this->ds_dims[0] = this->fNChannels;
            this->ds_dims[1] = this->fNumberOfSlices;
            this->ds_dims[2] = this->fNComponents;
            this->ds_dims[3] = this->fFFTSize;
            this->ds_maxdims[0] = this->fNChannels;
            this->ds_maxdims[1] = H5S_UNLIMITED;
            this->ds_maxdims[2] = this->fNComponents;
            this->ds_maxdims[3] = this->fFFTSize;
            this->fFFTDataSpace = new H5::DataSpace(4, this->ds_dims,this->ds_maxdims);
        }
        if(this->fFFTFreqArrayDataSpace == NULL) {
            hsize_t freqarray_dims[] = {this->fFFTSize};
            this->fFFTFreqArrayDataSpace = new H5::DataSpace(1, freqarray_dims);
        }


        //////////////////////////////////////////////////////////////////////////////////
        // Create Dataset

        KTDEBUG(publog, "Creating Spectrum Dataset in HDF5 file...");
        std::string FreqArray_name ("FreqArray");
        if(this->fFFTGroup == NULL)       this->fFFTGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/spectrum"));
        if(this->fDSet_FreqArray == NULL) this->fDSet_FreqArray = this->CreateDSet(FreqArray_name, this->fFFTGroup, *(this->fFFTFreqArrayDataSpace));
        if(this->fDSet == NULL)           this->fDSet = this->CreateDSet(this->fSpectrumName, this->fFFTGroup, *(this->fFFTDataSpace));


        //////////////////////////////////////////////////////////////////////////////////
        // Resize Dataset

        // If sliceN is greater than the number of Slices per Record, then extend the dataset
        // This probably happens because there is more than 1 record in the Egg file being processed
        if ( this->fSliceNumber >= this->ds_dims[1] ) {
            this->ds_dims[1] = this->ds_dims[1] + this->fNumberOfSlices;
            this->fDSet->extend( this->ds_dims );
        }

    }

    H5::DataSet* KTHDF5TypeWriterTransform::CreateDSet(const std::string& name,
                                                 const H5::Group* grp,
                                                 const H5::DataSpace& ds) {
        herr_t   hstatus;
        H5::DSetCreatPropList plist;
        unsigned default_value = 0;
        plist.setFillValue(H5::PredType::NATIVE_DOUBLE, &default_value);
        int rank = ds.getSimpleExtentNdims();
        if (rank==4) {
            hsize_t ChunkSize[4] = {this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]};
            plist.setChunk(4, ChunkSize);
        }
        if (rank==1) {
            hsize_t ChunkSize[1] = {this->fFFTSize};
            plist.setChunk(1, ChunkSize);
        }
        if ( fCompressFFTFlag ) {
          plist.setDeflate(6);
          KTDEBUG(publog, "Creating compressed HDF5 dataset.");
        }
        KTDEBUG(publog, "Creating HDF5 dataset " << name);
        KTDEBUG(publog, grp);
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               H5::PredType::NATIVE_DOUBLE,
                                                               ds,
                                                               plist));
        KTDEBUG("Done.");
        return dset;
    }


    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTW(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = this->fSliceSize;
            this->fNComponents = 2;
            this->fSpectrumName = "complexFS";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (Complex FFT) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = spec[0].GetReal(f);
                    (*this->fFFTBuffer)[iC][0][1][f] = spec[0].GetImag(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (Complex FFT) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolar(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = (this->fSliceSize >> 1) + 1;
            this->fNComponents = 2;
            this->fSpectrumName = "polarFS";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (Polar FFT) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = spec[0].GetAbs(f);
                    (*this->fFFTBuffer)[iC][0][1][f] = spec[0].GetArg(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (Polar FFT) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");

    }


    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPower(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = (this->fSliceSize >> 1) + 1;
            this->fNComponents = 1;
            this->fSpectrumName = "polarPS";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (Polar PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = sqrt(pow(spec[0].GetAbs(f), 2.0) + pow(spec[0].GetArg(f),2.0));
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (Polar PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPower(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = this->fSliceSize;
            this->fNComponents = 1;
            this->fSpectrumName = "complexPS";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (Complex PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = sqrt(pow(spec[0].GetReal(f),2.0) + pow(spec[0].GetImag(f),2.0));
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (Complex PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }


    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }

    void KTHDF5TypeWriterTransform::WritePowerSpectrum(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = (this->fSliceSize >> 1) + 1;
            this->fNComponents = 1;
            this->fSpectrumName = "PS";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (PS) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            KTPowerSpectrum* spec = fsData.GetSpectrum(iC);
            if (spec != NULL) {
                spec->ConvertToPowerSpectrum();
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = (*spec)(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (PS) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WritePowerSpectralDensity(KTDataPtr data) {
        if (!data) return;

        // Get Data and Slice Number
        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        this->fSliceNumber = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << this->fSliceNumber);

        // Prepare HDF5 for writing
        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->fFFTSize = (this->fSliceSize >> 1) + 1;
            this->fNComponents = 1;
            this->fSpectrumName = "PSD";
            this->PrepareHDF5File();
        }
        else return;

        // Select hyperslabs to save the spectrum data
        hsize_t offset[4] = {0,this->fSliceNumber,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],1,this->ds_dims[2],this->ds_dims[3]}; /* data slice dimensions */
        H5::DataSpace memspace1 ( 4, dims1 );
        H5::DataSpace filespace1 = this->fDSet->getSpace();
        filespace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );

        // Copy data to Buffer
        // For now we need to do this because of the way we want to save the data.
        // The spec object has the data organized in [Samples][Component].
        // The dataset is [...][Component][Samples].
        KTDEBUG(publog, "Copying Spectrum Dataset (PSD) to Buffer.");
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            KTPowerSpectrum* spec = fsData.GetSpectrum(iC);
            if (spec != NULL) {
                spec->ConvertToPowerSpectralDensity();
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = (*spec)(f);
                    if ((!fFirstSliceHasBeenWritten) & (iC==0))  (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write Buffer to Dataset
        KTDEBUG(publog, "Writing Spectrum Dataset (PSD) to HDF5 file.");
        if (!fFirstSliceHasBeenWritten) {
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, memspace1,filespace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }


    void KTHDF5TypeWriterTransform::WritePowerSpectrumDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WritePowerSpectralDensityDistribution(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolar(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteTimeFrequencyDataPolarPower(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteMultiFSDataPolar(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteMultiFSDataFFTW(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
}


