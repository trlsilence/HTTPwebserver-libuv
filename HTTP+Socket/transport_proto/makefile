CC = g++
PROTO = protoc
SOURCE = transport_proto.proto  
TARGET = transport_proto
INTERFLAG = -I=./
RESULT = --cpp_out=./ 
SOURCE1 = testproto.cpp 
SOURCE2 = transport_proto.pb.cc
SOURCE3 = all_proto.cpp			
CFALGS = -Wall -g -I=./ -I/usr/local/include  
LDFLAGS = -L/usr/local/lib
LIB1 = -I/usr/local/protobuf/include 
LIB2 = -L/usr/local/protobuf/lib 
LIB3 = -lprotobuf  
#OBJ = person.o person.pb.o  
  
transport_proto: $(SOURCE)
	$(PROTO) $(INTERFLAG) $(RESULT) $(SOURCE)
transport_proto.o:  
	$(CC) $(SOURCE1) $(SOURCE2) $(SOURCE3) -o $(TARGET) $(LIB1) $(LIB2) $(LIB3) 
clean:  
	rm -fr *.o $(TARGET) 
