FROM jwojtas1306/gcc_openmpi:x86_64_1.0.0

LABEL authors="Jakub Wojtas, Piotr Ploszczyca"

ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

RUN apt-get update \
    && apt-get install -y python3 python3-pip \
    && apt-get install -y python3 python3-venv

COPY ./app ./app
WORKDIR ./app

RUN ["make","-f","Makefile"]
ENTRYPOINT ["make", "run_and_make_plots"]