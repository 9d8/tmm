CC=gcc
TARGET=tmm

# Makefile automatically builds for whatever OS it's being run on. It can be
# explicitly set by uncommenting one of the lines below. 
#OS=Windows_NT
#OS=Linux

#Directories
BUILDDIR=build
INCLUDEDIRS=src
SRCDIR=src

CFLAGS=
LIBS=

# Windows settings. These do nothing when targeting other OS'.
WINDRES=windres
RCDIR=manifest

############################################

OBJECTS=$(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(wildcard $(SRCDIR)/*.c))
INCLUDE=$(patsubst %,-I%,$(INCLUDEDIRS))
LIB=$(patsubst %,-L%,$(LIBS))

ifeq ($(OS),Windows_NT)
OBJECTS:=$(OBJECTS) $(patsubst $(RCDIR)/%.rc,$(BUILDDIR)/%.o,$(wildcard $(RCDIR)/*.rc)) 
$(BUILDDIR)/%.o: $(RCDIR)/%.rc
	$(WINDRES) $< -o $@
endif

all: $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDE) 

# Make build directory when trying to create an object file.
$(OBJECTS): | $(BUILDDIR)  

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $(OBJECTS) $(INCLUDE) $(LIB)

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: debug clean install uninstall

debug: CFLAGS:=$(CFLAGS)-ggdb
debug: $(TARGET)

clean:
	-rm $(BUILDDIR)/$(TARGET) $(OBJECTS)

install: $(TARGET)
	cp $(BUILDDIR)/$(TARGET) /usr/bin/

uninstall:
	rm /usr/bin/$(TARGET)
