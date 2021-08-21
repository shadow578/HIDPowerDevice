# HID Power Device for Arduino

This project allows an Arduino board with USB capabilities to act as a HID- compliant UPS.

The project is useful if you want to build a smart UPS or other power device, which can report its status to the host PC.

For more information have a look at the [USB HID specs](https://www.usb.org/sites/default/files/pdcv11.pdf)

For more information about USB HID specs please read 


## Supported Boards

- Leonardo
- (Pro) Micro


## Setup & Usage

Setup is simple. Just clone this repo, update the `config.h` file to match your hardware, and upload using [PlatformIO](https://platformio.org/).

You may have to adjust the `PATCH_EXE` variable in `patch_core.py` if you're getting errors.

Once upload is complete you will find your HID Battery in your system.


### NUT

[NUT](https://networkupstools.org/) decided to kinda ignore the HID spec and only check on USB Interface 0 (see [here](https://github.com/abratchik/HIDPowerDevice/issues/1)).

While my original workaround is still valid, it has been automated with the switch to PlatformIO. so you don't really have to do any changes to the project (maybe change hwids in `patch_hwid.py`).


The Arduino will pretend to be a [EATON E51100iUSB](https://networkupstools.org/ddl/Eaton/5E1100iUSB.html). 
to make it work with NUT, use the following config file:

> ```
> [ups]
>   driver = usbhid-ups
>   port = auto
>   desc = Arduino UPS  // optional
>   pollfreq = 30       // optional
>   vendorid = 0463     // same as in boards.txt
>   productid = ffff    // same as in boards.txt
> ```

\* (pollfreq, vendorid and productid can be added in 'Auxiliary Paramters (ups.conf)' field in TrueNAS)


## Tested Operating Systems

- Windows 10
- TrueNAS Core
- Ubuntu 18.04.05 LTS\*
- Mac OSX 10.14.6 Mojave\*

\* tested by [abratchik](https://github.com/abratchik/HIDPowerDevice), but should work since the core logic did not change.


## License

Adaptions to a standalone project + NUT support + change to PlatformIO by shadow578 (everything except `/src/pdlib/`):
> Copyright 2021 shadow578
> 
> Licensed under the Apache License, Version 2.0 (the "License");
> you may not use this file except in compliance with the License.
> You may obtain a copy of the License at
> 
> http://www.apache.org/licenses/LICENSE-2.0
> 
> Unless required by applicable law or agreed to in writing, > software
> distributed under the License is distributed on an "AS IS" BASIS,
> WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or > implied.
> See the License for the specific language governing permissions > and
> limitations under the License.

---

Original library by [abratchik](https://github.com/abratchik/HIDPowerDevice) (the library is contained in `/src/pdlib/`):
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
