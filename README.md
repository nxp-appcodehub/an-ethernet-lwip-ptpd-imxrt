# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## AN12149: Implementing an IEEE 1588 V2 on i.MX RT  Using PTPd, FreeRTOS, and lwIP TCP/IP  stack

This software accompanies application note AN12149, which describes how to implementation a 1588 V2 application based on ptpd.

The software is based on MCUXpresso IDE and just enable RT1020-EVK and RT1050-EVKB. Use two boards and a ethernet cable can run the test. The board will print the log of delay information.

Please refer to AN12149 for complete instructions on how to use this software.

#### Boards: EVK-MIMXRT1020, EVKB-IMXRT1050
#### Categories: Time Sensitive Networking, Networking
#### Peripherals: ETHERNET
#### Toolchains: MCUXpresso IDE

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5) 
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
- SDK: 2.15.0
- IDE: MCUXpresso IDE v11.9.0

## 2. Hardware<a name="step2"></a>
- IMXRT1050-EVKB
- MIMXRT1020-EVK
- Ethernet Cable with RJ45 connector
- Two micro USB cable

## 3. Setup<a name="step3"></a>
1. Compile the project with the pre-defined macro 'PTP_APP_MASTER_TEST=1' and download it to the first board.
2. Compile the project with the pre-defined macro 'PTP_APP_MASTER_TEST=0' and download it to the sencond board.
3. Power on the two boards and connect the debug usb port to PC and open terminal tool on PC to monitor the log.
4. Connect two RJ45 ports of the two boards with an ethernet cable.
5. Follow the requirement in the AN to use the oscilloscope to monitor the two 1PPS signal output from the two boards.

![picture](images/setup.png)


## 4. Results<a name="step4"></a>
When the demo run successfully, the delay value in the log will tend to be stable. And in the oscilloscope, the 1PPS signal will close to each other.

![picture](images/result1.png)

![picture](images/result2.png)


## 5. FAQs<a name="step5"></a>
What is the synchronization principle.

![picture](images/sync_principle.png)

## 6. Support<a name="step6"></a>
If you have questions, please create a ticket in NXP Community.

#### Project Metadata

<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-EVK&ndash;MIMXRT1020-blue)](https://www.nxp.com/pip/MIMXRT1020-EVK)
[![Board badge](https://img.shields.io/badge/Board-EVKB&ndash;IMXRT1050-blue)](https://www.nxp.com/pip/MIMXRT1050-EVK)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-TIME%20SENSITIVE%20NETWORKING-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=tsn)
[![Category badge](https://img.shields.io/badge/Category-NETWORKING-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=networking)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-ETHERNET-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=ethernet)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://mcuxpresso.nxp.com/appcodehub?toolchain=mcux)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected functionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/X-Follow%20us%20on%20X-black.svg)](https://x.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | February 25<sup>th</sup> 2025 |

## Licensing



## Origin

