libs_for_unix = -ledit
libs_for_windows =
libs = -lm

ifeq ($(OS),Windows_NT)
	libs += $(libs_for_windows)
else
	libs += $(libs_for_unix)
endif

all:
	$(CC) -g -o byol-lisp main.c mpc.c types.c $(libs)
