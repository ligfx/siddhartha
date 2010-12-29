override CFLAGS := -fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable -g $(CFLAGS)

all: libsiddhartha.so include test

include: siddhartha.h
	@echo " CP $^ => include/siddhartha/"
	@mkdir -p include/siddhartha
	@cp $^ include/siddhartha

test: test.o libsiddhartha.so
	@echo " LD $^ => $@"
	@${CXX} $^ -L. -lsiddhartha -lgtest -lgtest_main -o $@ -Wl,-rpath,.

libsiddhartha.so: siddhartha.o
	@echo " LD $^ => $@"
	@${CXX} -shared $^ -o $@

%.o: %.c
	@echo " CC $^ => $@"
	@${CC} -c $^ ${CFLAGS} -std=c99

%.o: %.cpp
	@echo " CC $^ => $@"
	@${CXX} -c $^ ${CFLAGS}

clean:
	-rm main libsiddhartha.so test
	-rm -frv *.o
	-rm *~
	-rm -frv include
