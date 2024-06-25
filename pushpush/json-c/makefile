CC  = gcc   
INC  = -I.   
LIBS = -ljson-c   
CFLAGS = -O2 -g -Wall ${INC}   
   
json : json.o   
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)   
   
clean :  
	rm -f ./json  
.SUFFIXES: .o  
  
.c.o :  
	$(CC) $(CFLAGS) -c $<
