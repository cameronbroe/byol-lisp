libs_for_unix = -ledit
libs_for_windows =
libs =

ifeq ($(OS),Windows_NT)
	libs += $(libs_for_windows)
else
	libs += $(libs_for_unix)
endif

all:
	$(CC) -o byol-lisp main.c $(libs)
