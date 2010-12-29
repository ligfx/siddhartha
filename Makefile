override CFLAGS := -fvisibility=hidden -fpic -ansi -Wall -Wno-comment -Werror -Wno-error=unused-variable -g $(CFLAGS)

all: libsiddartha.so include test

include: siddartha.h
	@echo " CP $^ => include/siddartha/"
	@mkdir -p include/siddartha
	@cp $^ include/siddartha

test: test.o libsiddartha.so
	@echo " LD $^ => $@"
	@${CXX} $^ -L. -lsiddartha -lgtest -lgtest_main -o $@ -Wl,-rpath,.

libsiddartha.so: siddartha.o
	@echo " LD $^ => $@"
	@${CXX} -shared $^ -o $@

%.o: %.c
	@echo " CC $^ => $@"
	@${CC} -c $^ ${CFLAGS} -std=c99

%.o: %.cpp
	@echo " CC $^ => $@"
	@${CXX} -c $^ ${CFLAGS}

clean:
	-rm main libsiddartha.so test
	-rm -frv *.o
	-rm *~
	-rm -frv include
