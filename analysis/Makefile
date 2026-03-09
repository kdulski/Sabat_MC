CC = g++

CFLAGS = -Wall `root-config --cflags`

CLIBS = `root-config --glibs` `root-config --cflags`

all: SabAna SabEneFit Comp

SabAna: Sabat_Analysis_new.C Histo_Collection.h
	$(CC) Sabat_Analysis_new.C -o SabAna.exe $(CLIBS) -std=c++17

SabEneFit: Sabat_Energy_Fit.C
	$(CC) Sabat_Energy_Fit.C -o SabEneFit.exe $(CLIBS) -std=c++17

Comp: CompareIt.cpp
	$(CC) CompareIt.cpp -o Compare.exe $(CLIBS) -std=c++17

clean: 
	rm -rf *o

