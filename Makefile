# Attempt to load a config.make file.
# If none is found, project defaults in config.project.make will be used.
ifneq ($(wildcard config.make),)
	include config.make
endif

# make sure the the OF_ROOT location is defined
ifndef OF_ROOT
    OF_ROOT=../../..
endif

# vscode - make sure all the paths are absolute so the problem matcher can handle them.
# TODO: can we detect vscode from the env?
override OF_ROOT := $(realpath $(OF_ROOT))

# call the project makefile!
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk
