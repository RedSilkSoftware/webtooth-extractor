# WebTooth-Extractor

[![Join the chat at https://gitter.im/RedSilkSoftware/webtooth-extractor](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/RedSilkSoftware/webtooth-extractor?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
A cross-platform desktop tool to extract data from any webpage with filters in **1 click**. You can create filters with *HTML Tags*, IDs and attributes or by writing *Regular-Expressions*. Once the filters are setup you can extract the data as many times as you like with just 1 click (as long as the structure of the page doesn't change). Then you can simply copy&paste them into your favorite spreadsheet program or export them as CSV or JSON.

Please support this project with a donation via flattr [![Flattr this git repo](http://button.flattr.com/flattr-badge-large.png)](https://flattr.com/submit/auto?user_id=patlecat&url=https%3A%2F%2Fgithub.com%2FRedSilkSoftware%2Fwebtooth-extractor%2F&&title=WebTooth-Extractor&language=cpp&tags=github&category=software) or PayPal [![Make a PayPal donation](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=B8UFHJ6R5Z524). Let me know if you prefer other services to donate with.

Discuss this application here: [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/RedSilkSoftware/webtooth-extractor?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

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
- [x] Implement Export of CSV
- [ ] Implement Export of JSON
- [ ] Implement the Occurrence functionality, after more reflection and possibly *feedback from users*

##Build Infos
* Build the QPropertyBrowser library first with the buildlib.pro file in its own directory. Then test it with one of the example projects, for example the Demo.

##Manual
The manual for the WebTooth-Extractor is embedded in the application, just press **F1**. But check the **tooltips** first for the fields, windows, boxes and menus. They should explain a lot.

## Screenshot
Here a first screenshot of the program. More will follow. Feedback, testing and pull-requests are very welcome from :girl: and :boy:.
![Screenshot of WebTooth-Extractor](http://i.imgur.com/MstgpLY.png)

###Licensing
This application is under GLPv3 and free for personal use. Please look into the license file for more details including commercial licensing.
