ARG final_img_repo=ghcr.io/project8/luna_base
ARG final_img_tag=v1.3.3

ARG build_img_repo=ghcr.io/project8/luna_base
ARG build_img_tag=v1.3.3-dev

########################
FROM ${build_img_repo}:${build_img_tag} AS build

ARG build_type=Release
ARG build_tests_exe=FALSE
ARG katydid_subdir=katydid
ARG katydid_tag=beta
ARG nproc=4

ENV KATYDID_PREFIX=${P8_ROOT}/${katydid_subdir}/${katydid_tag}

RUN source ${P8_ROOT}/common/current/setup.sh &&\
    mkdir -p $KATYDID_PREFIX &&\
    chmod -R 777 $KATYDID_PREFIX/.. &&\
    cd $KATYDID_PREFIX &&\
    echo "source ${COMMON_PREFIX}/setup.sh" > setup.sh &&\
    echo "export KATYDID_TAG=${KATYDID_TAG}" >> setup.sh &&\
    echo "export KATYDID_PREFIX=${KATYDID_PREFIX}" >> setup.sh &&\
    echo 'ln -sfT $KATYDID_PREFIX $KATYDID_PREFIX/../current' >> setup.sh &&\
    echo 'export PATH=$KATYDID_PREFIX/bin:$PATH' >> setup.sh &&\
    echo 'export LD_LIBRARY_PATH=$KATYDID_PREFIX/lib:$LD_LIBRARY_PATH' >> setup.sh &&\
    /bin/true

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

# repeat the cmake command to get the change of install prefix to set correctly (a package_builder known issue)
RUN source $KATYDID_PREFIX/setup.sh &&\
    cd /tmp_source &&\
    mkdir build &&\
    cd build &&\
    cmake .. &&\
    cmake -D CMAKE_BUILD_TYPE=$build_type \
          -D CMAKE_INSTALL_PREFIX:PATH=$KATYDID_PREFIX \
          -D Katydid_ENABLE_TESTING:BOOL=$build_tests_exe \
          -D CMAKE_SKIP_RPATH:BOOL=True .. &&\
    make -j$nproc install &&\
    /bin/true

########################
FROM ${final_img_repo}:${final_img_tag}

COPY --from=build $KATYDID_PREFIX $KATYDID_PREFIX
