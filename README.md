# AlarmControlPanel [![Build Status](https://travis-ci.org/Plytirex/AlarmControlPanel.svg?branch=master)](https://travis-ci.org/Plytirex/AlarmControlPanel)

Alarm Control Panel is academic project written in C++ for Basics of Microprocessors
Technology Lab, which was created by three people. It uses STM32F4-Disovery board
together with STM32CubeF4 firmware and FreeRTOS. 

ACP is still in alpha stage, take a look into
[issue section](https://github.com/Plytirex/AlarmControlPanel/issues),
to see what needs tweaking.

Currently it supports LCD screen with user interface controlled by membrane keyboard,
up to 4 PIRs, httpserver with partial GET method support, local time with dedicated
python server for time synchronization and Web Control Panel (which is not currently
bundled, due to issue #13 .

## 1. BUILD ##
Project uses linux-only Makefile, so basic linux envoirnment (mkdir, rm, sed) is
recommeded. For building std::experimental::any library is needed. GCC since
version 5.1.0 is known to work. Check your compiler's documentation, if you would
like to use the other one.

### 1.1. Clone repo and submodules ###

    git clone https://github.com/Plytirex/AlarmControlPanel
    git submodule update --init
    

### 1.2. Release version: ###

    make depends && make release
    

### 1.3. Debug version: ###
Debug version uses semihosting to print additional debug info using C `printf`, 
which in addition slows program significantly. OpenOCD server started with `arm
semihosting enable` is needed to enable semihosting. Debug build **can't be used**
without semihosting server – otherwise the program will hang on first printf.

    make depends && make debug
    
 ## 2. Equipment ##

| Item                  | Model             | Links                       |
|-----------------------|-------------------|-----------------------------|
| STM32F4 Board         | STMF407 Discovery | [Docs][1]                   |
| Alphanumeric LCD 2x16 | HD44780           | [Docs][2]                   |
| Membrane keyboard 4x4 |                   |                             |
| Ethernet controller   | ENC28J60          | [Docs][3]  / [More Docs][4] |
| PIR Sensor            | HC-SR501          | [Docs][5]                   |
| Wireless adapter      | nRF24L01 2.4GHz   | [Docs][6]                   |

[1]: http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF252419#tab-2
[2]: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
[3]: http://ww1.microchip.com/downloads/en/DeviceDoc/39662e.pdf
[4]: http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en022889
[5]: http://www.mpja.com/download/31227sc.pdf
[6]: http://www.nordicsemi.com/jpn/content/download/2730/34105/file/nRF24L01_Product_Specification_v2_0.pdf

## 3. Pins connection

```
     +-------------------------------------------------------+-------------------------------+            
     |                                                       |                               |            
+----+----+                                                  |                         +-----+---------+  
|   GND   |                                                  |                         |    GND        |  
| e   VCC +---------------------------------------------+    |     +-------------------+ VCC        h  |  
| n   INT |----------------+                            |    |     | +-----------------| RS         d  |  
| c   RST |-------------+  |                            |    |     | |  +--------------| R/~W       4  |  
| 2    CS |----------+  |  |                            |    |     | |  | +------------| E          4  |  
| 8   SCK |--------+ |  |  |   +------------------------+----+--+  | |  | | +----------| D4         7  |  
| j    SO |------+ | |  |  +---+ PB2                    3V  GND |  | |  | | | +--------| D5         8  |  
| 6    SI +----+ | | |  +------+ PB1                            |  | |  | | | | +------| D6         0  |  
| 0       |    | | | |         |                             5V +--+ |  | | | | | +----+ D7            |  
|         |    | | | +---------+ PB12                           |    |  | | | | | |    |               |  
+---------+    | | +-----------| PB13                       PE1 +----+  | | | | | |    +---------------+  
               | +-------------| PB14                       PE3 |-------+ | | | | |                       
               +---------------+ PB15                       PE5 |---------+ | | | |                       
                               |        STM32F4-Discovery   PE0 |-----------+ | | |                       
+-----------------+            |                            PE2 |-------------+ | |                       
|                 |            |                            PE4 |---------------+ |                       
| k          Row0 +------------+ PE11                       PE6 +-----------------+                       
| e          Row1 |------------| PE12                           |                                         
| y          Row2 |------------| PE13                           |    3V+--+-----------+----+-----------+-+
| b          Row3 |------------| PE14                           |         |           |    |           |  
| o          Col0 |------------| PE7                            |         | +-------+ |    | +-------+ |  
| a          Col1 |------------| PE8                            |         +-+VCC  P | |    +-+VCC  P | |  
| r          Col2 |------------| PE9                        PC6 +-----------|OUT  I | | +----|OUT  I | |  
| d          Col3 +------------+ PE10                           |         +-+GND  R | | |  +-+GND  R | |  
|                 |            |                            PC7 +-----+   | +-------+ | |  | +-------+ |  
+-----------------+            |                                |     |   |           | |  |           |  
                               |                            PC8 +-----------------------+  |           |  
                               |                                |     |   |           |    |           |  
                               |                            PC9 +--+  |   |   |-------+    |   |-------+  
                               |                                |  |  |   | +-------+      | +-------+    
                               +--------------------------------+  |  |   | |VCC  P |      | |VCC  P |    
                                                                   |  +-----+OUT  I |  +-----+OUT  I |    
                                                                   |      | |GND  R |  |   | |GND  R |    
                                                                   |      | +-+-----+  |   | +-+-----+    
                                                                   |      |   |        |   |   |          
                                                                   +-------------------+   |   |          
                                                                          |   |            |   |          
                                                                 GND------+---+------------+---+----------
```

## 4. Known Bugs / Troubleshooting

* internet connection is not stable - issue #13
* keys on membrane keyboard are not reliable - use better quality membrane keyboard

## 5. Future plans

* wireless PIRs - issue #12
* JSON Server and API with authentication - issue #10
* microcontroller configuration to support battery power supplye - issue #6
* memory profiling to use memory more efficient (i.e. optimisation of FreeRTOS tasks stacks sizes)


# enc28j60 driver
*Original source code taken from:*  
  <http://www.elektroda.pl/rtvforum/topic2394580.html>  
*Modified version ported to FreeRTOS taken from:*  
  <https://github.com/Plytirex/FreeRTOS-TCP-enc28j60>
