add_library(KatydidEgg
    ${PROJECT_CURRENT_SOURCE_DIR}/KTArrayUC.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTEgg.cc
    ${PROJECT_CURRENT_SOURCE_DIR}/KTEvent.cc
)

install (FILES ${PROJECT_CURRENT_SOURCE_DIR}/KTArrayUC.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTEgg.hh
               ${PROJECT_CURRENT_SOURCE_DIR}/KTEvent.hh
         DESTINATION include)
         