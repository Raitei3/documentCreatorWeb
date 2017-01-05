# Font-extractor

Semi-automatic extraction of characters from an image into a font for the generation of images and old documents.

This application can be tested online, or installed locally. To 
install it two solutions are provided : manually or from a docker 
image.
## Manual installation
```
// Opencv installation
#Ubuntu Linux distribution
$ sudo apt-get install libopencv-dev
    
#MAC distribution (with Homebrew)
$ brew install opencv

// Libnavajo installation
// You may need to first install OpenSSL & PAM
// On Ubuntu:
// sudo apt-get install libssl-dev libpam0g-dev
// On Fedora:
// dnf install openssl-devel pam-devel
$ git clone https://github.com/titi38/libnavajo.git
$ cd libnavajo
$ mkdir build
$ cd build 
$ cmake ..
$ make
$ sudo make install
    
// Font-extractor installation
$ git clone https://github.com/BorisMansencal/Pdp.git
$ cd Pdp/server
$ make
```

## Docker installation
First install docker https://docs.docker.com/engine/installation/
```    
$ docker run -dti -p  80:8080 rtisne/font-extractor
```

This may take some times.
After the installation just go to your docker ip address 

## How to use
To lauch the server, go into the server folder and make:
```    
$ ./main
```
The server will be run by default on port 8080.
You can change the port in the file libnavajo/src/WebServer.cc of libnavajo

```#define DEFAULT_HTTP_PORT 8080```

If you are in local, you can access to the server throught: http://localhost:8080

## Demo
The application is host online :
https://intranet.iut.u-bordeaux.fr/navajo/

## Building and running unit tests
Tests are available to test the algorithm part that retrieves components in the image.
To launch test go to the test folder and build:
```    
$ cmake.
$ make
$ ./Unit_test
```

## Development
Developed by:
* [Tristan Autret](https://github.com/tautret)
* [Anthony Boschet](https://github.com/aboschet)
* [Thomas Labrousse](https://github.com/Shqrk)
* [Romain Tisné](https://github.com/rtisne)
* [Jimmy Gouraud]
* [Felix Bruneau]
* [Nicolas Vidal]
* [Boris Mansencal]
* [Nicholas Journet]

[Report link]()






