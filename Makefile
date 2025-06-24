CC      = gcc
CFLAGS  = -I.
LIBS    = -lm
OBJECTS = unwaver.o argument.o sequence.o utils.o
HEADERS = preproc.h global.h prototype.h

.PHONY : default

default: unwaver

unwaver.o : unwaver.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

argument.o : argument.c preproc.h global.h
	$(CC) -c -o $@ $< $(CFLAGS)

sequence.o : sequence.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

utils.o : utils.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

unwaver : $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY : clean

clean :
	rm unwaver $(OBJECTS)


