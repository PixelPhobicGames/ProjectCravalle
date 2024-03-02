
ifeq ($(PLATFORM), Windows)

	CFLAGS = -O3 --std=c++20
	LDFLAGS =  -static -Wall -Iexternal -DPLATFORM_DESKTOP -lopengl32 -lraylib -lgdi32 -lwinmm  -DWindows
	COMP = g++.exe
	BUILD_DIR = Build

else
	CFLAGS = -O3 --std=c++20
	LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DLinux
	COMP = g++
	BUILD_DIR = Build
endif


# Create the Build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJS = $(BUILD_DIR)/raygui.o \
  $(BUILD_DIR)/Main.o

COREHEADERS = $(BUILD_DIR)/Main.o

all: $(BUILD_DIR)/OTENGINE

$(BUILD_DIR)/OTENGINE: $(OBJS)
	$(COMP) $(OBJS) -o OTE $(CFLAGS) $(LDFLAGS)

$(BUILD_DIR)/Main.o: Source/Main.cpp Source/*.hpp 
	$(COMP) $(CFLAGS) -c Source/Main.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/raygui.o: Source/raygui/raygui.c
	$(COMP) $(CFLAGS) -c Source/raygui/raygui.c -o $@ -shared -DRAYGUI_IMPLEMENTATION -DBUILD_LIBTYPE_SHARED -static-libgcc -lopengl32 -lgdi32 -lwinmm -lraylib


.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/OTENGINE
