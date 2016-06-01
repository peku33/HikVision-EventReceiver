HikVision Event Receiver
========================

**APPLICATION UNDER DEVELOPMENT, DONT TRY TO RUN IT YET**

**DS2CD2x32EventReceiver_Standalone_Console is ready to use, you may try it**

Introduction
-------
This application is a web-friendly solution for receiving, logging and browsing 'events' sent by HikVision IP Cameras. It also provides console interface to see events reported by the camera. Its easy to extend with new features.

The application is under development and is going to be tested with:

 - DS2CD2132-i
 - DS2CD2132f-is
 - DS2CD2532
 - DS2CD2332

**Other DS2CD2x32 cameras are most likely supported.**
**Other DS2CDxxxx cameras are probably supported.**

Key features:

 - Single application supports multiple cameras (list defined in database)
 - Receives data from cameras and displays them in user-friendly web interface
 - Depends on camera built-in event system. Application does not analyze or even receive image stream from camera. Such approach allows operation in low-resource environments such as rasberry pi, etc
 - Talks to cameras via ISAPI (google hikvision isapi) in xml-based protocol (see AlertStream section in isapi documentation)
 - Provides mechanisms to detect connection problems, camera death etc

Supported events are:

 - General camera failure (connection problem / camera error / etc)
 - Video loss (event reported by the camera)
 - Video tampering (event reported by the camera)
 - Motion detection (event reported by the camera)
 - Line (crossing) detection (event reported by the camera)
 - Field detection (event reported by the camera)

Architecture
-------

The application consist of 3 parts:

 - SQLite3 database which is the main mechanism for storage and basic communication
 - C++ backend (daemon) maintaining connection to camera and storing received events into database
 - Web frontend responsible for presenting data from database to user

There only communication method between those parts is the database itself. No pipes / sockets / etc.

It is of course possible to use only database + backend or even backend only.

System requirements
-------

 - Linux-based operating system
 - Decent CPU (backend uses on avarage 2%-8% of 1GHZ arm core)
 - Decent amount of RAM (backend uses ~100MB of Vmem for 8 cameras, but this is not linear, most of this is sqlite3 cache)
 - BackEnd:
	 - c++11 compiler
	 - [SQLite3](https://www.sqlite.org/) (lightweight serverless database)
	 - [Boost](http://www.boost.org/) (additional libraries) (headers only are required)
	 - [scons](http://scons.org/) (make-like system)
 - FrontEnd:
	- [Node.js](https://nodejs.org/en/) (runtime environment and frontend web server)
	- [npm](https://www.npmjs.com/) (package manager to download bower and grunt)
	- [Bower](http://bower.io/) (web components downloader)
	- [Grunt](http://gruntjs.com/) (build system)


Getting dependencies
-------

(commands for Debian / Ubuntu)

Install BackEnd requirements using system package-manager:

`$ apt-get install g++ libsqlite3-dev libboost-dev scons`

Install FrontEnd system packages using system package-manager:

`$ apt-get install nodejs npm`

Install FrontEnd packages using npm:

`$ npm install --global bower grunt-cli`

Building
-------

BackEnd:
 - Do `cd` into BackEnd directory
 - Run `scons` to compile all source files to objects and all objects to target executable. Use `scons DEBUG=1` to build debug version (with symbols etc).
 - Executables would be placed in Release directory
 - It is ready to use
 
FrontEnd:
 - Do `cd` into FrontEnd directory.
 - Run `npm install` to install server dependencies.
 - Run `bower install` to install front-end dependencies.
 - Run `grunt server` to start the front-end server. It should automatically open the client in your browser when ready.

Usage
-------

Main application: TODO

`DS2CD2x32EventReceiver_Standalone_Console` - Application intended for standalone usage. Does not required SQLite3, front-end components. Simply connects to camera specified by command-cline arguments and prints all received events to console.
 - To run application execute `./Release/DS2CD2x32EventReceiver_Standalone_Console CameraIp AdminPassword`
 - Application runs and prints everything to console
 - To kill it - CTRL+C
 - See [screen](https://www.mattcutts.com/blog/a-quick-tutorial-on-screen/) command to put application in background