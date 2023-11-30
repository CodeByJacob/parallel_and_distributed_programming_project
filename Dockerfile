FROM jwojtas1306/gcc_openmpi:1.0.0

LABEL authors="Jakub Wojtas, Piotr Ploszczyca"

ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

COPY ./app ./app
WORKDIR ./app

RUN ["make","-f","Makefile"]
ENTRYPOINT ["make", "run_all"]