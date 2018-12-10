TARGET := CGProjectBrentMaas
SRC := src
LIBSRC := libsrc
BUILD := build
CXXFLAGS := -I$(SRC) -I$(LIBSRC) -g -std=c++17 -Wall -O3
LDFLAGS := 

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lopengl32 -mwindows -lglfw3
else
    LDFLAGS += -lGL -Llib -lglfw3 -lrt -lm -ldl -pthread -lX11 -lXinerama -lXi -lXxf86vm -lXcursor
endif

rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard, $d/, $2) $(filter $(subst *, %, $2), $d))

SRCS := $(patsubst $(SRC)/%, %, $(call rwildcard, $(SRC)/, *.cpp)) $(patsubst $(LIBSRC)/%, %, $(call rwildcard, $(LIBSRC)/, *.cpp))
OBJECTS := $(SRCS:%.cpp=%.o)

TOTAL := $(words $(OBJECTS) .)
progress = $(or $(eval PROCESSED := $(PROCESSED) .),$(info [$(words $(PROCESSED))/$(TOTAL)] $1))

vpath %.o $(BUILD)/objects
vpath %.cpp $(SRC) $(LIBSRC)

all: $(TARGET)
	@echo Done!

$(TARGET): $(OBJECTS)
	@$(call progress,Linking $@)
	@$(CXX) -o $@ $(OBJECTS:%=$(BUILD)/objects/%) $(LDFLAGS)

%.o: %.cpp
	@$(call progress,Compiling $<)
	@mkdir -p $(BUILD)/objects/$(dir $@)
	@$(CXX) -c $(CXXFLAGS) -o $(BUILD)/objects/$@ $<

clean:
	@echo Cleaning build files
	@rm -rf $(BUILD) $(TARGET)
	
run: all
	@./$(TARGET)

rundebug: all
	@./$(TARGET) debug

.PHONY: clean
