# SOD does not generally require a Makefile to build. Just drop sod.c and its accompanying
# header files on your source tree and you are done.
CC = emcc
CFLAGS = -lm -Wall -std=c99 -Wimplicit-int-float-conversion

sod: sod.c
	$(CC) sod.c samples/cnn_face_detection.c -o sod_face_detect.html -I. $(CFLAGS)
