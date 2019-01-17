
INCLUDES	:= -I$${AIO_LIB_DIR}
override CFLAGS	+= -D_GNU_SOURCE -I. $(INCLUDES) -I/usr/include/libusb-1.0 -std=gnu99 -g


FILES	:= $(wildcard *.c)
MOCKS	:= $(FILES:%.c=lib%.so)
GREEN	:= $(shell tput setaf 2)
RESET	:= $(shell tput sgr0 )
RED	:= $(shell tput setaf 1 )

CPPMOCK_SRC	:= mock_aiocontbuf_get_data_arduino.c mock_usb_xfers.c mock_capture_usb.c
CPPLIBS	:= $(patsubst %.c,lib%_cpp.so,$(CPPMOCK_SRC))


all:	$(MOCKS) $(CPPLIBS)

lib%.so:%.c
	@cmake -E cmake_echo_color --green "Building $@"
	@$(CC) $(CFLAGS) -shared -Wl,-soname,$@ -shared -fPIC -o $@ $< -ldl

lib%_cpp.so:%.c
	@cmake -E cmake_echo_color --green "Building $@"
	@$(CXX) -I. -I../  -I/usr/include/libusb-1.0  -D__aiousb_cplusplus -g -c  -DNDEBUG -Wall -pthread -fPIC $< -o $(patsubst %.c,%.o,$<)
	@$(CXX) -D_GNU_SOURCE -fPIC -I. -I${AIO_LIB_DIR} -I/usr/include/libusb-1.0 -g  -shared -Wl,-soname,$(patsubst %.c,lib%_cpp.so,$<) -o $(patsubst %.c,lib%_cpp.so,$<) $(patsubst %.c,%.o,$<) -ldl


clean:
	rm -f *.so
