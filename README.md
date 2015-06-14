# AlarmControlPanel [![Build Status](https://travis-ci.org/Plytirex/AlarmControlPanel.svg?branch=master)](https://travis-ci.org/Plytirex/AlarmControlPanel)
Project of Intruder Alarm Control Panel.  
You can find used devices at [project's wiki page](https://github.com/Plytirex/AlarmControlPanel/wiki/Equipment)

------------------------------------

# BUILD
    git clone https://github.com/Plytirex/AlarmControlPanel
    git submodule update --init

## Release version:
    make depends && make release

## Debug version
It needs openocd started with `arm semihosting enable`.

    make depends && make debug

# enc28j60
Original source code taken from http://www.elektroda.pl/rtvforum/topic2394580.html
Modified version taken from https://github.com/Plytirex/FreeRTOS-TCP-enc28j60
