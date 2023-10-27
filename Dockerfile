FROM jwojtas1306/gcc_openmpi:x86_64_1.0.0

LABEL authors="Jakub Wojtas, Piotr Ploszczyca"

COPY ./app ./app
WORKDIR ./app


RUN ["make","-f","Makefile"]
ENTRYPOINT ["make", "run_all"]

