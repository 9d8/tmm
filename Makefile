CC=gcc
TARGET=tmm

#Directories
BUILDDIR=build
INCLUDEDIR=src
SRCDIR=src

CFLAGS=
LIB=
INCLUDE=-I$(INCLUDEDIR)

############################################

OBJECTS:=$(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(wildcard src/*.c))
DEPS:=$(patsubst $(INCLUDEDIR)/%.c,$(INCLUDEDIR)/%.o,$(wildcard src/*.h))

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDE)

$(TARGET): mkdir $(OBJECTS)
	$(CC) -o $(BUILDDIR)/$@ $(OBJECTS) $(CFLAGS) $(INCLUDE) $(LIB)

debug: CFLAGS=-ggdb 
debug: $(TARGET)

.PHONY: clean mkdir
clean:
	-rm $(BUILDDIR)/$(TARGET) $(OBJECTS)

mkdir:
	-mkdir $(BUILDDIR)

