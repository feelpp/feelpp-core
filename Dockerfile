# SPDX-FileCopyrightText: 2025 Université de Strasbourg
# SPDX-FileContributor: Christophe Prud'homme
# SPDX-FileContributor: Vincent Chabannes
# SPDX-License-Identifier: LGPL-3.0-or-later

# -*- mode: Dockerfile -*-

FROM ghcr.io/feelpp/feelpp-env:ubuntu-24.04

USER root
COPY . /home/feelpp/
RUN ls -lrtR /home/feelpp

RUN dpkg -i /home/feelpp/*.deb 

USER feelpp

