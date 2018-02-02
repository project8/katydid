Executables 
=============

Three sets of executables are built: "Main," "Profiling," and "Validation."  The former includes the programs used for performing actual analyses.  The latter contains small applications used to verify particular parts of Katydid.

## Main Applications
* **Katydid** -- the main, general-purpose, analysis application.
* **EggScanner** -- prints the header that is extracted from an egg file.
* **RSAMatToEgg** -- converts one or more RSA Mat files into one egg file.
* **Truncate** -- truncates an egg2 file at a given file size.  Do NOT use this on egg3 or RSA Mat files!

## Profiling Applications
These files can be used to profile various aspects of Katydid.  They are optionally built by setting the Katydid_ENABLE_PROFILING flag in CMake.

## Validation Applications
There are a variety of validation applications that can be optionally built by setting the Katydid_ENABLE_TESTING flag in CMake.