# 编译器：本机 C:\MINI-ATE\mingw64 里的 cc1plus.exe 已损坏（运行即段错误），
# 所以这里改用可用的 D:\mingw64。若你机器上自带工具链正常，可改回 CXX = g++
CXX = D:/mingw64/bin/g++
RUNNER_HOME = c:/MINI-ATE/runner
CFGOPT = -shared -Wall -Wl,-subsystem,windows -mthreads -fPIC
LIBS = -lsetupapi -lkernel32 -luser32 -lgdi32 -lwinspool -llibCommon -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 \
		-L$(RUNNER_HOME) -lproject -lTester -lBinDef -lUsrApi -lWaveCalc -lShmoo
INCS = -I./ -I$(RUNNER_HOME)/inc/ \

SRC = Test.cpp \

TESTLIB = Test.dll
ifeq ($(RUNNER_HOME),)
$(error RUNNER_HOME not set in environment)
endif
all: | $(RUNNER_HOME)
	$(CXX) -g -std=c++17 $(SRC) $(INCS) $(LIBS) $(CFGOPT) -DINTERFACE_EXPORTS -o $(TESTLIB)
clean: $(TESTLIB)
	$(RM) $(TESTLIB)
$(RUNNER_HOME):
	$(error Folder $(RUNNER_HOME) does not exist)
