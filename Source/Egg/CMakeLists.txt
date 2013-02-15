# CMakeLists for Katydid/Source/Egg
# Author: N. Oblath

set (EGG_DICT_HEADERFILES
)

set (EGG_NODICT_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEgg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArrayFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSliceHeader.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeries.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesData.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesChannelData.hh
    #${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesPairedData.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesFFTW.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesReal.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/MonarchPP.hh
)

set (EGG_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEgg.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggProcessor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReader2011.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTEggReaderMonarch.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTJSONTypeWriterEgg.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTPhysicalArrayFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTSliceHeader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTThroughputProfiler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeries.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesData.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesChannelData.cc
    #${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesPairedData.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesFFTW.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/KTTimeSeriesReal.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/MonarchPP.cc
)

if (ROOT_FOUND) 
    set (EGG_NODICT_HEADERFILES
        ${EGG_NODICT_HEADERFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiBundleROOTTypeWriterEgg.hh
    )

    set (EGG_SOURCEFILES
        ${EGG_SOURCEFILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/KTBasicROOTTypeWriterEgg.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/KTMultiBundleROOTTypeWriterEgg.cc
    )		
endif (ROOT_FOUND)

set (EGG_HEADERFILES ${EGG_DICT_HEADERFILES} ${EGG_NODICT_HEADERFILES})

#set (EGG_LINKDEF_HEADERFILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef/EggLinkDef.hh)
#set (EGG_DICT_OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/EggDict.cxx)

set (KATYDID_LIBS
    KatydidUtility
    KatydidCore
    KatydidOutput
    ${Monarch_LIBRARIES}
)

##################################################

#ROOT_GENERATE_DICTIONARY ("${EGG_DICT_HEADERFILES}" "${EGG_LINKDEF_HEADERFILE}" "${EGG_DICT_OUTFILE}" "${CMAKE_INCLUDE_PATH}")

add_library (KatydidEgg ${EGG_SOURCEFILES} ${EGG_DICT_OUTFILE})
target_link_libraries (KatydidEgg ${KATYDID_LIBS} ${LIBS})

katydid_install_headers (${EGG_HEADERFILES})
katydid_install_libraries (KatydidEgg)
         
