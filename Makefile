CC=gcc
TARGET=tmm

#Directories
BUILDDIR=build
INCLUDEDIRS=src
SRCDIR=src

CFLAGS=
LIBS=

############################################

OBJECTS=$(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(wildcard $(SRCDIR)/*.c))
INCLUDE=$(patsubst %,-I%,$(INCLUDEDIRS))
LIB=$(patsubst %,-L%,$(LIBS))

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
	-rm $(BUILDDIR)/$(TARGET) $(OBJECTS) 2>/dev/null

install: $(TARGET)
	cp $(BUILDDIR)/$(TARGET) /usr/bin/

uninstall:
	rm /usr/bin/$(TARGET)
