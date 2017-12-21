cmake_minimum_required(VERSION 2.6)
SET(RESOURCES
		vad.c
)

ADD_LIBRARY(libvad STATIC ${RESOURCES})
