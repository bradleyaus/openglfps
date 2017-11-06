PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW 
DEFS = `pkg-config --cflags glfw3`

REQPKG=freetype2
REQPKG:=$(shell pkg-config --exists $(REQPKG) && echo '$(REQPKG)')
ifneq ($(REQPKG),)
    FT_LIBS = `pkg-config --libs freetype2`
    FT_DEFS = `pkg-config --cflags freetype2` -DUSEFREETYPE
else
    $(shell echo 'Warning: Freetype not found, font's wont be displyed')
endif

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
    DEFS =-DWIN32
endif

CC:=g++ -std=c++0x $(DEFS) $(FT_DEFS)
FLAGS:=-c
SRCDIR:=./
BUILDDIR:=./
EXT:=cpp
OEXT:=o
SOURCES:=$(shell find $(SRCDIR) -type f -name "*.$(EXT)")
OBJECTS:=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(EXT)=.$(OEXT)))
EXE:=./assign3part2

all: $(OBJECTS) $(EXE) 

$(OBJECTS): | $(BUILDDIR) 
$(BUILDDIR):
		mkdir $(BUILDDIR) 

$(BUILDDIR)/%.$(OEXT) : $(SRCDIR)/%.$(EXT)
		$(CC) $(FLAGS) $< -o $@

$(EXE): $(OBJECTS)
		$(CC) $(OBJECTS) -o $(EXE) $(GL_LIBS) $(FT_LIBS)
		
clean:
		-rm -rf $(BUILDDIR)/*.$(OEXT) $(EXE)
.PHONY: clean
