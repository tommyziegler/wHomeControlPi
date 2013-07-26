wHomeControlPi
==============

Wireless HomeControl NodeJS Addon for RaspberryPi


## About

wHomeControlPi is for controlling radio controlled remote control sockets / switch.


## Prerquirements

 1. [Raspberry Pi](http://www.amazon.de/gp/product/B008PT4GGC/ref=as_li_ss_tl?ie=UTF8&camp=1638&creative=19454&creativeASIN=B008PT4GGC&linkCode=as2&tag=tommzieg-21)
 2. [433 MHz Sender](http://www.exp-tech.de/Shields/RF-Link-Transmitter-434MHz-WRL-10534.html)
 5. wiringPi


###Install wiringPi

`````bash
sudo apt-get install git-core
sudo apt-get update
sudo apt-get upgrade
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build
`````
See wiringPi website: https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install

### Using wHomeControlPi

`````javascript
var wHomeControlPi = require('wHomeControlPi');

wHomeControlPi.switchElroOn('00010', 'A');
//wHomeControlPi.switchRevOn('A', 1);

wHomeControlPi.switchElroOff('00010', 'A');
//wHomeControlPi.switchRevOff('A', 1);

`````
