# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Start from the base CERN CentOS 7 image.
FROM cern/cc7-base:latest

# Perform the image setup as root.
USER root
WORKDIR /root

# Install some additional packages.
RUN yum -y install which sclo-git212 wget tar atlas-devel libuuid-devel texinfo \
           python2-pip redhat-lsb-core libX11-devel libXpm-devel libXft-devel  \
           libXext-devel openssl-devel glibc-devel rpm-build libcurl-devel     \
           mesa-libGL-devel libaio && \
    yum clean all

# Set up the ATLAS user, and give it super user rights.
RUN echo '%wheel	ALL=(ALL)	NOPASSWD: ALL' >> /etc/sudoers && \
    adduser atlas && chmod 755 /home/atlas && \
    usermod -aG wheel atlas

# Copy the interactive environment setup scripts in place.
COPY atlas_prompt.sh setup_atlas.sh enable_sclo_git212.sh /etc/profile.d/
# Add the message of the day instructions for the image.
COPY motd /etc/

# Install flake8 python linter
RUN pip install flake8 six
RUN scl enable sclo-git212 'pip install git+https://:@gitlab.cern.ch:8443/atlas/atlasexternals.git#subdirectory=External/flake8_atlas'

# Switch to the ATLAS account.
USER atlas
WORKDIR /home/atlas
RUN echo "cat /etc/motd" >> /home/atlas/.bashrc

# Set up the command to run when starting the image.
CMD /bin/bash
