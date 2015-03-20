EXE=Carrefour
INT=GMenu.h

SRC=$(INT:.h=.cpp)
OBJ=$(INT:.h=.o)

MainSRC=main.cpp
MainOBJ=$(MainSRC:.cpp=.o)

EXEPATH=./
EXTERNINTERFACEPATH=/share/public/tp/tp-multitache/
EXTERNLIBPATH=/share/public/tp/tp-multitache/
LIB=-ltp -ltcl -lncurses

ECHO=@echo

CLEAN=clean
RM=rm
RMFlags=-f

BACKUP=backup
CP=cp
BACKUPPATH=/share/public/tp/tp-multitache/Compte-Rendu/2015/3/B3325/


Compilator=@g++
CompilationsOptions=-Wall -pedantic -std=c++11
Linker=@g++
LinkerOptions=

all: $(EXE)

$(EXE): $(MainOBJ) $(OBJ)
	$(ECHO) "Edition des liens de $(EXE)"
	$(Linker) -o $(EXEPATH)$(EXE) $(MainOBJ) $(OBJ) -L$(EXTERNLIBPATH) $(LIB) $(LinkerOptions)
	
$(MainOBJ): $(MainSRC) $(INT)
	$(ECHO) "Compilation de $(MainSRC)"
	$(Compilator) -o $(MainOBJ)  $(CompilationsOptions) -I$(EXTERNINTERFACEPATH) -c $(MainSRC)

%.o: %.cpp
	$(ECHO) "Compilation de $<"
	$(Compilator) $(CompilationsOptions) -I$(EXTERNINTERFACEPATH) -c $< 
	
# ***** CLEAN ***** #

.PHONY: $(CLEAN)
$(CLEAN):
	$(ECHO) "Nettoyage local"
	$(RM) $(RMFlags) $(EXEPATH)$(EXE) $(OBJ) $(MainOBJ)
	
	
# ***** BACKUP ***** #

.PHONY: $(BACKUP)
$(BACKUP):
	$(ECHO) "Nettoyage du backup"
	$(RM) $(RMFlags) $(BACKUPPATH)*.cpp $(BACKUPPATH)*.h $(BACKUPPATH)Makefile
	$(ECHO) "Copie des fichiers"
	$(CP) *.cpp $(BACKUPPATH)
	$(CP) *.h $(BACKUPPATH)
	$(CP) Makefile $(BACKUPPATH)
	make EXE=BackUpCarrefour
