CC := g++
CFLAGS := -c -O2 -std=c++11 -Iexternal/SOIL -Iexternal/vsl
LDFLAGS := -lGL -lGLU -lglut -lGLEW
TARGET := water
SRCDIR := ./src
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(addprefix build/,$(notdir $(SRCS:.cpp=.o)))
SOILCS := $(wildcard ./external/SOIL/*.c)
SOILOBJS := $(addprefix build/,$(notdir $(SOILCS:.c=.o)))
VSCPPS := $(wildcard ./external/vsl/*.cpp)
VSOBJS :=  $(addprefix build/,$(notdir $(VSCPPS:.cpp=.o)))

all:$(TARGET)

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) $< -o $@

build/%.o: external/SOIL/%.c
	$(CC) $(CFLAGS) $< -o $@

build/%.o: external/vsl/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(TARGET):$(OBJS) $(VSOBJS) $(SOILOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)



clean:
        rm -rf $(TARGET) build/*.o
