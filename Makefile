#   ============================================================================
#   Youhua Xu Dec-13-2017
#   ============================================================================
#   Make a build dir for compilation
MDIR := $(shell pwd)
WRKDIR = $(MDIR)/build
BINDIR = $(MDIR)/bin

#	make build & binary dirs
.base:
	if ! [ -e $(WRKDIR) ]; then mkdir $(WRKDIR) ; fi;
	touch $(WRKDIR)/.base;
	if ! [ -e $(BINDIR) ]; then mkdir $(BINDIR) ; fi;
	touch $(BINDIR)/.base;

#   ============================================================================
#   Set the source file path
vpath %.cpp main:src
vpath %.cc  main:src
vpath %.c   main:src
vpath %.hpp include
vpath %.h   include
vpath %.o build
vpath .base build

INCLUDES = -I $(MDIR)/include

CPP			= gcc
CC          = mpicc
CCFLAG  	= -Wall -DHAVE_INLINE -D_FULL_DEBUG_
CCFLAG += -D_USE_SWITCH_
# CCFLAG += -pg
CCFLAG	+= -D_ENABLE_BETA_ANGLE_

OPTFLAG		= -O3 #-ffast-math #( not recognized by intel compiler )
# OPTFLAG += -pg

LDFLAG      =
#   http://www.tuicool.com/articles/NBfeYj
ifeq ($(shell uname -s),Linux)
	LDFLAG	+= -Wl,--no-as-needed
endif
#LIBS		= -limcmc
#LIBS		+= -larmadillo -lblas -llapack
#LIBS        += -lgsl -lgslcblas

LIBS	= -lgsl -lgslcblas -lm

ifeq ($(shell uname -s),Darwin)
	LDFLAG	+= -framework Accelerate #(-framework Accelerate is for Mac OSX)
endif

%.o: %.cpp .base
	cd $(WRKDIR);$(CC) $(OMPFLAG) $(OPTFLAG) $(CCFLAG) $(INCLUDES) -c ../$< -o $*.o

%.o: %.cc .base
	cd $(WRKDIR);$(CC) $(OMPFLAG) $(OPTFLAG) $(CCFLAG) $(INCLUDES) -c ../$< -o $*.o

%.o: %.c .base
	cd $(WRKDIR);$(CC) $(OMPFLAG) $(OPTFLAG) $(CCFLAG) $(INCLUDES) -c ../$< -o $*.o

VERBOSE = Verbose.o

SURVEY_SIM_MPI	= SurveySim_MPI.o
GET_BETA_TIME   = get_beta_time.o
ADJ_BETA_TIME	= adjust_beta.o
CHECK_ROTATION	= check_rotation.o

DEPS = LocateSatellite.o transformTools.o SkyAreaSplit.o SurveyConditionLimit.o \
        Parser.o CMG.o CoverAreaStat.o SearchNewPoints.o rotation.o 
EPHCMP = gnulliver.o ephcom.o

all:SurveySim GetTime adjust_beta check_rotation

SurveySim:${SURVEY_SIM_MPI} ${EPHCMP} ${DEPS}
	${CC} ${OPTFLAG} ${LDFLAG} $(addprefix build/,$(notdir $^)) ${LIBS} -o $(BINDIR)/$@

GetTime:${GET_BETA_TIME} ${EPHCMP} ${DEPS}
	${CC} ${OPTFLAG} ${LDFLAG} $(addprefix build/,$(notdir $^)) ${LIBS} -o $(BINDIR)/$@

adjust_beta:${ADJ_BETA_TIME} ${EPHCMP} ${DEPS}
	${CC} ${OPTFLAG} ${LDFLAG} $(addprefix build/,$(notdir $^)) ${LIBS} -o $(BINDIR)/$@

check_rotation:${CHECK_ROTATION}  ${EPHCMP}  ${DEPS}
	${CC} ${OPTFLAG} ${LDFLAG} $(addprefix build/,$(notdir $^)) ${LIBS} -o $(BINDIR)/$@

#   ================================================================================================
.PHONY:clean tidy run
clean: .base
	rm -rf $(WRKDIR);
tidy:
	make clean; rm -rf $(BINDIR);
