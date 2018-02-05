User Interface
=============



For programs that use the "official" application interface (e.g. via the Katydid application) a user sets options in a program using either a configuration file, or command-line options.  Both of these mechanisms allow the user to change the parameters of a program without recompiling any code.

Before getting to the configuration files or command-line options, there are three important built-in command line options that all users should be aware of:
* **-h [--help]** -- Print the help message for the executable and exit.
* **-v [--version]** -- Print the version information for the executable and exit.
* **-c [--config] /path/to/config/file.json** -- Specifies the path to the configuration file that should be used.

## Configuration Files
Configuration files are broken up into two main sections:
* Configuring the processor toolbox, including defining which processors will be used, how they are connected, and which processor(s) is in charge of the run,
* Configuring each individual processor.

Here is an example of an annotated JSON-formatted configuration file::


json
{
    "processor-toolbox":
    {
        "processors":
        [
            { "type": "egg-processor",       "name": "egg" },
            { "type": "forward-fftw",        "name": "fft" },
            { "type": "convert-to-power",    "name": "to-ps" },
            { "type": "basic-root-writer",   "name": "writer" }
        ],::

        "connections":
        [
        {
            "signal": "egg:header",
            "slot": "fft:header"
        },::
                    
        {
            "signal": "egg:ts",
            "slot": "fft:ts-fftw"
        },
        {
            "signal": "fft:fft",
                "slot": "to-ps:fs-fftw-to-psd"
            },
            {
                "signal": "to-ps:psd",
            "slot": "writer:ps"
        }
        ],::
                
        "run-queue":
        [
            "egg"
        ]
    },::
    
    "egg":
    {
        "filename": "/path/to/file.egg",
        "egg-reader": "egg3",
        "slice-size": 16384,
        "number-of-slices": 1
    },::
        
    "fft":
    {
        "transform-flag": "ESTIMATE"
    },::
        
    "writer":
    {
        "output-file": "/path/to/file.root",
        "file-flag": "recreate"
    }
}



Command-Line Options
----------------- 
The built-in options are discussed above.  Those are available for any program using Katydid's application interface.

Other command-line options can be added by various parts of the code to provide short-cuts for setting certain parameters.  Generally there are also configuration-file options available that can set the same parameters; Command-line options always take precedence over settings from a configuration file.  The command-line options available for an application can be found using the -h (--help) command-line option.

Modifying Configuration-File Parameters from the Command Line
---------
Even if an application or a class does not specify a command-line option for modifying a certain parameter, it is still possible to set that parameter from the command line, or add parameters to the configuration.  The entire nested address of the parameter is used as the command-line-option name.  For example, to change the FFTW transform flag in the above configuration file, one would use `--fft.transform-flag="PATIENT"`.  To get all of the options, build-in and configuration-file-based, you can run `Katydid --help-config -c my_config.json` .