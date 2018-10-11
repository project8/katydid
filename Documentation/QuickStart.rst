Quick Start Guide
======================

These instructions assume that you've compiled and installed Katydid. The installation prefix will be referred to as ${PREFIX}.  
    * Several example configuration files are included with Katydid; they are installed in the ``${PREFIX}/config`` directory.  
    * Start with the configuration file KatydidPSConfig.yaml in Examples/ConfigFiles/. It will extract a single time slice from an egg file, perform a Fourier Transform, and save a plot of the power spectrum to a ROOT file.  
    * Make a copy of the configuration file in the directory from which you'll run Katydid.
    * Customize the input egg file and the output ROOT file. You can use absolute or relative paths (relative paths are relative to the directory in which Katydid is run).
    * Run Katydid with the following command::
      > ${PREFIX}/bin/Katydid -c KatydidPSConfig.yaml

    * Open the output ROOT file (e.g. ``> root MyPowerSpectrum.root``). See the ROOT documentation for information about how to use ROOT, but just for getting started, one way to look at the data is to give the command ``TBrowser fthdgfn``. (ROOT is object-oriented, so are instantiating a TBrowser and need a name for it, even though you never need it for anything. fthdgfn is arbitrary.) The histogram of the frequency spectrum will be called ``histFSpolar_0_0``.

    * The out-of-the-box KatydidPSConfig.yaml file will read an egg3 file.  If you want to read an RSA mat file, do the following:
        - At line 43 change ``"egg3"`` to ``"rsamat"`` to tell the egg processor to use the proper file reader.
        - Since RSA files contain complex time-domain data, you'll need to change the slot used in the FFT processor from ``"fft:ts-real"`` to ``"fft:ts-fftw"`` at line 21.
        - For the same reason, you'll need to change the slot used in the to-power processor from ``"to-ps:fs-polar-to-psd"`` to ``"to-ps:fs-fftw-to-psd"`` at line 25.
    
    
    * If you're fine with the one slice example, we can go on to generate a raw spectrogram which contains many time slices.
    	* The configuration file for this purpose is SpectrogramConfig.yaml in Examples/ConfigFiles. 
    	* The parameter that can be played with are under the ``egg`` and ``writer`` configuration categories
    	* We first look at the options available under ``egg``.
    	* ``filename:`` specifies the input file you want to run on. This can also be specified with ``-e`` flag followed by the input file when running katydid.
    	* ``egg-reader:`` specifies which type of input file to read, ``egg3`` or ``rsamat``.
    	* ``slice-size:`` specifies how many data points in time series go into one fft time slice. For example, ``4096`` means each time slice in the raw spectrogram are from fft of 4096 points in the original time series data.
    	* Now we move onto the options under ``writer``.
    	* ``output-file`` specifies the ROOT file you want to put the output in. This can also be specified with ``--rtw-file`` flag when running katydid.
    	* The definitions of other parameters can be found in Source/IO/ROOTSpectrogramWriter/KTROOTSpectrogramWriter.hh
    * Believe it or not, checking the raw spectrogram is useful from time to time. However, in other cases for Katydid users, they would use a configuration file for reconstructing tracks and events. The way of running katydid with a track reconstruction configuration file is not substantially different from running the raw spectrogram configuration.
    	* The main difference is in the output ROOT file. In raw spectrogram generation, the output ROOT file has a 2D histogram recording the raw spectrogram in it. With a track and event reconstructing configuration the output file contains a tree structure recording event and track information and sometimes sparse spectrogram.
    * After running the spectrogram configurations we get a sense of how katydid make spectrograms, but in many cases the tracks are what people are interested in. Here, we give a basic example on katydid reconstructing tracks.
    	* First, look for TrackConstructionConfig.yaml in Examples/ConfigFiles folder.
    	* Suppose an egg file is ready in hand, run
    		``${PREFIX}/bin/Katydid -c <path/to/TrackConstructionConfig.yaml> -e <path/to/eggfile> --rtw-file tracks.root``
    	* It would produce a tracks.root file in the present directory. It would contain the tree procTracks with various track properties information and tree DiscretePoints1D with information of discrete points where the tracks are built from.

    

How did it go for you? Suggestions? Problems? Additions? Please let us know on Slack, especially so we can improve and update the documentation.


