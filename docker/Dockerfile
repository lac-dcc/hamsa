################################################# Image #################################################
FROM ubuntu:20.04

############################################# Date and Time #############################################
ENV TZ="America/Sao_Paulo"
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
    && rm -rf /var/lib/apt/lists/* 

RUN apt-get update -y && apt-get install -y locales curl wget tar sudo git apt-utils \
    && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

ENV LANG en_US.utf8

############################################# DEPENDENCIES ##############################################
RUN apt-get update -y && apt-get install -y gcc g++ ninja-build graphviz vim python2 python3 python3-pip \
    python3-dev automake make build-essential haskell-stack

############################################# CMAKE #####################################################
RUN wget -c https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0-linux-x86_64.tar.gz \
    && tar -xvf cmake-3.20.0-linux-x86_64.tar.gz \
    && cp -r cmake-3.20.0-linux-x86_64/* /usr \
    && rm -rf cmake-3.20.0-linux-x86_64*

############################################## LLVM 14 ###################################################
RUN cd $HOME && git clone -b llvmorg-14.0.0 https://github.com/llvm/llvm-project \
    && cd llvm-project \
    && mkdir -p build \
    && cd build \
    && cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -G "Ninja" ../llvm \
    && ninja \
    && ninja install

############################################## PSYCHE-C ##################################################
RUN cd $HOME && git clone -b original https://github.com/ltcmelo/psychec.git \
    && cd psychec && cd solver \
    && stack build && cd .. \
    && cmake CMakeLists.txt && make -j 4

########################################### COPY HAMSA ##################################################
RUN cd $HOME && git clone -b perfModel https://github.com/lac-dcc/hamsa 

########################################### SETUP TOOL ##################################################
RUN cd $HOME/hamsa/tool && ./setup.sh

########################################### WORKDIR #####################################################
WORKDIR /root/hamsa
ENTRYPOINT ["/bin/bash"]
