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

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDE) 

$(TARGET): mkdir $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $(OBJECTS) $(INCLUDE) $(LIB)

.PHONY: debug clean mkdir

debug: CFLAGS:=$(CFLAGS)-ggdb
debug: $(TARGET)

clean:
	-rm $(BUILDDIR)/$(TARGET) $(OBJECTS) 2>/dev/null

mkdir:
	@-mkdir $(BUILDDIR) || true
