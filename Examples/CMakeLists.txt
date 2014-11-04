# CMakeLists for Katydid/Examples
# Author: N. Oblath

# Tutorial code is optionally built from the Source/SpectrumAnalysis and Source/Data CMakeLists files.

# Config files
set (EXAMPLE_CONFIGFILES
    ConfigFiles/DisplayFFT.json
    ConfigFiles/EvaluateCandidates.json
    ConfigFiles/FSPeakAnalysisConfig.json
    ConfigFiles/KatydidPSAnalysisTemplate.json
    ConfigFiles/KatydidPSComplexConfig.json
    ConfigFiles/KatydidPSConfig.json
    ConfigFiles/KatydidWVConfig.json
    ConfigFiles/ThroughputProfiler.json
    ConfigFiles/ts-plot-direct-from-mantis.json
    ConfigFiles/HDF5/AverageFFT.json
    ConfigFiles/HDF5/AveragePS.json
    ConfigFiles/HDF5/ConvertEggToHDF5.json
)

pbuilder_install_config (${EXAMPLE_CONFIGFILES})
