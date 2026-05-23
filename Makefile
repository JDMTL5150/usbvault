main: src/usbvault.cpp
	g++ -std=c++20 src/usbvault.cpp -o bin/UsbVault
debug: src/usbvault.cpp
	g++ -std=c++20 -g src/usbvault.cpp -o bin/UsbVault
clean:
	rm bin/*
ssl: src/usbvault.cpp
	g++ -std=c++20 src/usbvault.cpp -o bin/UsbVault -lssl -lcrypto
gssl: src/usbvault.cpp
	g++ -g -std=c++20 src/usbvault.cpp -o bin/UsbVault -lssl -lcrypto