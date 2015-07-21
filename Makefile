
all: libc 
LDFLAGS = -L. 
libc: src/overload.cpp src/fd.cpp src/fd.hpp src/registerEnv.cpp src/interface.cpp
	g++ -c -g -fpic src/overload.cpp src/overload.hpp src/fd.cpp src/fd.hpp src/registerEnv.cpp src/registerEnv.hpp src/interface.cpp src/interface.hpp src/objectpool.hpp src/objectpool.cpp src/ipc.hpp src/ipc.cpp src/buffer.hpp src/buffer.cpp src/lock.hpp src/lock.cpp src/globals.cpp src/globals.hpp -Wall -Wextra -ldl -std=c++0x -pthread -fpermissive -ljson_linux-gcc-4.8_libmt 
	g++ -g -shared -o libc.so.6 overload.o fd.o registerEnv.o interface.o buffer.o objectpool.o ipc.o lock.o globals.o -Wall -Wextra -ldl -std=c++0x -pthread -fpermissive -ljson_linux-gcc-4.8_libmt
	g++ -g src/resilience.cpp -lm -o resilience -Wall -Wextra -std=c++0x -Wno-write-strings -pthread -fpermissive
	


clean: 
	rm -rf libc.so.6 *.o src/*.hpp.*
