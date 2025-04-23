# -*- mode: Dockerfile -*-

FROM ghcr.io/feelpp/feelpp-env:ubuntu-24.04

USER root
COPY . /home/feelpp/
RUN ls -lrtR /home/feelpp

RUN dpkg -i /home/feelpp/*.deb 

USER feelpp

