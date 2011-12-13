#ifndef KTCORE_MANAGER_H
#define KTCORE_MANAGER_H
/**
 * @file

 * @brief contains KTCoreManager

 <b>Revision History:</b>
 \verbatim
 Date         Name          Brief description
 -----------------------------------------------
 29 Mar 2010   N. Oblath     First version
 13 Apr 2010   N. Oblath     Made a singleton; added parameter status map
 14 Jul 2010   D. Furse      Major changes in preparation for radon run
 07 Sep 2010   M. Babutzka   Added command line variables (fargc, fargv) with getters and setters
 18 Jan 2011   N. Oblath     Separated KTCoreManagerSimulation from KTCoreManager
 \endverbatim

 *
 */

/*!
 * @class Katydid::KTCoreManager
 @author N. Oblath

 @brief Base class for the primary manager for applications in Katydid

 @details
 <b>Detailed Description:</b><br>
 Classes derived from KTCoreManager (e.g. KTCoreManagerSimulation) perform the actions specific to each application.  See KTCoreManagerSimulation for notes on how to make a derived class of KTCoreManager for an application.

 */

#include "KTManager.hh"
#include "KTCommandLineUser.hh"

#include "KTCoreMessage.hh"

#include <vector>
#include <deque>
#include <map>

namespace Katydid
{
    class KTCoreManagerFactory;
    class KTCoreManagerTable;

    class KTCoreManager :
        public KTManager,
        public KTCommandLineUser
    {
        public:
            friend class KTCoreManagerFactory;
            friend class KTCoreManagerTable;

        protected:
            KTCoreManager(Int_t aNArgs = 0, Char_t** anArgList = 0);
            KTCoreManager();
            virtual ~KTCoreManager();

            //*************
            // Application interfaces (usually not overridden)

        public:
            //! Responsible for running Setup() and Prepare()
            virtual void SetupApplication();
            //! Responsible for running Execute()
            virtual void RunApplication();
            //! Responsible for running Shutdown()
            virtual void ShutdownApplication();

            //*************
            //Top-level control functions (usually not overridden or used; these are called by the Application methods)

        public:
            // From the Toolboxes/Managers
            //! Establishes the manager hierarchy
            virtual Bool_t Setup();
            //! Reads configuration files
            virtual Bool_t Prepare();
            //! Shutdown manager hierarchy
            virtual Bool_t Shutdown();
            //! Abort the core manager
            virtual Bool_t Abort();

            //! Starts the application (must be overridden)
            virtual Bool_t Execute() = 0;

            //*************
            //CoreManager-specific control functions

        protected:
            //! Setup for the CoreManager
            virtual Bool_t SetupManager() = 0;
            //! Prepare for the CoreManager
            virtual Bool_t PrepareManager() = 0;
            //! Shutdown for the CoreManager
            virtual Bool_t ShutdownManager() = 0;

            //*************
            //Recursive shutdown functions (usually not overridden)

        protected:
            ///sets up this manager's dependencies
            virtual Bool_t SetupDependentManagers();
            ///protected method for starting the shutdown process for this core manager's manager aspect and its dependencies
            virtual Bool_t RecursiveManagerShutdown();
            ///shuts down this core manager's manager depencencies
            virtual Bool_t ShutdownDependentManagers();

            //*************
            // Setup the management dependencies (should not be overridden)

        public:
            virtual void AddManager(const string& aType);

            /// You might as well use the version without the bool, because it's not used for CoreManagers
            virtual void AddManager(const string& aType, Bool_t /*b*/);


            //*************
            // Identification

        public:
            const string& GetApplicationType() const;
            const string& GetApplicationString() const;

        protected:
            string fApplicationType;
            string fApplicationString;

            //*************
            // Command line options

        public:
            virtual void AddCommandLineOptions();
            virtual void UseParsedCommandLineImmediately();

            virtual void AddDerivedCommandLineOptions() = 0;
            virtual void UseDerivedParsedCommandLineImmediately() = 0;

    };

    inline const string& KTCoreManager::GetApplicationType() const
    {
        return fApplicationType;
    }
    inline const string& KTCoreManager::GetApplicationString() const
    {
        return fApplicationString;
    }


} // end Katydid namespace

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 \mainpage Katydid

 \image html Cassiopeia.png

 \section KTContent Content

 <ol>
 <li>\subpage KTFirstSteps "Getting Started"
 <li>\subpage KTInstallationpage "Installation"
 <li>\subpage KTGenStruct "General Structure"
 <li>\subpage KTRunning "Running a Simulation"
 <li>\subpage KTOutput "Output"
 <li>\subpage KTGlossary "Glossary"
 </ol>

 \attention The source of this documentation can be found in: /KTCore/Main/include/KTCoreManger.hh

 \page KTFirstSteps Getting Started


 \section Version Latest Version: 1.20.00
 <hr>


 \section Requirements System Requirements
 <hr>
 Linux/MacOS with a reasonably new c++ compiler:
 \li Several versions of gcc 4.x have been tested and compile; gcc 3.2 does not work.
 \li Windows+cygwin has not been tested, and is therefore not supported.

 Dependencies:
 \li ROOT (http://www.cern.ch/root) version 5.24 or higher
 \li GSL (http://www.gnu.org/software/gsl) 1.0 or higher
 \li boost (http://www.boost.org) version 1.34 or higher


 \section GettingKT Getting Katydid
 <hr>
 You have two options for downloading any distribution of Katydid:

 <ul>
 <li><b>Distribution Tarball</b><br>
 For every Katydid release there will be a tarball of the full distribution.
 This can currently be downloaded from the Muenster SVN repository (a better location will be found in the future).
 The suggested method for doing this is via the web interface: https://nuserv.uni-muenster.de/viewvc/Katydid/tar .
 From there you can download the file <tt>kassiopeia-X.YY.ZZ.tar.gz</tt>, and place it in the directory in which you want to unpack it.
 \code
 >  tar -xzvf kassiopeia-X.YY.ZZ
 >  cd kassiopeia-X.YY.ZZ
 \endcode
 This is the simplest method for obtaining a copy of \textsc{Katydid}.
 It is also independent of the Autotools suite, so it is recommended for installing on older operating systems
 or systems which do not have the required versions of autoconf, automake, or libtool (see below).
 </li>

 <li> <b>SVN Check-Out</b><br>
 Katydid source can be found on the KATRIN SVN repository at the University of Muenster
 (https://nuserv.uni-muenster.de/svn/katrin/).  You must have an account to install Katydid in this way,
 you must have the following versions of the Autotools applications:
 <ul>
 <li>autoconf v2.65 or higher, earlier versions may work, but we make no promises</li>
 <li>automake v.1.10, recommended 1.11 or higher.</li>
 <li>libtool v. 2.2, earlier versions may work, but again, no promises.</li>
 </ul>

 The distribution can be obtained from the SVN repository in the standard way:
 \code
 >  svn co https://nuserv.uni-muenster.de/svn/katrin/Katydid/tags/kassiopeia-X.YY.ZZ
 >  cd kassiopeia-X.YY.ZZ
 \endcode
 </li>


 \section Documentation Documentation
 <hr>
 <ol>
 <li>
 Wiki -- There are instructions on the installation and use of Katydid on the KATRIN Wiki:
 https://wiki.fzk.de/katrin/index.php/Katydid
 The standard KATRIN login is required.
 </li>
 <li>
 Reference Manual -- You're looking at it.  Code documentation is available through the doxygen system.
 In the <tt><maindir>/Doc</tt> directory, run the following to generate the HTML documentation:
 \code
 > makeDoc Reference
 \endcode
 Open <tt><maindir>/Doc/index.hhtml</tt> and click on "Reference Manual" to get to the main page.
 </li>
 <li>
 3. User Manual -- A PDF-format manual is available in the Doc directory: <tt><maindir>/Doc/User/KTGuide.pdf</tt>.
 It is also built by running the following in the </tt><maindir>/Doc</tt> directory:
 \code
 > makeDoc User
 \endcode
 The User Manual includes detailed information about the various components of Katydid.
 </li>
 </ol>

 \section Help Getting Help
 <hr>
 Report bugs and problems on <https://savannah.fzk.de/projects/katrin/>.
 See the KATRIN Wiki for registration instructions (https://wiki.fzk.de/katrin/index.php/SoftwareADMIN)

 Primary email contacts:
 <ul>
 <li>KATRIN Simulation Development List: <tt>katrin-simulation-dev-at-lists.kit.edu</tt></li>
 <li>Noah Oblath: <tt>nsoblath-at-mit.edu</tt></li>
 </ul>





 \page KTInstallationpage Installation


 \section Configuration Configuring the Installation
 <hr>
 These commands are run from <tt><maindir></tt>.

 If you used the <b>SVN Check-Out</b> method (not using the tarball) to obtain Katydid, then the <tt>configure</tt> script and Makefiles
 still need to be created via the Autotools suite.  The <tt>autogen.sh</tt> script will take care of this, as well as running
 the <tt>configure</tt> script.  From the command line, run:
 \code
 >  ./autogen.sh
 \endcode

 For those who used the <b>Distribution Tarball</b> method to obtain Katydid, the <tt>configure</tt> script already exists.  Simply run it from the command line:
 \code
 >  ./configure
 \endcode

 The rest of these instructions apply to <b>both</b> methods of obtaining Katydid, except as noted.

 See below for further installation options.

 \subsection{Compiling and Installing}

 From <tt><maindir></tt>, type the following two commands to, respectively, compile and install Katydid:
 \code
 >  make
 >  make install
 \endcode
 If everything compiles and installs correctly, Katydid is ready to use.


 \section ConfigurOptions Important Installation Options
 <hr>
 If you would like to see the available installation options, type
 \code
 >  ./configure --help
 \endcode

 Since the screen output is quite vervose, here are the highlights:
 To install Katydid in a specific directory, use
 \code
 > ./configure --prefix=/some/path/to/some/directory
 \endcode
 The default value for Katydid is '<tt>pwd</tt>', (NOT <tt>/usr/</tt>).

 You can also independently switch the installation location for binaries, headers, libraries, datafiles, and configuration files:
 \code
 > ./configure --bindir=/some/path/to/some/directory/bin
 > ./configure --includedir=/some/path/to/some/directory/include
 > ./configure --libdir=/some/path/to/some/directory/lib
 > ./configure --datadir=/some/path/to/some/directory/data
 > ./configure --sysconfdir=/some/path/to/some/directory/etc
 \endcode

 You can select the ROOT, boost, and GSL versions to link against:
 \code
 --with-rootsys          top of the ROOT installation directory
 --with-boost=DIR        prefix of Boost 1.34 [guess]
 --with-gsl-prefix=PFX   Prefix where GSL is installed (optional)
 \endcode

 <!--
 \section KTEnvironmentVar Important environment variables
 <hr>
 In order to have a system wide installation, you might to modify the following environment variables,
 Assuming you have installed Katydid in
 \code
 /some/path/kassiopeia/latest
 \endcode
 please put the following in your .bashrc.

 \code
 #to execute kassiopeia from any directory
 export PATH=/some/path/kassiopeia/latest/bin:$PATH

 #to link against kassiopeia using pkg-config
 export PKG_CONFIG_PATH=/some/path/kassiopeia/lastest/lib/pkgconfig:$PKG_CONFIG_PATH

 #for root to find the rootmap file and the kassiopeia libraries, so that root can dynamically load them...
 export LD_LIBRARY_PATH=/some/path/kassiopeia/latest/lib:$LD_LIBRARY_PATH

 #kassiopeia specific environment variables (optional), see \ref{KTEnvVariables}
 export KTCONFIG=/some/path/kassiopeia/latest/etc
 export KTDATA=/some/path/kassiopeia/latest/Data

 \endcode

 Non bash users are hopefully smart enough to translate that that into c-shell syntax themselves.
 Simply use the following syntac instead:
 \code
 setenv KTCONFIG /some/path/kassiopeia/latest/Data
 \endcode
 -->




 \page KTGenStruct General Structure

 \section KTDirectories Directory Structure

 <hr>
 After compilation and installation, you will find the following directories on the top level of the Katydid folder.
 Directories marked with a (*) are created during the installation and do not exist until you have completed that.
 Also they can be created in a different place using configure--prefix or redirected separately using the configure options in the list below.
 Directories marked with a (!) contain source code:

 <ul>
 <li><b>ac_macros</b>: The location of scripts used by Autoconf. These can usually be ignored.</li>
 <li><b>Applications</b> (!): Source directory for executables.
 Its subdirectories include Main (the primary executables), Examples, Scripts, and Validation.</li>
 <li><b>autom4te.cache</b> (*): This directory is part of the Automake process and can be ignored.</li>
 <li><b>bin</b> (*): Installation directory for executables and scripts. can be changed with configure --bindir
 The most relevant executables for users are Katydid and FileReader.</li>
 <li><b>Data</b> (*): Installation directory for Katydids data files (geometry, cross sections, etc.).can be changed with configure --datadir</li>
 <li><b>Doc</b>: The documentation folder. Run doxygen Doxyfile here to create html documentation.</li>
 <li><b>etc</b> (*): Installation directory for the default configuration files. can be changed with configure --sysconfdir
 Local versions of these files can be used to control Katydid.
 The files here are overwritten with every make install, so all changes to them will be lost!</li>
 <li><b>include</b> (*!): Installation directory for all Katydid headers. can be changed with configure--includedir</li>
 <li><b>KTCore</b> (!): Source directory for Katydid's core modules.</li>
 <li><b>KXML</b> (!): Source directory for a future KXML interface.  It is currently not in use.</li>
 <li><b>lib</b> (*): Installation directory for Katydids libraries. can be changed with configure --libdir</li>
 <li><b>Modules</b> (!): Source directory for the various sub-modules of Katydid.</li>
 </ul>

 The KTCore directory contains the following subdirectories:
 <ul>
 <li><b>Base</b>: contains the base classes which are needed to run a simulation, like toolboxes and rootprocesses.</li>
 <li><b>Configuration</b>: contains the code responsible for building the configuration of the simulation out of the tools provided in the toolbox and the builders fore the core classes.</li>
 <li><b>Geometry</b>: the simulation geometry is read in and managed here...</li>
 <li><b>DataStructure</b>: classes responsible for reading and writng. this directory has no dependency on the rest of the program and will be moved to Kali at some point.</li>
 <li><b>Main</b>: general organizational classes, like the coremanager, a general command class, the logging class and the general base classes for reading the input file.</li>
 <li><b>Math</b>: a couple of mathematical classes, e.g. solver for differntial equations</li>
 <li><b>Utility</b>: a few generally useful things like templates or a header with numerical constants.</li>
 </ul>
 The Modules directory contains the following subdirectories:
 <ul>
 <li><b>DAQ</b>: the daq simulation</li>
 <li><b>FieldCalculators</b>: electric and magnetic field calculation code</li>
 <li><b>Generator</b>: generation mechanisms </li>
 <li><b>SSC</b>: WGTS simulation</li>
 <li><b>Transport</b>: particle transport classes</li>
 </ul>


 \section KTConfig Configuration Files
 <hr>
 Katydid is controlled by a series of XML-like configuration files.
 In a future release the current configuration files will be replaced by KXML files.  These will be similar to, but not exactly the same as, the files currently in use.

 All of the configuration files are installed in the <maindir>/etc directory.
 The files in this directory are overwritten during installation, so it's suggested that you make local copies of the files you customize and use.
 <ul>
 <li><b>DAQConfiguration.txt</b>: DAQ processors</li>
 <li><b>FieldConfiguration.txt</b>: Electromagnetic field calculation methods</li>
 <li><b>GeneratorConfiguration.txt</b>: Particle generators</li>
 <li><b>GeometryConfiguration.txt</b>: Basic simulation geometry</li>
 <li><b>KatydidConfiguration.txt</b>: Setting up the simulation from the tools available in the toolboxes</li>
 <li><b>SSCConfiguration.txt</b>: WGTS modeling package SSC</li>
 <li><b>StepStrategyConfiguration.txt</b>: Particle transport mechanisms and their exit conditions</li>
 <li><b>UserConfiguration.txt</b>: The primary user interface for the Katydid executable, including parameter replacement variables and local configuraiton filenames</li>
 </ul>

 The configuration files accomplish two purposes: setting up the toolboxes, and setting up the simulation based on the tools
 available in the toolboxes and user input.  The files that accomplish the former purpose are the DAQ, Field, Generator, Geometry, SSC,
 and StepStrategy Configuration files.  The Global, Katydid, and User Configuration files accomplish the latter purpose.

 The syntax of the configuration files is demonstrated in the following dummy code block, it is essentially XML:

 \code
 #this is a comment

 / * (without the space in between)
 and this is
 a block comment
 * / (without the space in between)

 <Element1>                  #The beginning of an element for the Katydid class being configured
 <<Name=name1>>              #Every element with parameters is named first
 <<Parameter1Name=Value1>>   #A basic parameter being set to Value1
 <<Parameter2Name=Value2>>   #Another parameter being set to Value2
 </Element1>                 #The end of the element for the class

 <Element2>
 <<Name=name2>>
 <SubElement>
 <<Name=subname>>
 <<Parameter3Name=Value3>>
 </SubElement>
 </Element2>
 \endcode

 <!--
 <ComplexType> #the begin token for a complex type (typical a Katydid class)
 <<name=foo>># a basic parameter, in this case a string determining the name of this complex type
 <<anotherParameter=bar>> #another basic parameter
 <anInsideComplexType> #a different complex type inside the other one
 <<aParameteroftheInsideComplexType=0>> #a parameter for the inside complex type
 </anInsideComplexType> #the end token for the first complex type
 </ComplexType> #end token

 #sometimes it is possible or even mandatory, to define the inside type first, and give the other complex type a reference to the inside one.
 #So in some cases, it is possible/mandatory to write the above in the following way.
 #In cases, where the two types have to be defined in different configuration files, this scheme has to be used...
 #
 <anInsideComplexType>
 <<name=foobar>> #a parameter for the inside complex type
 <<aParameteroftheInsideComplexType=0>> #a parameter for the inside complex type
 </anInsideComplexType>

 <aComplexType>
 <<name=foo>># a basic parameter, in this case a string determining the name of this complex type
 <<anotherParameter=bar>> #another basic parameter
 <<aReferenenceToAnInsideComplexType=foobar>>
 </aComplexType>
 -->

 Basically, a configuration file is divided into elements (everything between the <element> and </element> tags).
 Elements can be nested, as is shown in the second element in the example.  With only a few exceptions, all elements have
 a Name that needs to be set first, before any parameters.  Those names need to be unique within the entire Katydid parameter space
 (i.e. everything that's in the set of configuration files).  The configuration files follow this example layout,
 and should contain enough examples to fill you in with the details.

 \subsection ConfigFileKeywords Keywords
 <hr>
 Configuration files can include keywords that allow the user to communicate directly with the system that is reading the files.
 The keyword syntax is:
 \code
 @keyword(comma,separated,arguments)
 \endcode

 Available keywords:
 <ul>
 <li><b>include</b>: Configurations can be split up among multiple files, and the files can be included from one another.  This keyword
 takes only one argument, the file name. This allows for more flexibility and reusability in making configuration files.  So, for example,
 perhaps you want to split the GeometryConfiguration into multiple files, each of which describes one region of the system being simulated.
 GeometryMaster.txt brings all of the files used together:
 \code
 @include(GeometryRegion1.txt)
 @include(GeometryRegion2.txt)
 \endcode
 </li>
 </ul>

 \subsection ConfigFileTechDetails Technical Details
 <hr>
 Under the current configuration system (non-KXML), all configuration files are parsed with the KTTokenizer, which works with
 an Initializer and a Builder to correctly construct classes and set parameters.  The files have to be loaded in a particular order to
 properly setup the simulation.  The UserConfiguration file is first, with some preliminary settings, the parameter replacement
 variables, and the local configuration files specified by the user.  Next, the KTCoreManager reads the GlobalConfiguration file,
 which sets various parameters within the KTCoreManager itself.  Third, the hierarchical calling of the Toolbox Prepare() functions
 read in the configuration files that set up each toolbox.  Finally, the KatydidConfiguration file is read by the KTCoreManager,
 selecting various tools from the toolboxes to be used in the simulation, and setting a few final parameters.


 \section KTData The Data Files
 <hr>
 The data files contain information such as cross sections and electrode configurations, which are used by the simulation.
 They are installed in the <maindir>/Data directory.
 There is currently not a standardized format, though most of them are ASCII files.
 Generally these files remain unchanged, though occasionally new files will be added or existing files modified slightly.


 \section KTManagement Management Structure
 <hr>
 The management structure in Katydid has three purposes: to setup and configure all of the components of an application,
 to actually run the application, and to shut it down smoothly.  These tasks are split into two types of management classes.
 The Toolboxes are responsible for containing all of the configured elements of the application, and the Managers are responsible for
 performing the setup, running the application, and shutting it down.

 The entire management structure is overseen by the CoreManager (singleton).  The class KTCoreManager is a base class for application-specific
 derived classes.  For instance, KTCoreManagerSimulation is the CoreManager for the Katydid application (tracking simulations).
 Since this is a singleton class, only one CoreManager can exist at any given time.  Creation of the CoreManager is taken care of through
 the KTManagerFactoryTable.

 KTCoreManager takes care of setting up the management hierarcyy, and reading in the main configuration files.  It initiates the Setup and Prepare
 processes for all Managers and Toolboxes.  The specific derived CoreManager being used is responsible for specifying what the management
 structure will be (by choosing its downstream Managers), and performing the executing the actions taken by the application.  Again using
 KTCoreManagerSimulation as an example, the downstream Managers include KTRunManager, KTGeometryToolbox, KTFieldToolbox, SSCToolbox,
 KTGeneratorToolbox, KTStepStrategyToolbox, and KTDAQToolbox.  The Execute method creates and closes the output file, and runs the
 simulation.

 The KTExceptionManager and the hierarchical structure of KTExceptionHandlers are responsible for writing log and error messages.
 They also cleanly handle situations where the simulation needs to shut down due to any sort of error.

 The DataManager (singleton) is responsible for writing the output data.  See \ref Data for more information on the output data.

 The Management base class, KTManagerBase, provides the basic functionality common to all managers.
 These functions include those necessary to build (Setup()), navigate  (Get[Up/Down]streamManager())
 and shutdown (ShutDown()) the tree-like management structure.
 Furthermore, three other important functions are common to all managers:
 <ul>
 <li>Prepare()</li>
 <li>ProcessCommand()</li>
 <li>Execute()</li>
 </ul>

 Prepare() reads in the module configuration from the default or user-provided configuration file (if applicable),
 and generally gets the Manager or Toolbox ready for use in the simulation.

 ProcessCommand() allows requests for configuration changes between managers to happen dynamically during the simulation
 (e.g. to change the field calculation method when entering another region of the experimental setup).
 ProcessCommand() can deal with different types of inputs, given in form of KTCommands,
 and is used for initialization and for configuration changes during a simulation.

 Execute() causes the manager to perform its responsibilities on the data that is given as an argument.
 Since each manager deals with different types of data, Execute() takes an arbitrary argument in form of a boost::any,
 and returns another arbitrary argument (again in the form of a boost::any*).
 In contrast to KTManagerBase::ProcessCommand, KTManagerBase::Execute should typically only deal with one type of input,
 although that is not a strict requirement (for the experts: boost::anys provide intrinsic type information,
 which is its main advantage over void pointers as a solution for this kind of problem).

 \section KTManagers Toolboxes and Managers
 <hr>
 The Katydid application (tracking simulation) uses the following hierarchy:
 <ul>
 <li>KTCoreManagerSimulation
 <ul>
 <li>KTRunManager
 <ul>
 <li>KTEventManager
 <ul>
 <li>KTTrackManager
 <ul>
 <li>KTStepManager</li>
 </ul>
 </li></ul>
 </li>
 <li>KTDAQEventManager</li>
 </ul>
 <li>KTDAQToolbox</li>
 <li>KTGeometryToolbox</li>
 <li>KTFieldToolbox</li>
 <li>KTSSCToolbox</li>
 <li>KTStepStrategyToolbox</li>
 </ul>
 </ul>



 \page KTRunning Running a Simulation

 The primary executable for Katydid is called, predictably, Katydid.  Along with all of the other executables, it is installed
 in <maindir>/bin.

 \section KTUserInterface User Interface
 <hr>
 The user can change settings in Katydid in four ways:
 <ol>
 <li>The UserConfiguration file</li>
 <li>The command line</li>
 <li>Configuration files</li>
 <li>Environment variables</li>
 </ol>



 \section KTUserConfigInterface The UserConfiguration File
 <hr>
 The UserConfiguration file and the command line (see below) are the primary user interface for Katydid.  They are both responsible for
 the same settings, with settings specified on the command line overwriting anything specified in the UserConfiguration file.

 The settings in the UserConfiguration file fall into three categories: preliminary settings, parameter replacement variables, and
 local configuration files.  The syntax is the same as for the other configuration files.  The syntax for the command line
 will be discussed below.

 \subsection UserConfigPrelim Preliminary Settings
 <hr>
 There are a few settings which need to be established early in the process of setting up Katydid.  These include the verbosity,
 the configuration-file default directory, and the data-file directory.  The verbosity should be set first in the UserConfiguration
 file, so that all subsequent parts of the simulation print to the screen and log/error files at the appropriate level.
 The configuration-file default directory is set early (in the example UserConfiguraiton file it's set after the parameter replacement
 variables) so that it's available before any other configuration files are needed.

 \subsubsection UserConfigDirSet Direct Parameter Settings
 <hr>
 The user can change parameter settings in other configuration files directly from the UserConfiguration file.  This can be done for
 any parameter besides the "Name" of a configuration element.  The user only needs to know the configuration element Name, and the name of
 the parameter that is being set.  The notation is:
 \code
 <DirectSetting>
 <<ParameterLocation1=Value1>>
 <<ParameterLocation2=Value2>>
 ...
 </DirectSetting>
 \endcode
 where the location of a parameter is the simple combination of the configuration element name and the parameter name: ElementName:ParameterName.

 So, for instance, if you wanted to change the EnhancementFactor for scattering processes, where the KTrackScattering element is named
 "Scattering," you would use the following setting:
 \code
 <DirectSetting>
 <<Scattering:EnhancementFactor=1.5e8>>
 </DirectSetting>
 \endcode

 This feature can be used for any configuration element that has a Name, and the Name cannot be altered in this way.
 Unlike with the Parameter Replacement Variables (below), you do not need to modify the other configuration file in any way.

 \subsubsection UserConfigParamRepl Parameter Replacement Variables
 <hr>
 The user can take advantage of the parameter replacement variables to be able to change parameters in different
 configuration files while only editing the UserConfiguration file.  This is accomplished by editing the other configuration
 file once, to put a variable in place of the parameter value, and then defining that variable in the UserConfiguration file.
 After that, the parameter value can be easily changed with the variable definition in the UserConfiguration file.

 For example, say you wanted to be able to change the enhancement factor for scattering processes, as in the above example.  You would open
 your StepStrategyConfiguration file (a local copy of StepStrategyConfiguration.txt) in an editor, and change the line that
 says <<EnhancementFactor=1e8>> to be something like <<EnhancementFactor=${MyEnhancementFactor}>>.  Then, in the UserConfiguration
 file you would add a parameter replacement variable definition:
 \code
 <ParameterReplacement>
 <<MyEnhancementFactor=1.5e8>>
 # <<VARIABLE=VALUE>>   <-- General syntax
 </ParameterReplacement>
 \endcode
 From that point on you are able to change the enhancement factor via the UserConfiguration file.

 Parameter replacement variables can be used on other tokens specified in the UserConfiguration file.  For instance, you might want
 to use a variable in the default configuration-file directory setting (OptionHome).  As long as the variable is defined before
 the parameter is set, this will work.  That can be done by defining the parameter replacements early in the UserConfiguration file,
 or by giving them on the command line.

 \subsection UserConfigLocalFiles Local Configuration Files
 <hr>
 Katydid searches for configuration files with the default names in the run directory and in the default configuration file
 directory.  However, what if you want to change a configuration file name?  Perhaps you want to experiment with the geometry,
 and in the same run directory have the files GeometryConfig_attempt1.txt and GeometryConfig_attempt2.txt.  You can tell
 Katydid which of these to use via the UserConfiguration file:
 \code
 <ConfigFile>
 <<Geometry=./GeometryConfig_attemp1.txt>>
 # <<KEY=FILENAME>>   <-- General syntax
 </ConfigFile>
 \endcode
 The available keys are: Katydid, DAQ, Field, Generator, Geometry, SSC, and StepStrategy.



 \section KTCommandLine The Command Line
 <hr>
 All of the settings in the UserConfiguration file can be specified from the command line.  Anything set on the command line
 takes precedence over settings in the UserConfiguration file or extracted from environment variables.

 The full available syntax is:
 Katydid [UserConfiguraiton file] [-r [Parameter replacement variables]] [-c [Local configuration files]] [-t [Other tokens]]

 <b>[UserConfiguration file]</b>: If you want to specify the UserConfiguration file for your Katydid run, it must be done
 as the first argument.

 <b>[-d [Direct settings]]</b>: You can make direct parameter settings from the command line with this simple syntax:
 \code
 -d PARAMETER1=VALUE1 PARAMETER2=VALUE2 . . .
 \endcode
 where "PARAMETER1" is the combination of the configuration element name and the parameter name: ElementName1:ParameterName.

 <b>[-r [Parameter replacement variables]]</b>: You can define parameter replacement variables from the command line with this
 simple syntax:
 \code
 -r VARIABLE1=VALUE1 VARIABLE2=VALUE2 . . .
 \endcode

 <b>[-c [Local configuration files]]</b>: You can specify local configuration files from the command line with this simple syntax:
 \code
 -c KEY1=FILENAME1 KEY2=FILENAME2 . . .
 \endcode

 <b>[-t [Other tokens]]</b>: Any other tokens can be given from the command line as well.  You can use the -t flag to give
 parameters from the preliminary settings.  You can also specify parameter replacement variables or local config files, but
 the syntax of the -r and -c flags is much simpler.

 The general syntax for setting any tokens with the -t flag in the command line is:
 \code
 -t --BuilderType1 ParameterName1=ParameterValue1 ++ --BuilderType2 --BuilderType3 Parameter2=Value2 ++ Parameter3=Value3 ++
 \endcode
 In configuration-file syntax, this would appear as:
 \code
 <BuilderType1>
 <<ParameterName1=ParameterValue1>>
 </BuilderType1>
 <BuilderType2>
 <BuilderType3>
 <<Parameter2=Value2>>
 </BuilderType3>
 <<Parameter3=Value3>>
 </BuilderType2>
 \endcode
 The points to note about the command line syntax are:
 <ul>
 <li>A builder is started with the --[TYPE] flag</li>
 <li>A parameter is set simply with NAME=VALUE</li>
 <li>The last builder started is ended with ++</li>
 <li>Nesting of builders is supported (though right now there's no reason to use it in the UserConfiguration file)</li>
 </ul>

 As a concrete example, you would set the default configuration file directory from the command line as follows:
 \code
 -t --PreliminaryConfiguration OptionHome=[some path] ++
 \endcode

 <b>NOTE</b>: parameter replacement variables are not allowed in parameters being set from the command line!

 \section KTEnvVariables Environment Variables
 <hr>
 The default configuration-file directory and the data-file directory can be specified using the environment variables KTCONFIG and
 KTDATA, respectively.  Using these variables, however, is optional, as long as these parameters are set in some way.

 A script is provided to conveniently set these variables based on the main directory of your Katydid installation.
 In <maindir>/Applications/Scripts you'll find setenv.sh and setenv.csh (for bash shell and c-shell users, respectively).
 You can source the appropriate script manually, or add "source setenv.sh" or "source setenv.csh" to your shell login script.

 During the Katydid installation process the setenv scripts will be filled in with the appropriate directory path for your
 Katydid installation.  However, before using them the first time you should double-check to make sure the path is correct.

 The directories set with the environment variables will be overwritten by both the UserConfiguration file and the
 command line, if OptionHome or DataHome are specified there.



 \section KTConfigFileInterface Configuration Files
 <hr>
 These files include the complete set of things a user can configure.  There are several ways that a user can use these files to
 configure the simulation:
 <ul>
 <li>The settings within a file can be directly modified.  This involves simply opening a file in a text editor and applying the
 desired changes.</li>
 <li>Parameter replacement variables can be added to a file, and the variable defined in the UserConfiguration file.  See below for
 details on the UserConfiguration file for how to do this.</li>
 <li>The UserConfiguration file can be overwritten via settings from the command line (again, see below).</li>
 </ul>

 The complete set of configuration files is installed in <maindir>/etc.  The default files are all named to match their identity
 (i.e. the default filename for the GlobalConfiguration file is GlobalConfiguration.txt).  However, that does not necessarily have
 to be the case.  If you make a local copy of a configuration file you are welcome to change its name, as long as you tell
 Katydid where to look.

 Katydid looks for the UserConfiguration file in the following ways (in order of priority, from high to low):
 <ol>
 <li>The user can specify the file on the command line.</li>
 <li>Katydid will look in the configuration file install directory (typically <maindir>/etc) for UserConfiguration.txt.</li>
 <li>If no configuration file is found, the application will exit.</li>
 </ol>

 Kassiopiea looks for other configuration files in two ways (in order of priority, high to low):
 <ol>
 <li>Local copies of configuration files can be specified by the user in the UserConfiguration file or the command line.  These do not
 have to have the default filenames.</li>
 <li>If a configuration file is not found in the run directory, then Katydid looks in the configuration-file directory for the default
 filename. This directory is typically going to be <maindir>/etc, but it could also be something else specified by the user.</li>
 <li>If no configuration file is found, default values (as set in the source code) are used for the parameter settings.</li>
 </ol>



 \section KTExectuable The Katydid Executable
 <hr>
 The executable itself is fairly simple:
 \code
 int main(Int_t argc, Char_t** argv)
 {
 KTManagerFactoryTable::GetInstance()->GetManagerFactory("CoreSimulation")->Build();
 KTCoreManager *tCoreManager = KTCoreManager::GetInstance();
 tCoreManager->SetupApplication(argc, argv);
 tCoreManager->RunApplication();
 tCoreManager->ShutdownApplication();
 return 0;
 }
 \endcode

 The first two lines simply create and return the pointer to the KTCoreManagerSimulation object.

 The third, fourth, and fifth lines are the initialization phase, execution phase, and shutdown phase, respectively.

 Most other applications within Katydid that use the management structure will be similarly structured.  While some of the details discussed
 below will be different from application to application, the overall flow of the applications will be the same.

 \subsection KTInitializing SetupApplication: The Initialization Phase
 <hr>
 The setup of Katydid occurs in two stages:
 <ul>
 <li><b>KTCoreManger::Setup([command line arguments])</b>:   The manager/toolbox hierarchy
 is setup with calls to the Setup() functions of the managers downstream of the KTCoreManager.</li>
 <li><b>KTCoreManager::Prepare()</b>: The command line arguments are parsed and processed.  Next the
 UserConfiguration file is read and processed.  PreliminaryConfiguration settings are applied to the KTCoreManager.
 Parameter replacement variables are parsed and stored in a map that is given out to the KTTokenizer instances as they're created
 (the KTTokenizer class is responsible for performing the actual variable replacement).  Local configuration filenames are stored
 for later access when the KTCoreManager is asked to find the subsequent configuration files.
 Next, all of the other managers and toolboxes are Prepared by the recursive calls of each manager/toolbox's Prepare() function.
 Finally, the KatydidConfiguration file is read in by the KTCoreManager, selecting which tools out of the toolboxes are going to
 be used, and performing the final setup actions.</li>
 </ul>

 \subsection KTExecution RunApplication: The Execution Phase
 <hr>
 The execution of Katydid happens hierarchicaly down the run management chain.
 <ul>
 <li><b>Run</b>
 The simulation is started by a call to KTRunManager::Execute().  First, this function opens a file to which
 it will write the output for the run.  It also starts a timer to keep track of the time taken to run the simulation.
 It then creates the KTRun object, and passes it to the KTRunManager via the KTRunManager::Execute() function.
 boost::any wrappers are used to pass objects from one manager to the next throughout this sequence.
 </li>
 <ul>
 <li><b>Event</b>
 The KTRunManager creates each KTEvent object, and processes it by calling KTEventManager::Execute().
 The KTEventManager first sets the bookkeeping information of the KTEvent (i.e. its number, etc),
 and then populates the KTEvent with one or more "initial particle states" using a generator that is
 randomly selected from those inialized by the user.  each initial particle state is the beginning of a KTTrack.
 The KTEvent The KTeventManager loops over these tracks (and any tracks that are subsequently prodcued),
 handing each one off to the KTTrackManager by calling its Execute() function.
 </li>
 <ul>
 <li><b>Track</b>
 The KTTrackManager first determines the configuration of the StepManager for the next step based on the feedback
 of the KTNavigator object.  The first KTStep is created and handed off to the KTStepManager via
 KTStepManager::Execute().
 </li>
 <ul>
 <li><b>Step</b>
 KTStepManager::ProcessStep() has two parts:
 1. The computation of the step is performed by KTRootProcess::Execute().
 2. Postprocessing of the step (e.g. the exit condition check) is performed.
 The program then returns to the KTTrackManager, where the KTStep's write command is triggered to save the output data.
 </li>
 </ul>
 <li>
 After the KTTrackManager has successfully computed a complete KTTrack, KTrackManager::ProcessTrack
 triggers the writing of the KTTrack.
 </li>
 </ul>
 <li>
 After all KTTracks are processed and written out, the KTEvent is complete.
 KTEventManager::ProcessEvent() write the event to the output data structure.
 </li>
 </ul>
 <li>
 When the specified number of events are complete, the KTRun is written out, and the output file is closed
 by the KTCoreManager.
 </li>
 </ul>

 The format of the output trees depends on the configuration of the KMCDataManager --
 it is either in a full event tree, or in seperate trees for events, tracks and steps.
 This is documented in \ref output.


 \subsection KTShutdown ShutdownApplication: The Shutdown Phase
 <hr>
 This phase involves the recursive calling of each manager or toolbox's Shutdown function.  They are then responsible for
 freeing up all of the memory they are using.



 \page KTOutput Output

 The standard format for all output data is the ROOT TTree.  The output TTrees are stored in a TFile.

 Output falls into three categories: "configuration", "truth" and "real."

 Configuration data contains all the user specifications.
 However it is currently not existent, so you better keep track of your simulation settings....

 Truth data includes all of the simulation-side information.  This includes all information about all particles
 that were simulated, regardless of whether were actually "detected."

 The real data replicates what would be seen in an actual data file.  It uses the same TTree format, and only includes
 information about detected events.  Since the DAQ simulation is currently not functional, there is no
 real data tree in the Katydid output files.

 Two types of truth data are recorded: "physics" events and "DAQ" events.  These are recorded in separate trees.  Again,
 however, since the DAQ simulation is currently not functional, only the TTree of physics events is recorded.

 The truth-side physics data can be written out in one of two different standardized formats,
 both of which reflect the event/track/step hierarchy.  The output format is specified in the GlobalConfiguration file.
 <ul>
 <li><b>TClonesArray</b>: the TFile consists of a TTree of KMCEvents, which also include track and step data
 by using TClonesArrays.  This is the standard output format, since it's faster than the ThreeTrees format.</li>
 <li><b>ThreeTrees</b>: the TFile consists of three trees, one for event, one for track and one for step data.
 This is primarily useful for simulations with large numbers of steps (e.g. trapped-particle studies), and as a more convenient format if you want to use the TTreeViewer in ROOT.
 Otherwise it is slower than the TClonesArray format.</li>
 </ul>

 The classes used to save the various blocks of data are different than the main KTEVent/KTTrack/KTStep classes.
 The output classes are called KMCEvent, KMCTrack, and KMCStep.
 The data are stored in blocks classes are KMCEventData, KMCTrackData, and a few different things for the steps.

 The internal structure of the KMCEvent is rather non-intuitive, since for performance reasons during the saving,
 the data blocks inside an event have to have fixed size.
 A step in Katydid is a rather complicated thing -- different data may need to be saved depending on the active stepper and
 its internal state. Therefore the design of the output file structure has been challenging.

 \image latex Output_fig1.eps "Tracking Step Output" width=\textwidth
 \image html Output_fig1.png

 The KMCStep class is special in the sense that its size in memory can change from one step to the next.
 e.g. you may chose to save scattering information only if a scattering interaction actually took place,
 or a process can be switched of and thus stop providing data altogether.
 For this reason, the step itself is split up in different fixed size blocks when it is written to disk
 and the KMCStep again has a non-trivial substructure.
 It consists of several Data blocks, which may or may not be present, and it's Save function knows how to deal with that.
 Simultaneously, it has a ReadNextStep block for accessing each step recursively.
 Note that the KMCStep class is strictly speaking not necessary, since it is not written to disk.
 it is only provided to encapsulate the complex step structure of a step within it.

 The event structure as it is written to file is depicted in Fig.2: An event consists of an array of fixed sized blocks for the Track data,
 and presently eight fixed sized blocks which may of may not be written during each step.
 In order to retrieve a step from the track, the Trackdata block also contains a two-layered key system allowing the retrieval of step information.
 This key scheme is explained in the following figures:

 \image latex Output_fig2.eps "Step Keys scheme" width=\textwidth
 \image html Output_fig2.png

 \image latex Output_fig3.eps "Step Keys example" width=\textwidth
 \image html Output_fig3.png


 In order to ensure accessibility of the data at all times,
 the output classes have a version ID provided by the ROOT object IO system.
 If their design changes, this version number will have to be increased.


 \section KTAnalysis Analysis
 <hr>
 \subsection KTTools existing tools
 As an extremely preliminary general analysis tool, the macro FileReader.C is installed in the binary folder.
 It can be used as follows:
 First of all, start root.
 Then in root do:
 \code
 root [0] .L /path/to/kassiopeia/bin/FileReader.C
 root [1]  FileReader("/path/to/kassiopeia-filename.root", "/path/to/kassiopeia/lib);
 \endcode
 The intension of that macro is that the user adjusts it to suite his purposes. That is the main reason why it is provided as a ROOT macro, not a compiled binary.
 Apart from missing header inclusions, the code in the macro compiles.
 So if you want to compile your own analysis code, the following piece of code shows how to read the output.
 It is a one to one copy from the macro FileReader.C. Make sure to read the section about linking to kassiopeia.
 This code works regardless of the file format, although it is to some extend experimental.
 Note that the the Filereader macro gets overwritten everytime you do make install, so you better rename it.
 It's original source code can be found in Application/Macros...


 \code
 Katydid::KMCDataManager* myManager = Katydid::KMCDataManager::GetInstance();
 myManager->OpenFile(argv[1],0);
 Katydid::KMCEventIterator* myevent2 = myManager->GetEvent();
 //unitilized pointer to event data, does not contain any useful information at this point

 for (UInt_t ievent = 0; ievent < myManager->GetNEvents(); ievent++){
 myManager->ReadNextEvent(); //now we load the event data of the next event.
 Katydid::KMCTrackIterator* mytrack2 = myevent2->GetTrack();
 for (UInt_t itrack = 0; itrack < myevent2->GetNTracks(); itrack++){
 myevent2->ReadNextTrack();
 Katydid::KMCStepIterator* myStep2 = mytrack2->GetStep();
 for (UInt_t istep = 0; istep < mytrack2->GetNSteps(); istep++){
 mytrack2->ReadNextStep();
 }
 }
 }
 \endcode
 Additionally, a program called Trackplotter is installed, which plots the geometry and tracks of a simulation.

 \subsection KTlinking Linking your own code against Katydid
 In case you want to link your own code against kassiopeia, you can use the 'pkg-config'utility to print the compiler and linker flags using
 \code
 pkg-config --cflags kassiopeia
 pkg-config --libs kassiopeia
 \endcode
 in the command line or rather your makefile. pkg-config is a general tool, its purpose is similar to the root-config utlity you probably know.
 The way this works is that Katydid installs a file called libkassiopeia.pc.
 This file tells pkg-config the necessary information so that it can print the linker options.
 For this to work, pkg-config must find this file!
 That either requires that you install kassiopeia in a default place like /usr/,
 or that you add the directory containing kassiopeia.pc (@prefix@/@libdir@/pkg-config) to the environment variable PKG_CONFIG_PATH.

 Alternatively, you can also include your own code in the Katydid build itself. Please look at the README for instructions.

 You can find a small makefile below:

 \code
 #this is a simple makefile for linking your own executable against kassiopeia.
 #it assumes that root-config and gsl-config are in PATH and kassiopeia.pc is in PKG_CONFIG_PATH
 # otherwise it will probably not work...
 #also it requires an executable in a file called test.cxx.
 #you might need to add the path to boost's headers to the CXXFLAGS too.

 CXXFLAGS = `pkg-config --cflags kassiopeia` `root-config --cflags` `gsl-config --cflags` -Wall -W -O2
 LDFLAGS =  `pkg-config --libs kassiopeia` `root-config --libs` `gsl-config --libs`
 OBJECTS = test.o
 CXX = g++


 test.exe: ${OBJECTS}
 ${CXX} ${CXXFLAGS} ${LDFLAGS} ${OBJECTS} -o test.exe

 %.o: %.cxx
 ${CXX} ${CXXFLAGS}  -c $<

 all: test.exe

 clean:
 rm -f test.exe *.o *~
 \endcode

 it tries to compile a file calles test.cxx and will spit out an executable test.exe. the first thing you can try is to try to compile the following classic code:
 \code
 #include <iostream>
 #include "KTLog.hh"

 int main(){
 std::cout<<"hello world"<<std::endl;
 Katydid::KTLog* myLog = Katydid::KTLog::GetInstance();
 myLog->SetLogFileName("foo.txt");
 myLog->StartLog();
 myLog->LogStream() <<  "hello logger" << std::endl;
 }
 \endcode
 From then on, you are on your own.

 For a complex project, you might want to use autotools, the following snippet shows the important part of configure.ac:
 \code
 #this is an autoconf macro which calls pkg-config and if successful stores the return values in
 #KASSIOPEIA_CFLAGS
 #and
 #KASSIOPEIA_LIBS
 # respectively
 PKG_CHECK_MODULES([KASSIOPEIA], [kassiopeia])
 #the other variables need to be set before too....
 AM_CPPFLAGS="$ROOT_CPPFLAGS $BOOST_CPPFLAGS  $(GSL_CFLAGS) $KASSIOPEIA_CFLAGS"
 AM_CXXFLAGS="-g -W -Wall $ROOT_CXXFLAGS"
 AM_LDFLAGS="-fPIC -pg -g $ROOT_LDFLAGS $GSL_LIBS $KASSIOPEIA_LIBS"
 \endcode


 \subsection KTdynamicLoading Dynamic loading of kassiopeias output classes in ROOT
 Another small little feature which might come in handy: Katydid installs a file called libkassiopeia.rootmap.
 This file tells root in a CINT session (i.e. the root command line), which libraries it needs to load if it encounters kassiopeias classnames.
 CINT will then try to do that. Of course it only works, if root finds the rootmap file and the library.
 I haven't found any explicit documentation where ROOT searches for these files, but it seems to look for bith in the current directory, default places like /usr/lib, the root library directory probably LD_LIBRARYPATH.

 You can check whether ROOT knows the kassiopeia DataStructure classes by opening a simulation output file in root, e.g. by passing it as argument to root, when starting it.
 in that case, you could directly use
 \code
 Katydid::KMCDataManager* myManager = Katydid::KMCDataManager::GetInstance();
 \endcode
 and the following lines from the block above, in a CINT session and it would work...

 if you see lots of warnings like that,
 \code
 Warning in <TClass::TClass>: no dictionary for class Katydid::KMCEventData is available
 ...
 \endcode
 ROOT has not found the rootmap file.
 If you additionally see
 \code
 Warning in <TClass::TClass>: no dictionary for class Katydid::KMCEventData is available
 Error in <TUnixSystem::DynamicPathName>: libKTDataStructure.so does not exist in .:...
 ...
 \endcode
 ROOT has found the rootmap file, but not the library.

 and if it just opens the file without any complaints, everything works. Congratulations.

 The FileReader macro does not rely on the correct installation of the rootmap file, but rather forces the user to specify the directory of the library by hand.


 \page KTGlossary Glossary

 <ul>
 <li>Run: a series of subruns with a common purpose (e.g .a calibration measurement, etc.) </li>
 <li>Subrun: a measurement where all control parameters are constant (or he simulation thereof)</li>
 <li>Event: a physical reaction (e.g a radioactive decay) inside the KATRIN setup and its consequences </li>
 <li>Track: a particle and its history  </li>
 <li>Step: the State of a particle at a given time </li>
 <li> Geometry:</li> pure geometric information
 <li>(Tracking) Strategy: all settings (step computation method, exit conditions, and possibly field computation methods) necessary to specify the particle transportation method for a region </li>
 <li> Configuration: a part of the experimental setup and the strategy associated with it (?).  </li>
 <li> <maindir> Katydids top level folder </li>
 <li> complex type: a data block with some structure (usually but not necessarily corresponding to a Katydid class)</li>
 <li> Toolbox: a container for tools</li>
 </ul>

 \page KTModules Modules
 <ul>
 <li>\subpage KTRACKMain "KTrack"
 <li>\subpage KESSMain "KESS - KATRIN Electron Scattering in Silicon"
 <li>\subpage KPAGEmain "KPAGE - KATRIN PArticle GEnerator"
 <li>\subpage SSCmain "SSC - Source Spectrum Calculation"
 </ul>

 \attention The source of this documentation can be found in: /KTCore/Main/include/KTCoreManager.hh

 */

#endif // KTCORE_MANAGER_H
