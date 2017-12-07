cmake_minimum_required(VERSION 2.6)
SET(RESOURCES
	pre_process.c
	fft.c
	tools.c
	mel_bank.c
	cmfcc.c
	mass_center.c
	compare.c
)

ADD_LIBRARY(libcore STATIC ${RESOURCES})
