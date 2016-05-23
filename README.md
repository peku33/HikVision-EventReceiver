HikVision Event Receiver
========================

Introduction
-------
This application is a web-friendly solution for receiving, logging and browsing 'events' sent by HikVision IP Cameras.

The application is under developement and is going to be tested with:

 - DS2CD2132-i
 - DS2CD2132f-is
 - DS2CD2532
 - DS2CD2332

**Other DS2CD2x32 cameras are most likely supported.**

Key features:

 - Single application supports multiple cameras (list defined in database)
 - Receives data from cameras and displays them in user-friendly web interface
 - Depends on camera built-in event system. Application does not analyze or even receive image stream from camera. Such approach allows operation in low-resource environments such as rasberry pi, etc
 - Talks to cameras via ISAPI (google hikvision isapi) in xml-based protocol (see AlertStream section in isapi documentation)
 - Provides mechanisms to detect connection problems, camera death etc

Supported events are:

 - General camera failure (either connection problem or 'camera failure' event reported from camera)
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
	 - sqlite3 libraries
	 - scons
 - FrontEnd:
	- @TODO
	- Bower
	- npm
	- grunt

Building
-------

BackEnd:
 - Do cd into BackEnd directory
 - Run `scons` to compile all source files to objects and all objects to target executable. Use `scons DEBUG=1` to build debug version (with symbols etc).
 - Executable would be placed in Release directory
 - It is ready to use

Usage
-------
@TODO