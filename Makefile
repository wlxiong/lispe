################################################################
include Makefile.in
################################################################
COMPPLUSPLUS = g++
################ Compiler LispE #################################
SOURCE = lispe.cxx jagget.cxx eval.cxx elements.cxx tools.cxx systems.cxx maths.cxx strings.cxx randoms.cxx rgx.cxx sockets.cxx composing.cxx ontology.cxx sets.cxx lists.cxx dictionaries.cxx
SOURCEMAIN = jag.cxx main.cxx lispeditor.cxx
SOURCEJAG = jagmain.cxx jag.cxx jagget.cxx jagrgx.cxx jagtools.cxx
#------------------------------------------------------------
OBJECTCXX = $(SOURCE:%.cxx=objs/%.o)
OBJECTMAIN = $(SOURCEMAIN:%.cxx=objs/%.o)
OBJECTJAG = $(SOURCEJAG:%.cxx=objs/jag/%.o)
#------------------------------------------------------------
INCLUDES = -Iinclude
# Selon les compilateurs, il faudra peut-être choisir l'un des flags suivants
#C++11Flag = -std=c++0x -w -c -fPIC $(COPTION) -DUNIX $(VTERM_MOUSE)
#C++11Flag = -std=gnu++0x -w -c -fPIC $(COPTION) -DUNIX $(VTERM_MOUSE)

C++11Flag = -fPIC -std=c++11 -w -c $(COPTION) $(REGEX) -DUNIX $(VTERM_MOUSE)
#------------------------------------------------------------
objs/%.o: src/%.cxx
	$(COMPPLUSPLUS) $(C++11Flag) $(INCLUDES) $< -o $@

objs/jag/%.o: src/%.cxx
	$(COMPPLUSPLUS) $(C++11Flag) $(INCLUDES) $< -o $@
#------------------------------------------------------------

# For those who prefer a small executable linked with a dynamic library
lispe: $(OBJECTCXX) $(OBJECTMAIN)
	$(COMPPLUSPLUS) -o bin/lispe $(OBJECTCXX) $(OBJECTMAIN) -ldl -lpthread $(LIBBOOST)

jag: install $(OBJECTJAG)
	$(COMPPLUSPLUS) -o bin/jag $(OBJECTJAG) $(LIBBOOST)

# Version dynamique. Elle est un peu plus compliquée à manipuler:
# il faut initialiser LD_LIBRARY_PATH (ou DYLD_LIBRARY_PATH sur Mac OS)
# L'avantage c'est que les libs sont plus petites
#liblispe: $(OBJECTCXX)
#	$(COMPPLUSPLUS) -shared -o bin/liblispe.so $(OBJECTCXX)

# Version statique de la bilbilothèque
liblispe: $(OBJECTCXX)
	ar rcs bin/liblispe.a $(OBJECTCXX)
	ranlib bin/liblispe.a

all: install lispe liblispe
	$(liblispe)
	$(lispe)

install:
	mkdir -p bin
	mkdir -p objs
	mkdir -p objs/jag

clean:
	rm -Rf objs
	rm -Rf bin

libs:
	$(MAKE) -C curl clean all
	$(MAKE) -C xml clean all
	$(MAKE) -C sqlite clean all
	$(MAKE) -C pythonlispe clean all
	$(MAKE) -C transducer clean all
	$(MAKE) -C gui clean all

