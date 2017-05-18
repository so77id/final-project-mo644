FROM ubuntu
# ...
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get -y install gcc mono-mcs make vim && \
    rm -rf /var/lib/apt/lists/*

ADD ./cholesky/cholesky_serial.c cholesky_serial.c
ADD ./cholesky/cholesky_openmp.c cholesky_openmp.c
ADD ./cholesky/cholesky_pthreads.c cholesky_pthreads.c
ADD ./cholesky/cholesky_cuda.cu cholesky_cuda.cu

ADD ./inputs/* ./

ENTRYPOINT ["bash"]