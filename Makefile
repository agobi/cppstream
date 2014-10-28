CXXFLAGS+=-std=c++11 -W -Wall -pedantic -Wno-uninitialized -g
CPPFLAGS?=-O3 -fomit-frame-pointer

TESTS=test_function\
	  test_override\
	  test_fibonacci\
	  test_change\
	  test_literals\
	stream2

all: $(TESTS)
$(TESTS): % : %.o
	$(CXX) $(LDFLAGS) -o $@ $<
$(TESTS:=.o): %.o : test_common.h stream.h

clean:
	$(RM) $(TESTS) $(TESTS:=.o)

