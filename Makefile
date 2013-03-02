CC = cc
CFLAGS = -g
targets = hw2
src = hw2.c
objs = $(patsubst %.c,%.o,$(src))

#Naming our phony Targets
.PHONY: all clean hw2

all: $(targets)

hw2: $(objs)
	$(CC) $(CFLAGS) -o hw2 $(objs)

$(objs) : $(src)

clean:
	rm $(objs)  $(targets)