
#
# Minimal makefile for the adf2mp3 tool.
# Tuned from a C++11 compatible compiler.
#

COMPILER = c++
CPPFLAGS = -Wall -Wextra -Weffc++ -std=c++11 -O3
FILES    = adf2mp3.cpp
OUTPUT   = adf2mp3

all:
	$(COMPILER) $(CPPFLAGS) $(FILES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)

