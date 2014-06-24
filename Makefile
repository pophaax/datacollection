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

SOURCES_DB = DBHandler.cpp
HEADERS_DB = DBHandler.h
FILE_DB = DBHandler.o

SOURCES_JSON = JSON.cpp
HEADERS_JSON = JSON.h
FILE_JSON = JSON.o

HEADERS = $(HEADERS_JSON) $(HEADERS_DB)
SOURCES = $(SOURCES_JSON) $(SOURCES_DB)

all : $(FILE_JSON) $(FILE_DB)

$(FILE_JSON) : $(SOURCES_JSON) $(HEADERS_JSON)
	$(CC) $(SOURCES_JSON) $(FLAGS) $(LIBS) -c -o $(FILE_JSON)

$(FILE_DB) : $(SOURCES_DB) $(HEADERS_DB)
	$(CC) $(SOURCES_DB) $(FLAGS) $(LIBS) -c -o $(FILE_DB)

example : $(SOURCES) $(HEADERS) example.cpp
	$(CC) $(SOURCES) example.cpp $(FLAGS) $(LIBS) -o example

