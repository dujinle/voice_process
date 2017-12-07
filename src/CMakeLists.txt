cmake_minimum_required(VERSION 2.6)

ADD_SUBDIRECTORY(vad_snr)
ADD_SUBDIRECTORY(core)

SET(RESOURCES
	process.c
)

ADD_LIBRARY(libprocess STATIC ${RESOURCES})
target_link_libraries(libprocess libvad)
target_link_libraries(libprocess libcore)
target_link_libraries(libprocess ${libsndfile})
target_link_libraries(libprocess m)

#ADD_LIBRARY(libcore STATIC ${RESOURCES})

#	TARGET_LINK_LIBRARIES(libcore ${libopencv2_jpeg})
