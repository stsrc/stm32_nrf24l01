# stm32_nrf24l01 - GPS localisation project

Idea of this project is to create GPS localisation system which utilises android
application to present distance and direction to the GPS receiver.

To achieve that multiple devices are used: on slave side we use NEO-6M GPS
module and nrf24l01 module to transmit GPS data to the master side.
On master side we use nrf24l01 to receive GPS data and HM-10 module to send it
to the android phone.

## Repository content:

slave PCB with nrf24l01 transmitter and NEO-6M GPS receiver, battery powered

    ./src-kicad/stm32_slave_bat

slave firmware

    ./stm32f031f6_slave

master PCB with nrf24l01 receiver and HM-10 bluetooth transmitter, battery powered

    ./src-kicad/stm32_master_bt_bat

master firmware

    ./stm32f031f6_master

example code for STM32VLDISCOVERY board

    ./stm32f100rb
