ifeq ($(PLATFORM), Windows)
	CFLAGS = -O3 -g
	LDFLAGS = -static-libgcc -static-libstdc++ -static -Wall -Iexternal -DPLATFORM_DESKTOP -lopengl32 -lraylib -lgdi32 -lwinmm -lpthread
	COMP = g++.exe
	BUILD_DIR = Build
else
    # Linux build settings
	CFLAGS = -O2 --std=c++20 -g
    LDFLAGS = -ISource/External/raygui/ -LSource/External -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    COMP = g++
    BUILD_DIR = Build
endif

# Create the Build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJS = $(BUILD_DIR)/Main.o \
	$(BUILD_DIR)/PPGIO.o \
	$(BUILD_DIR)/raygui.o

COREHEADERS = $(BUILD_DIR)/Main.o

all: $(BUILD_DIR)/OTENGINE

$(BUILD_DIR)/OTENGINE: $(OBJS)
	$(COMP) $(OBJS) -o OTENGINE $(CFLAGS) $(LDFLAGS)

$(BUILD_DIR)/Main.o: Source/Main.cpp  Source/*.hpp
	$(COMP) $(CFLAGS) -c Source/Main.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/raygui.o: Source/External/raygui/raygui.c
	$(COMP) $(CFLAGS) -c Source/External/raygui/raygui.c -o $@ -shared -DRAYGUI_IMPLEMENTATION -DBUILD_LIBTYPE_SHARED -static-libgcc -lopengl32 -lgdi32 -lwinmm -lraylib

$(BUILD_DIR)/PPGIO.o: Source/PPGIO/PPGIO.cpp
	$(COMP) $(CFLAGS) -c Source/PPGIO/PPGIO.cpp -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o OTENGINE

# Target to run clang-format on all C++ files
.PHONY: format
format:
	clang-format -i Source/*.cpp Source/*.hpp 
