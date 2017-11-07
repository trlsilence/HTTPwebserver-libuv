CC = g++
PROTO = protoc
SOURCE = ../transport_proto/transport_proto.proto  
TARGET = main
INTERFLAG = -I=./
RESULT = --cpp_out=./ 
SOURCE1 = main.cpp 
SOURCE2 = ../transport_proto/transport_proto_pb.cc
SOURCE3 = ../transport_proto/all_proto.cpp
SOURCE4 = ClientIDtohandler.h			
CFALGS = -Wall -g -I=./ -I/usr/local/include  
LDFLAGS = -L/usr/local/lib
LIB0 = -luv
LIB1 = -I/usr/local/protobuf/include 
LIB2 = -L/usr/local/protobuf/lib 
LIB3 = -lprotobuf  
#OBJ = person.o person.pb.o  
  
main: $(SOURCE)
	$(PROTO) $(INTERFLAG) $(RESULT) $(SOURCE)
main.o:  
	$(CC) $(SOURCE1) $(SOURCE2) $(SOURCE3) $(SOURCE4) -o $(TARGET) $(LIB0) $(LIB1) $(LIB2) $(LIB3) 
clean:  
	rm -fr *.o $(TARGET) 
