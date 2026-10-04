CXXFLAGS = -Wall -O2 -std=c++17 -Wno-deprecated-declarations
ifeq ($(shell uname),Darwin)
LDLIBS = -framework GLUT -framework OpenGL
else
LDLIBS = -lglut -lGLU -lGL
endif
BINS = dda bresenham circle transform liang_barsky

all: $(BINS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

# checks the same algorithms as the browser demo (web/cg.js)
test:
	node web/test.cjs

clean:
	rm -f $(BINS)

.PHONY: all test clean
