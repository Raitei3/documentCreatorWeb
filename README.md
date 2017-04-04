# Font-extractor

Semi-automatic extraction of characters from an image into a font for the generation of images and old documents.

This application can be tested online, or installed locally.


## Manual installation
### Dependencies
There are three dependencies: Qt5 ,OpenCV and, tesseract

#### Qt installation

```
$ sudo apt-get install qtbase5-dev qtdeclarative5-dev libqt5xmlpatterns5-dev
```
#### tesseract installation
```
$ sudo apt-get install tesseract-ocr tesseract-ocr-fra libtesseract-dev libleptonica-dev
```
#### Opencv installation
```
$ sudo apt-get install libopencv-dev
```
#### Libnavajo installation
 You may need to first install OpenSSL & PAM
```
$ sudo apt-get install libssl-dev libpam0g-dev
```
Then you have to clone and install the libnavajo projet
```
$ git clone https://github.com/titi38/libnavajo.git
$ cd libnavajo
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
```
### Installation
```
$ git clone https://github.com/Raitei3/documentCreatorWeb.git
$ cd server
$ mkdir build
$ cd build
$ cmake ..
$ make
```


## How to use
To lauch the server, go into the build folder and make:
```    
$ ./documentCreatorWeb
```
The server will be run by default on port 8080.
You can change the port in the file libnavajo/src/WebServer.cc of libnavajo

```#define DEFAULT_HTTP_PORT 8080```

If you are in local, you can access to the server throught: http://localhost:8080

## Demo
The application is host online :
    https://intranet.iut.u-bordeaux.fr/navajo/


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
* [Martin Bazalgette]
* [Antoine Borde]
* [Paul BEZIAU]
* [Zakia Taoufik]
