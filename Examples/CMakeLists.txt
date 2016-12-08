# CMakeLists for Katydid/Examples
# Author: N. Oblath

# Tutorial code is optionally built from the Source/SpectrumAnalysis and Source/Data CMakeLists files.
# Processor template code is built from Source/Utility CMakeLists file.

# Config files
set (EXAMPLE_CONFIGFILES
    ConfigFiles/DisplayFFT.json
    ConfigFiles/EvaluateCandidates.json
    ConfigFiles/FSPeakAnalysisConfig.json
    ConfigFiles/KatydidPSAnalysisTemplate.json
    ConfigFiles/KatydidPSComplexConfig.json
    ConfigFiles/KatydidPSConfig.json
    ConfigFiles/KatydidWVConfig.json
    ConfigFiles/PrintInfo.json
    ConfigFiles/SpectrogramConfig.json
    ConfigFiles/ThroughputProfiler.json
    ConfigFiles/ts-plot-direct-from-mantis.json
    ConfigFiles/HDF5/AverageFFT.json
    ConfigFiles/HDF5/AveragePS.json
    ConfigFiles/HDF5/ConvertEggToHDF5.json
    
    ConfigFiles/DisplayFFT.yaml
    ConfigFiles/DisplaySpectrograms.yaml
    ConfigFiles/KatydidPSConfig.yaml
    ConfigFiles/SidebandTrackAnalysis.yaml    
)

pbuilder_install_config (${EXAMPLE_CONFIGFILES})
