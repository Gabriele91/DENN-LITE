#!/bin/bash
TOP=$(pwd)
#config
CUDA=OFF
MKL=OFF
MKL_INCLUDE=OFF
SHARED=OFF

#args
while getopts "cuda:mkl:mkl_inc:shared:" option; do
 case "${option}" in
 cuda) CUDA=${OPTARG};;
 mkl) MKL=${OPTARG};;
 mkl_inc) MKL_INCLUDE=${OPTARG};;
 shared) SHARED=${OPTARG};;
 \?)
    #no error
  ;;
 esac
done

#dips
if [ "$(uname)" == 'Darwin' ]; then
  brew install cmake \
  		       protobuf \
  		       gflags \
			   eigen
else
  sudo apt install build-essential \
                   cmake \
                   libgoogle-glog-dev \
                   libprotobuf-dev \
				   libeigen3-dev\
                   protobuf-compiler
fi


#clone nodes
git clone https://github.com/caffe2/caffe2
#prefix
PREFIX=$PWD/caffe2/caffe2_cmake_release
#make dir
mkdir -p caffe2/caffe2_cmake_release

#go to caffe
cd caffe2

#args
CMAKE_ARGS=()

# Default leveldb from conda-forge doesn't work. If you want to use leveldb,
# use this old pip version
# pip install leveldb==0.18
CMAKE_ARGS+=("-DUSE_LEVELDB=OFF")


# This installation defaults to using MKL because it is much faster. If you
# want to build without MKL then you should also remove mkl from meta.yaml in
# addition to removing the flags below
if [ "$MKL" == "ON" ]; then
  CMAKE_ARGS+=("-DBLAS=MKL")
  CMAKE_ARGS+=("-DMKL_INCLUDE_DIR=$MKL_INCLUDE")
fi

# The following libraries are incompatible with macOS builds
CMAKE_ARGS+=("-DUSE_GLOO=OFF")
CMAKE_ARGS+=("-DUSE_MOBILE_OPENGL=OFF")
CMAKE_ARGS+=("-DUSE_METAL=OFF")

# It is rare for macs to have a GPU card with CUDA support, but if you have one
# then remove these flags
CMAKE_ARGS+=("-DUSE_CUDA=$CUDA")
CMAKE_ARGS+=("-DUSE_NCCL=$CUDA")
CMAKE_ARGS+=("-DUSE_NNPACK=OFF")

#no mpi
CMAKE_ARGS+=("-DUSE_MPI=OFF")

#no python, no test
CMAKE_ARGS+=("-DBUILD_PYTHON=OFF")
CMAKE_ARGS+=("-DBUILD_TEST=OFF")

#shared
CMAKE_ARGS+=("-DBUILD_SHARED_LIBS=$SHARED")

# Install under specified prefix
CMAKE_ARGS+=("-DCMAKE_INSTALL_PREFIX=$PREFIX")
CMAKE_ARGS+=("-DCMAKE_PREFIX_PATH=$PREFIX")

#build dir caffe
mkdir -p build
cd build

#create make file
cmake "${CMAKE_ARGS[@]}" ..

#make
if [ "$(uname)" == 'Darwin' ]; then
  make "-j$(sysctl -n hw.ncpu)"
else
  make "-j$(nproc)"
fi

# #back
cd ../../

#build dips dir
mkdir -p $TOP/../dips/lib/linux/
mkdir -p $TOP/../dips/lib/macOS/
#build include dir
SUBDIRS=$(find $TOP/caffe2/caffe2 -type d)
for dir in $SUBDIRS
do
	mkdir -p $(echo $dir | sed -e 's/caffe2\/caffe2/..\/dips\/include\/caffe2/g')
done
#copy
#make
if [ "$(uname)" == 'Darwin' ]; then
  find $TOP/caffe2/build/lib/ -name \*.a -exec cp {} $TOP/../dips/lib/macOS/ \;
else
  find $TOP/caffe2/build/lib/ -name \*.a -exec cp {} $TOP/../dips/lib/linux/ \;
fi
#copy headers
SUBFILES=$(find $TOP/caffe2/caffe2 -type f -name \*.h)
for files in $SUBFILES
do
  cp $files $(echo $files | sed -e 's/caffe2\/caffe2/..\/dips\/include\/caffe2/g')
done
#copy proto headers
SUBFILES=$(find $TOP/caffe2/caffe2 -type f -name \*.proto)
for files in $SUBFILES
do
  cp $files $(echo $files | sed -e 's/caffe2\/caffe2/..\/dips\/include\/caffe2/g')
done
#copy pb.h headers (linux)
SUBFILES=$(find $TOP/caffe2/caffe2 -type f -name \*.pb.h)
for files in $SUBFILES
do
  cp $files $(echo $files | sed -e 's/caffe2\/caffe2/..\/dips\/include\/caffe2/g')
done
#delete caffe
yes | rm -R caffe2

