# WebTooth-Extractor
A cross-platform desktop tool to extract data from any webpage with predefined filters with one click.

##Supported platforms & Operating Systems
Generally only pure Qt classes and datatypes have been used, so this code should be runnable on all platforms that Qt supports. So far I can confirm the following platforms and OS to be tested:
* **Windows** MSVC 2013 64bit, tested on Windows 8.1-x64
* **Linux** GCC 4.8.2 64bit, tested on Ubuntu 14.04-x64

##Requirements
* Modern C++ compiler suite (CLANG >= 3.5, GCC >= 4.82, MSVC-2013)
* Qt 5.4.1 is recommended, older versions of v5 could run as well but are not confirmed as of now

##ToDo List
- [ ] Compilable and runnable on recent Linux-64bit distros (released in 2014) - *Help needed*
- [ ] Compilable and runnable on recent Mac OS X (10.9) - *Help needed*
- [ ] Implement Export of CSV & JSON
- [ ] Implement the Occurrence functionality, after more reflection and possibly *feedback from users*

##Build Infos
* Build the QPropertyBrowser library first with the buildlib.pro file in its own directory. Then test it with one of the example projects, for example the Demo.

##Manual
The manual for the WebTooth-Extractor is embedded in the application. Just press F1.

## Screenshot
Here a first screenshot of the program. More will follow. Feedback, testing and pull-requests are very welcome from :girl: and :boy:.
![Screenshot of WebTooth-Extractor](http://i.imgur.com/MstgpLY.png)

###Licensing
This application is under GLPv3 and free for personal use. Please look into the license file for more details including commercial licensing.
