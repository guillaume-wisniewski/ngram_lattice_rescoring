OS=$(shell uname)
PREFIX=`pwd`

BOOST_DIR=$(PREFIX)
BOOST_LIB_DIR=$(BOOST_DIR)/stage/lib

CXXFLAGS=-DBOOST_LOG_DYN_LINK -std=c++11 -Iinclude -I$(BOOST_DIR) -Wfatal-errors -g
LDLIBS=-ldl -lm -lpthread -lfst -lfstfar -lngram -lboost_program_options
LDFLAGS=-DBOOST_LOG_DYN_LINK -L$(BOOST_LIB_DIR) -Llib 

ifeq ($(OS), Linux)
	LDFLAGS += -Wl,-rpath=`pwd`/lib -Wl,-rpath=$(BOOST_LIB_DIR)
endif

all: lattice_rescoring

lattice_rescoring: lattice_rescoring_cli.o jsoncpp.o lattice_rescoring.o
	g++ $(LDFLAGS) -o $@ lattice_rescoring_cli.o jsoncpp.o lattice_rescoring.o $(LDLIBS)

clean:
	rm -f *.o
	rm -f lattice_rescoring
	rm -f rescoring_test

test: rescoring_test

rescoring_test: test_lattice_rescoring.o lattice_rescoring.o jsoncpp.o
	g++ $(LDFLAGS) -o $@ test_lattice_rescoring.o lattice_rescoring.o -lboost_unit_test_framework $(LDLIBS)
	./rescoring_test

