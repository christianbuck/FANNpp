CXXFLAGS =	-O3 -g -Wall -fmessage-length=0
LDFLAGS =  -static -lstdc++ -largtable2 -lfann

OBJS =		fann++.o

TARGET =	fann++

$(TARGET):	$(OBJS)
	$(CXX)  $(CXXFLAGS) ${OBJS} $(LIBS) ${LDFLAGS} -o $(TARGET)

%.o : %.cpp *.h Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@  


all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
