ifeq ($(PLATFORM), Windows)
	CFLAGS = -O3 -g
	LDFLAGS = -static-libgcc -static-libstdc++ -static -Wall -Iexternal -DPLATFORM_DESKTOP -lopengl32 -lraylib -lgdi32 -lwinmm -DWindows -lpthread
	COMP = wine C:/raylib/mingw/bin/g++.exe
	BUILD_DIR = Build

else
    # Linux build settings
	CFLAGS = -O2 --std=c++20 -g
    LDFLAGS = -LSource/External -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DLinux 
    COMP = g++
    BUILD_DIR = Build
endif

# Create the Build directory if it doesn't exist
$(shell mkdir -p $(BUILD_DIR))

OBJS = $(BUILD_DIR)/raygui.o \
  $(BUILD_DIR)/cJSON.o \
  $(BUILD_DIR)/Lumina.o \
  $(BUILD_DIR)/Encoder.o \
  $(BUILD_DIR)/PPGIO.o \
  $(BUILD_DIR)/SigCheck.o \
  $(BUILD_DIR)/Language.o \
  $(BUILD_DIR)/Main.o 

COREHEADERS = Source/Core.hpp \
  $(BUILD_DIR)/Encoder.o \
  $(BUILD_DIR)/Main.o

all: $(BUILD_DIR)/OTENGINE

$(BUILD_DIR)/OTENGINE: $(OBJS)
	$(COMP) $(OBJS) -o OTENGINE $(CFLAGS) $(LDFLAGS)

$(BUILD_DIR)/Main.o: Source/Main.cpp Source/*.hpp Source/libPPG/Parasite/*.hpp Source/libPPG/Language/*.hpp Source/CineFlow/*.hpp Source/Lumina/*.hpp
	$(COMP) $(CFLAGS) -c Source/Main.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/PPGIO.o: Source/libPPG/PPGIO/PPGIO.cpp
	$(COMP) $(CFLAGS) -c Source/libPPG/PPGIO/PPGIO.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/Language.o: Source/libPPG/Language/Language.cpp
	$(COMP) $(CFLAGS) -c Source/libPPG/Language/Language.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/Encoder.o: Source/libPPG/Encoder/Encoder.cpp
	$(COMP) $(CFLAGS) -c Source/libPPG/Encoder/Encoder.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/Lumina.o: Source/Lumina/Lumina.cpp
	$(COMP) $(CFLAGS) -c Source/Lumina/Lumina.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/cJSON.o: Source/Lumina/Json/cJSON.c
	$(COMP) $(CFLAGS) -c Source/Lumina/Json/cJSON.c -o $@ $(LDFLAGS)

$(BUILD_DIR)/SigCheck.o: Source/libPPG/Security/SigCheck.cpp
	$(COMP) $(CFLAGS) -c Source/libPPG/Security/SigCheck.cpp -o $@ $(LDFLAGS)

$(BUILD_DIR)/raygui.o: Source/External/raygui/raygui.c
	$(COMP) $(CFLAGS) -c Source/External/raygui/raygui.c -o $@ -shared -DRAYGUI_IMPLEMENTATION -DBUILD_LIBTYPE_SHARED -static-libgcc -lopengl32 -lgdi32 -lwinmm -lraylib

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o OTENGINE

# Target to run clang-format on all C++ files
.PHONY: format
format:
	clang-format -i Source/*.cpp Source/*.hpp  Source/libPPG/PPGIO/*.cpp Source/libPPG/PPGIO/*.hpp Source/libPPG/Language/*.cpp Source/CineFlow/*.hpp Source/libPPG/Security/*.cpp 
