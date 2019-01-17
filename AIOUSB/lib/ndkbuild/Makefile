

all: builtndk.txt


builtndk.txt:
	cd libusb && git submodule init && git submodule update
	ndk-build
	touch builtndk.txt

clean:
	ndk-build clean
	rm -rf obj
	rm -rf libs
	rm -f builtndk.txt
