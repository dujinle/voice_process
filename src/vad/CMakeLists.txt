cmake_minimum_required(VERSION 2.6)
SET(RESOURCES
		vad_core.c
		vad_filterbank.c
		vad_gmm.c
		vad_sp.c
		webrtc_vad.c
)

ADD_LIBRARY(libvad STATIC ${RESOURCES})
