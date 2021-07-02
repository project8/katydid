ARG IMG_USER=project8
ARG IMG_REPO=p8compute_dependencies
ARG IMG_TAG=v1.0.0

FROM ${IMG_USER}/${IMG_REPO}:${IMG_TAG} as katydid_common

ARG build_type=Release
ENV KATYDID_BUILD_TYPE=$build_type
ARG build_tests_exe=FALSE
ENV KATYDID_BUILD_TESTS_EXE=$build_tests_exe

ENV KATYDID_TAG=v2.18.0
ENV KATYDID_BUILD_PREFIX=/usr/local/p8/katydid/$KATYDID_TAG

RUN mkdir -p $KATYDID_BUILD_PREFIX &&\
    chmod -R 777 $KATYDID_BUILD_PREFIX/.. &&\
    cd $KATYDID_BUILD_PREFIX &&\
    echo "source ${COMMON_BUILD_PREFIX}/setup.sh" > setup.sh &&\
    echo "export KATYDID_TAG=${KATYDID_TAG}" >> setup.sh &&\
    echo "export KATYDID_BUILD_PREFIX=${KATYDID_BUILD_PREFIX}" >> setup.sh &&\
    echo 'ln -sfT $KATYDID_BUILD_PREFIX $KATYDID_BUILD_PREFIX/../current' >> setup.sh &&\
    echo 'export PATH=$KATYDID_BUILD_PREFIX/bin:$PATH' >> setup.sh &&\
    echo 'export LD_LIBRARY_PATH=$KATYDID_BUILD_PREFIX/lib:$LD_LIBRARY_PATH' >> setup.sh &&\
    /bin/true

########################
FROM katydid_common as katydid_done

COPY Cicada /tmp_source/Cicada
COPY cmake /tmp_source/cmake
COPY Examples /tmp_source/Examples
COPY External /tmp_source/External
COPY Nymph /tmp_source/Nymph
COPY Source /tmp_source/Source
COPY CMakeLists.txt /tmp_source/CMakeLists.txt
COPY KatydidConfig.cmake.in /tmp_source/KatydidConfig.cmake.in
COPY KatydidConfig.hh.in /tmp_source/KatydidConfig.hh.in
COPY libkatydid.rootmap /tmp_source/libkatydid.rootmap
COPY this_katydid.sh.in /tmp_source/this_katydid.sh.in
COPY .git /tmp_source/.git
COPY CI /$KATYDID_BUILD_PREFIX/CI

# repeat the cmake command to get the change of install prefix to set correctly (a package_builder known issue)
RUN source $KATYDID_BUILD_PREFIX/setup.sh &&\
    cd /tmp_source &&\
    mkdir build &&\
    cd build &&\
    cmake -D CMAKE_BUILD_TYPE=$KATYDID_BUILD_TYPE \
          -D CMAKE_INSTALL_PREFIX:PATH=$KATYDID_BUILD_PREFIX \
          -D Katydid_ENABLE_TESTING:BOOL=$KATYDID_BUILD_TESTS_EXE \
          -D CMAKE_SKIP_RPATH:BOOL=True .. &&\
    cmake -D CMAKE_BUILD_TYPE=$KATYDID_BUILD_TYPE \
          -D CMAKE_INSTALL_PREFIX:PATH=$KATYDID_BUILD_PREFIX \
          -D Katydid_ENABLE_TESTING:BOOL=$KATYDID_BUILD_TESTS_EXE \
          -D CMAKE_SKIP_RPATH:BOOL=True .. &&\
    make -j3 install &&\
    /bin/true

########################
FROM katydid_common

COPY --from=katydid_done $KATYDID_BUILD_PREFIX $KATYDID_BUILD_PREFIX
