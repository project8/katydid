FROM project8/p8compute_dependencies:v0.2.0 as katydid_common

ARG build_type=Release
ENV KATYDID_BUILD_TYPE=$build_type

ENV KATYDID_TAG=v2.15.0
ENV KATYDID_BUILD_PREFIX=/usr/local/p8/katydid/$KATYDID_TAG

RUN mkdir -p $KATYDID_BUILD_PREFIX &&\
    cd $KATYDID_BUILD_PREFIX &&\
    echo "source ${COMMON_BUILD_PREFIX}/setup.sh" > setup.sh &&\
    echo "export KATYDID_TAG=${KATYDID_TAG}" >> setup.sh &&\
    echo "export KATYDID_BUILD_PREFIX=${KATYDID_BUILD_PREFIX}" >> setup.sh &&\
    echo 'ln -sf $KATYDID_BUILD_PREFIX $KATYDID_BUILD_PREFIX/../current || /bin/true' >> setup.sh &&\
    echo 'export PATH=$KATYDID_BUILD_PREFIX/bin:$PATH' >> setup.sh &&\
    echo 'export LD_LIBRARY_PATH=$KATYDID_BUILD_PREFIX/lib:$LD_LIBRARY_PATH' >> setup.sh &&\
    /bin/true

########################
FROM katydid_common as katydid_done

# repeat the cmake command to get the change of install prefix to set correctly (a package_builder known issue)
RUN source $KATYDID_BUILD_PREFIX/setup.sh &&\
    mkdir /tmp_install &&\
    cd /tmp_install &&\
    git clone https://github.com/project8/katydid &&\
    cd katydid &&\
    git fetch && git fetch --tags &&\
    git checkout $KATYDID_TAG &&\
    git submodule update --init --recursive &&\
    mkdir build &&\
    cd build &&\
    cmake -D CMAKE_BUILD_TYPE=$KATYDID_BUILD_TYPE \
          -D CMAKE_INSTALL_PREFIX:PATH=$KATYDID_BUILD_PREFIX .. &&\
    cmake -D CMAKE_BUILD_TYPE=$KATYDID_BUILD_TYPE \
          -D CMAKE_INSTALL_PREFIX:PATH=$KATYDID_BUILD_PREFIX .. &&\
    make -j3 install &&\
    /bin/true

########################
FROM katydid_common

COPY --from=katydid_done $KATYDID_BUILD_PREFIX $KATYDID_BUILD_PREFIX
