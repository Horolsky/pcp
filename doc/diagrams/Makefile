PLANTUML 	:= plantuml
CFG_F		:= "global.skin"
ROOT_DIR	:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

SRC_D 		:= $(ROOT_DIR)/src
OUT_D		:= $(ROOT_DIR)/img

.PHONY: all list 

all:
	$(PLANTUML) -config $(CFG_F) plantuml $(SRC_D)/* -o $(OUT_D)
