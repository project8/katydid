Quick Start Guide
======================

These instructions assume that you've compiled and installed Katydid. The installation prefix will be referred to as ${PREFIX}.  
    * Several example configuration files are included with Katydid; they are installed in the ``${PREFIX}/config `` directory.  
    * Start with the configuration file KatydidPSConfig.yaml in Examples/ConfigFiles/. It will extract a single time slice from an egg file, perform a Fourier Transform, and save a plot of the power spectrum to a ROOT file.  
    * Make a copy of the configuration file in the directory from which you'll run Katydid.
    * Customize the input egg file and the output ROOT file. You can use absolute or relative paths (relative paths are relative to the directory in which Katydid is run).
    * Run Katydid with the following command::     
      > ${PREFIX}/bin/Katydid -c KatydidPSConfig.yaml

    * Open the output ROOT file (e.g. ``> root MyPowerSpectrum.root``). See the ROOT documentation for information about how to use ROT, but just for getting started, one way to look at the data is to give the command ``TBrowser fthdgfn``. (ROOT is object-oriented, so are instantiating a TBrowser and need a name for it, even though you never need it for anything. fthdgfn is arbitrary.) The histogram of the frequency spectrum will be called ``histFSpolar_0_0``.

    * The out-of-the-box KatydidPSConfig.yaml file will read an egg3 file.  If you want to read an RSA mat file, do the following:
        - At line 43 change ``"egg3"`` to ``"rsamat"`` to tell the egg processor to use the proper file reader.
        - Since RSA files contain complex time-domain data, you'll need to change the slot used in the FFT processor from ``"fft:ts-real"`` to ``"fft:ts-fftw"`` at line 21.
        - For the same reason, you'll need to change the slot used in the to-power processor from ``"to-ps:fs-polar-to-psd"`` to ``"to-ps:fs-fftw-to-psd"`` at line 25.
    

How did it go for you? Suggestions? Problems? Additions? Please let us know on Slack, especially so we can improve and update the documentation.


