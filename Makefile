#######################################################
#
#    Aland Sailing Robot
#    ===========================================
#    dbhandler
#    -------------------------------------------
#
#######################################################

CC = g++
FLAGS = -Wall -pedantic -Werror
LIBS = -lsqlite3

SOURCES = DBHandler.cpp
HEADERS = DBHandler.h
FILE = DBHandler.o



all : $(FILE)
$(FILE) : $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) $(FLAGS) $(LIBS) -c -o $(FILE)

example : $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) example.cpp $(FLAGS) $(LIBS) -o example

