APP=cpp_samples

SRC_DIR = ./
UNIGINE_DIR = ../
INCLUDE_DIR = $(UNIGINE_DIR)include
TARGET_DIR = $(SRC_DIR)/../bin
LIBPATH = $(UNIGINE_DIR)bin

CC = g++
LD = g++
FLAGS =
CFLAGS = -std=c++14 $(FLAGS) -msse -msse2 -msse4.1 -Wall
LDFLAGS = -L$(LIBPATH) $(FLAGS) -Wl,-rpath,'$$ORIGIN',-no-as-needed
INCFLAGS = -I$(INCLUDE_DIR)

UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
	ARCH = x64
	FLAGS += -m64
else
	ARCH = x86
	FLAGS += -m32
endif

CFLAGS += -D_LINUX

ifeq ($(debug),1)
	DEBUG_POSTFIX = d
	CFLAGS += -g -DDEBUG
else
	DEBUG_POSTFIX=
	CFLAGS += -O2 -DNDEBUG
endif

ifeq ($(double),1)
	DOUBLE_POSTFIX = _double
	CFLAGS+= -DUNIGINE_DOUBLE
else
	DOUBLE_POSTFIX =
endif
UNIGINE_LIB_NAME=Unigine$(DOUBLE_POSTFIX)
TARGET := $(TARGET_DIR)/$(APP)$(DOUBLE_POSTFIX)_$(ARCH)$(DEBUG_POSTFIX)
SOURCES := $(shell find . -name "*.cpp")
OBJ_DIR := $(ARCH)$(DEBUG_POSTFIX)$(DOUBLE_POSTFIX)
OBJECTS := $(addprefix $(OBJ_DIR)/, $(patsubst %.cpp,%.o,$(SOURCES)))
LDFLAGS += -l$(UNIGINE_LIB_NAME)_$(ARCH)$(DEBUG_POSTFIX)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJECTS): $(OBJ_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) *.o $(TARGET)
