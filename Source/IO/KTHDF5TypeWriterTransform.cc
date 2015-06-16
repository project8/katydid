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

    void KTHDF5TypeWriterTransform::PrepareHDF5File(const std::string& SlotName) {

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


        //////////////////////////////////////////////////////////////////////////////////
        // Define Buffer and Dataset Size
        // 2 Choices to make:  Polar vs Complex ;  FFT Amplitude vs Power

        // Number of Samples in the FFT in each Slice
        // Polar FFT -> Each row is fSliceSize/2 + 1 long.
        // Complex FFT -> Each row is fSliceSize long.
        // PS and PSD spectrum -> Power is only calculated for positive frequencies and DC. The buffer is therefore the same shape as the polar power spectrum.
        if ( SlotName.compare("fs-polar")==0 | SlotName.compare("fs-polar-phase")==0 | SlotName.compare("fs-polar-power")==0  | SlotName.compare("ps")==0  | SlotName.compare("psd")==0 )
            this->fFFTSize = (this->fSliceSize >> 1) + 1;
        if ( SlotName.compare("fs-fftw")==0 | SlotName.compare("fs-fftw-phase")==0 | SlotName.compare("fs-fftw-power")==0 )
            this->fFFTSize = this->fSliceSize;

        // Power spectra have only magnitude (real) components.  Complex FFT has real and imaginary component, and Polar FFT has Magnitude and Angle
        if ( SlotName.compare("fs-fftw-power")==0 | SlotName.compare("fs-polar-power")==0 )
                this->fNComponents = 1;
        if ( SlotName.compare("fs-fftw")==0 | SlotName.compare("fs-polar")==0)
                this->fNComponents = 2;


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

        // Create Names for Groups and Datasets to be written to HDF5 file
        std::string FreqArray_name ("FreqArray");
        std::string spectrum_name ("");
        if ( SlotName.compare("fs-fftw")==0 ) spectrum_name = "complexFS";
        if ( SlotName.compare("fs-polar")==0 ) spectrum_name = "polarFS";

        KTDEBUG(publog, "Creating Spectrum Dataset in HDF5 file...");
        if(this->fFFTGroup == NULL)       this->fFFTGroup = new H5::Group(this->fSpectraGroup->createGroup("/spectra/spectrum"));
        if(this->fDSet_FreqArray == NULL) this->fDSet_FreqArray = this->CreateDSet(FreqArray_name, this->fFFTGroup, *(this->fFFTFreqArrayDataSpace));
        if(this->fDSet == NULL)           this->fDSet = this->CreateDSet(spectrum_name, this->fFFTGroup, *(this->fFFTDataSpace));

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
            hsize_t ChunkSize[4] = {this->fNChannels,1,this->fNComponents,this->fFFTSize};
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
        const std::string SlotName ("fs-fftw");

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
            this->PrepareHDF5File(SlotName);

        }
        else {
            return;
        }

        // Get Slice Number
        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        KTDEBUG(publog, "Writing Slice " << sliceN);

        KTDEBUG(publog, "Copying Spectrum Dataset (Complex FFT) to Buffer.");
        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nChannels = fsData.GetNComponents();
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fFFTBuffer)[iC][0][0][f] = spec[0].GetReal(f);
                    (*this->fFFTBuffer)[iC][0][1][f] = spec[0].GetImag(f);
                    if ( (!fFirstSliceHasBeenWritten) & (iC==0)) (*this->fFFTFreqArrayBuffer)[f] = spec[0].GetBinCenter(f);
                }
            }
        }

        // Write data to HDF5 file
        KTDEBUG(publog, "Writing Spectrum Dataset (Complex FFT) to HDF5 file.");
        if ( !fFirstSliceHasBeenWritten ) {
            // Write Frequency Array
            this->fDSet_FreqArray->write(this->fFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
            SetFirstSliceHasBeenWritten(true);
        }
        // If sliceN is greater than the number of Slices per Record, then extend the dataset
        // This probably happens because there is more than 1 record in the Egg file being processed
        if (sliceN>=this->ds_dims[1])
        {
            this->ds_dims[1] = this->ds_dims[1] + this->fNumberOfSlices;
            this->fDSet->extend( this->ds_dims );
        }

        // Select hyperslabs to save the data
        hsize_t offset[4] = {0,sliceN,0,0};
        hsize_t dims1[4] = { this->ds_dims[0],
                             1,
                             this->ds_dims[2],
                             this->ds_dims[3]};            /* data dimensions */
        H5::DataSpace mspace1 ( 4, dims1 );
        H5::DataSpace fspace1 = this->fDSet->getSpace();
        fspace1.selectHyperslab( H5S_SELECT_SET, dims1, offset );
        // Write spectrum
        this->fDSet->write(this->fFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE, mspace1,fspace1);
        KTDEBUG(publog, "Done Writing Slice to File.");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolar(KTDataPtr data) {
        KTDEBUG(publog, "DISABLED TEMPORARILY - 2015-06-09 - Luiz");
        /*

        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        // Get Slice Number
        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();


        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;
        name_builder << "FSpolar_" << sliceN;
        name_builder >> spectrum_name;

        KTDEBUG(publog, "Creating Frequency Array...");
        std::string FreqArray_name;
        std::stringstream FreqArray_name_builder;
        FreqArray_name_builder << "FreqArray_" << sliceN;
        FreqArray_name_builder >> FreqArray_name;
        // END First Slice -> Create Frequency Array Name

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nChannels = fsData.GetnChannelsonents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Polar FFT data to HDF5 file.");
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    (*this->fPolarFFTBuffer)[iC][f] = spec[0].GetAbs(f);
                    (*this->fPolarFFTBuffer)[iC+1][f] = spec[0].GetArg(f);
                    if ( !fFirstSliceHasBeenWritten ) (*this->fPolarFFTFreqArrayBuffer)[iC][f] = spec[0].GetBinCenter(f);
                }
                
            }
        }
        if ( !fFirstSliceHasBeenWritten ) {
          // If First Slice -> Create and Write the Frequency Array
          H5::DataSet* dset_FreqArray = this->CreatePolarFreqArrayDSet(FreqArray_name);
          dset_FreqArray->write(this->fPolarFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
          SetFirstSliceHasBeenWritten(true);
        }
        H5::DataSet* dset = this->CreatePolarFFTDSet(spectrum_name);
        dset->write(this->fPolarFFTBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");

        */
    }
  



    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data) {
        KTDEBUG(publog, "NOT IMPLEMENTED");
    }

    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataPolarPower(KTDataPtr data) {
        KTDEBUG(publog, "DISABLED TEMPORARILY - 2015-06-09 - Luiz");
        /*
        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating polar power spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "polarPS_" << sliceN;
        name_builder >> spectrum_name;

        // START First Slice -> Create Frequency Array Name
        KTDEBUG(publog, "Creating Frequency Array...");
        std::string FreqArray_name;
        std::stringstream FreqArray_name_builder;
        FreqArray_name_builder << "FreqArray_" << sliceN;
        FreqArray_name_builder >> FreqArray_name;
        // END First Slice -> Create Frequency Array Name

        KTFrequencySpectrumDataPolar& fsData = data->Of<KTFrequencySpectrumDataPolar>();
        unsigned nChannels = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Polar PS data to HDF5 file.");
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumPolar* spec = fsData.GetSpectrumPolar(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    double mag = sqrt(pow(spec[0].GetAbs(f), 2.0) 
                                      + pow(spec[0].GetArg(f),2.0));
                    (*this->fPolarPwrBuffer)[iC][f] = mag; 
                    if ( !fFirstSliceHasBeenWritten ) (*this->fPolarFFTFreqArrayBuffer)[iC][f] = spec[0].GetBinCenter(f);
                }
            }
        }
        if ( !fFirstSliceHasBeenWritten ) {
          // If First Slice -> Create and Write the Frequency Array
          H5::DataSet* dset_FreqArray = this->CreatePolarFreqArrayDSet(FreqArray_name);
          dset_FreqArray->write(this->fPolarFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
          SetFirstSliceHasBeenWritten(true);
        }
        H5::DataSet* dset = this->CreatePolarPowerDSet(spectrum_name);
        dset->write(this->fPolarPwrBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
        */
    }
    void KTHDF5TypeWriterTransform::WriteFrequencySpectrumDataFFTWPower(KTDataPtr data) {
        KTDEBUG(publog, "DISABLED TEMPORARILY - 2015-06-09 - Luiz");
        /*

        if (!data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "complexPS_" << sliceN;
        name_builder >> spectrum_name;

        // START First Slice -> Create Frequency Array Name
        KTDEBUG(publog, "Creating Frequency Array...");
        std::string FreqArray_name;
        std::stringstream FreqArray_name_builder;
        FreqArray_name_builder << "FreqArray_" << sliceN;
        FreqArray_name_builder >> FreqArray_name;
        // END First Slice -> Create Frequency Array Name

        KTFrequencySpectrumDataFFTW& fsData = data->Of<KTFrequencySpectrumDataFFTW>();
        unsigned nChannels = fsData.GetNComponents();

        if( !fWriter->OpenAndVerifyFile() ) return;

        KTDEBUG(publog, "Writing Complex FFT data to HDF5 file.");
        for (unsigned iC = 0; iC < nChannels; iC++) {
            const KTFrequencySpectrumFFTW* spec = fsData.GetSpectrumFFTW(iC);
            if (spec != NULL) {
                for(int f=0; f < spec[0].size(); f++) {
                    double mag = sqrt(pow(spec[0].GetReal(f),2.0) + pow(spec[0].GetImag(f),2.0));
                    (*this->fCmplxPwrBuffer)[iC][f] = mag;
                    if ( !fFirstSliceHasBeenWritten ) (*this->fCmplxFFTFreqArrayBuffer)[iC][f] = spec[0].GetBinCenter(f);
                }
                
            }
        }
        if ( !fFirstSliceHasBeenWritten ) {
          // If First Slice -> Create and Write the Frequency Array
          H5::DataSet* dset_FreqArray = this->CreateFFTWFreqArrayDSet(FreqArray_name);
          dset_FreqArray->write(this->fCmplxFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
          SetFirstSliceHasBeenWritten(true);
        }
        H5::DataSet* dset = this->CreateComplexPowerDSet(spectrum_name);
        dset->write(this->fCmplxPwrBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        KTDEBUG(publog, "Done.");
        */
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
        KTDEBUG(publog, "DISABLED TEMPORARILY - 2015-06-09 - Luiz");
        /*
        if (! data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "PS_" << sliceN;
        name_builder >> spectrum_name;

        // START First Slice -> Create Frequency Array Name
        KTDEBUG(publog, "Creating Frequency Array...");
        std::string FreqArray_name;
        std::stringstream FreqArray_name_builder;
        FreqArray_name_builder << "FreqArray_" << sliceN;
        FreqArray_name_builder >> FreqArray_name;
        // END First Slice -> Create Frequency Array Name

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nChannels = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nChannels; iC++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iC);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectrum();
                for(int i=0; i < this->fPwrSpecSize; i++) {
                    double val = (*spectrum)(i);
                    (*this->fPwrSpecBuffer)[iC][i] = val;
                    if ( !fFirstSliceHasBeenWritten ) (*this->fPolarFFTFreqArrayBuffer)[iC][i] = spectrum->GetBinCenter(i);
                }
            }
        }
        if ( !fFirstSliceHasBeenWritten ) {
          // If First Slice -> Create and Write the Frequency Array
          H5::DataSet* dset_FreqArray = this->CreatePolarFreqArrayDSet(FreqArray_name);
          dset_FreqArray->write(this->fPolarFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
          SetFirstSliceHasBeenWritten(true);
        }
        H5::DataSet* dset = this->CreatePowerSpecDSet(spectrum_name);
        dset->write(this->fPwrSpecBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        return;
        */
    }
    void KTHDF5TypeWriterTransform::WritePowerSpectralDensity(KTDataPtr data) {
        KTDEBUG(publog, "DISABLED TEMPORARILY - 2015-06-09 - Luiz");
        /*

        if (! data) return;

        if ( fWriter->DidParseHeader() ) {
            this->ProcessEggHeader();
        }
        else {
            return;
        }

        KTDEBUG(publog, "Creating spectrum and dataset...");
        std::string spectrum_name;
        std::stringstream name_builder;

        uint64_t sliceN = data->Of<KTSliceHeader>().GetSliceNumber();
        name_builder << "PSD_" << sliceN;
        name_builder >> spectrum_name;

        // START First Slice -> Create Frequency Array Name
        KTDEBUG(publog, "Creating Frequency Array...");
        std::string FreqArray_name;
        std::stringstream FreqArray_name_builder;
        FreqArray_name_builder << "FreqArray_" << sliceN;
        FreqArray_name_builder >> FreqArray_name;
        // END First Slice -> Create Frequency Array Name

        KTPowerSpectrumData& fsData = data->Of<KTPowerSpectrumData>();
        unsigned nChannels = fsData.GetNComponents();

        if (! fWriter->OpenAndVerifyFile()) return;

        for (unsigned iC=0; iC<nChannels; iC++)
        {
            KTPowerSpectrum* spectrum = fsData.GetSpectrum(iC);
            if (spectrum != NULL)
            {
                spectrum->ConvertToPowerSpectralDensity();
                for(int i=0; i < this->fPwrSpecSize; i++) {
                    double val = (*spectrum)(i);
                    (*this->fPSDBuffer)[iC][i] = val;
                    if ( !fFirstSliceHasBeenWritten ) (*this->fPolarFFTFreqArrayBuffer)[iC][i] = spectrum->GetBinCenter(i);
                }
            }
        }
        if ( !fFirstSliceHasBeenWritten ) {
          // If First Slice -> Create and Write the Frequency Array
          H5::DataSet* dset_FreqArray = this->CreatePolarFreqArrayDSet(FreqArray_name);
          dset_FreqArray->write(this->fPolarFFTFreqArrayBuffer->data(), H5::PredType::NATIVE_DOUBLE);
          SetFirstSliceHasBeenWritten(true);
        }
        H5::DataSet* dset = this->CreatePSDDSet(spectrum_name);
        dset->write(this->fPSDBuffer->data(), H5::PredType::NATIVE_DOUBLE);
        return;

    */
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


