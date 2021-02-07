# Use baseimage-docker which is a modified Ubuntu specifically for Docker
# https://hub.docker.com/r/phusion/baseimage
# https://github.com/phusion/baseimage-docker

FROM phusion/baseimage:18.04-1.0.0

# Use baseimage-docker's init system
CMD ["/sbin/my_init"]

# Update and install packages
RUN apt update && apt -y upgrade && apt -y install \
	build-essential \
	git \
	libtool-bin \
  doxygen \
  graphviz \
  time \
	mpich \
  python3-dev \
  python3-pip \
	automake 

RUN pip3 install pybind11

# Clean up apt mess
RUN apt clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Setup Environment for SST
ARG dir=/home/sst/build
RUN mkdir -p $dir
ARG SST_CORE_HOME=/home/sst/sst-core
ENV SST_CORE_HOME=/home/sst/sst-core
ENV PATH="$PATH:$SST_CORE_HOME/bin"

# Clone the repos from GitHub
#RUN git clone https://github.com/sstsimulator/sst-core.git $dir/sst-core

WORKDIR $dir

# from https://github.com/sstsimulator/sst-core/releases/tag/v10.1.0_Final
COPY sstcore-10.1.0.tar.gz .
RUN tar zxvf sstcore-10.1.0.tar.gz
RUN mv sstcore-10.1.0 sst-core

# Build SST Core
RUN cd $dir/sst-core && ./autogen.sh && \
	./configure --prefix=$SST_CORE_HOME && \
	make all install

WORKDIR /opt/

RUN git clone https://github.com/sabbirahm3d/SIT.git

# Following is from https://github.com/sabbirahm3d/SIT/blob/master/.github/workflows/gcc.yml
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update

RUN apt install -y libhwloc-dev libopenmpi-dev openmpi-bin libtool

# https://cmake.org/download/
RUN mkdir /opt/cmake/
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.19.4/cmake-3.19.4-Linux-x86_64.tar.gz  | tar xz -C /opt/cmake
ENV PATH="/opt/cmake/cmake-3.19.4-Linux-x86_64/bin/:${PATH}"


RUN mkdir /opt/systemc/
RUN curl -L https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz | tar xz -C /opt/systemc


ARG LIBZMQ_VER="4.3.2"
ARG CPPZMQ_VER="4.6.0"
ARG libzmq_url="https://github.com/zeromq/libzmq/archive/v${LIBZMQ_VER}.tar.gz"
ARG cppzmq_url="https://github.com/zeromq/cppzmq/archive/v${CPPZMQ_VER}.tar.gz"

RUN mkdir /opt/zmq/
RUN curl -L ${libzmq_url} | tar xz -C /opt/zmq
RUN curl -L ${cppzmq_url} | tar xz -C /opt/zmq 
#RUN mkdir -p ${DEPS_DIR}/libzmq-${LIBZMQ_VER}/build
#RUN mkdir -p ${DEPS_DIR}/cppzmq-${CPPZMQ_VER}/build

RUN pip3 install pyzmq

RUN pip3 install pyrtl

RUN apt install -y iverilog
RUN pip3 install cocotb

RUN mkdir /opt/SIT/build
WORKDIR /opt/SIT/build

# cmake .. 

