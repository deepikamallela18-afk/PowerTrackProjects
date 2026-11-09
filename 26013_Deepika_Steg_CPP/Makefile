CXX      ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

stego: test_encode.cpp encode.cpp decode.cpp encode.hpp decode.hpp types.hpp common.hpp
	$(CXX) $(CXXFLAGS) -o $@ test_encode.cpp encode.cpp decode.cpp

clean:
	rm -f stego

.PHONY: clean
