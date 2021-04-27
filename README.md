# HID UPS Power Device Library for Arduino
This library allows an Arduino board with USB capabilities to act as a HID-compliant UPS according to USB HID specifications. 
The library is useful if you want to build a smart UPS or other power device, which can report its state to the PC host
or perform power on/power off operations as necessary for your project.

For more information about USB HID specs please read https://www.usb.org/sites/default/files/pdcv11.pdf

## Supported Arduinos
* Leonardo
* (Pro)Micro

## Setup & Usage
Setup is very simple. Just clone this repository and upload the sketch HIDPowerDevice.ino.
Once upload is completed successfully you will find HID Device Battery in  
your system.

## Additional setup for NUT
[NUT](https://networkupstools.org/) is dumb, see [here](https://github.com/abratchik/HIDPowerDevice/issues/1#issuecomment-826086454).

> Managed to get this working with NUT (on TrueNas SCALE, tho it should work on any other os too). Quick note that this solution is really just multiple hacks, but hey, if it works it > ain't stupid.
> 
> 1. you have to disable CDC so the HID device is the only descriptor on the arduino. Luckily, the AVR Cores have a [pull request](https://github.com/arduino/ArduinoCore-avr/pull/383) > implementing exactly this, so you just have to kinda copy that. It may also be possible to keep CDC as the second / third descriptor, but just disabling it is way simpler.
> 
> 2. since we now no longer have Serial, all calls to Serial.* have to be removed / commented in HIDPowerDevice.ino
> 
> 3. lastly, NUT for some reason does not detect the device with stock PID and VID, so you have to change it. This can be done in the boards.txt file, keys leonardo.build.vid and > leonardo.build.pid. I just values equivalent to a [EATON E51100iUSB](https://networkupstools.org/ddl/Eaton/5E1100iUSB.html), tho any random ups that supports the usbhid-ups driver > should work fine for this.
> 
> 4. upload the sketch. This has to be done using a ICSP (like a USBASP) as we disabled CDC. Well, technically you could upload using the "normal" method, however only the first time.
> 
> The NUT config file looks something like this:
> ```
> [ups]
>   driver = usbhid-ups
>   port = auto
>   desc = Arduino UPS  // optional
>   pollfreq = 30             // optional
>   vendorid = 0463       // same as in boards.txt
>   productid = ffff         // same as in boards.txt
> ```
> (pollfreq, vendorid and productid can be added in "Auxiliary Parameters (ups.conf)" field in TrueNas)
> 
> From here on, it's just like with any other UPS.
> Note, however, that this solution really is just a bunch of hacks to the arduino core, and may break any time. That being said, I'd also recommend reverting all changed files to  their  original state to avoid unpleasant surprises with other projects.

## Tested on Operating Systems
* Mac OSX 10.14.6 Mojave
* Ubuntu 18.04.05 LTS 
* Windows 10

## License

Original work by [abratchik](https://github.com/abratchik/HIDPowerDevice).
Modifications by shadow578

> Copyright (c) Alex Bratchik 2020. All right reserved.
>
> This library is free software; you can redistribute it and/or
> modify it under the terms of the GNU Lesser General Public
> License as published by the Free Software Foundation; either
> version 2.1 of the License, or (at your option) any later version.
> 
> This library is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
> Lesser General Public License for more details.
> 
> You should have received a copy of the GNU Lesser General Public
> License along with this library; if not, write to the Free Software
> Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
