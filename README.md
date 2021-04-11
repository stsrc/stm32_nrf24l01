# stm32_nrf24l01 - GPS localisation project

Idea of this project is to create GPS localisation system which utilises android
application to present distance and direction to the GPS receiver.

To achieve that multiple devices are used: on slave side we use NEO-6M GPS
module and nrf24l01 module to transmit GPS data to the master side. On master
side we use nrf24l01 to receive GPS data and HM-10 module to send it to the
android phone.

Repository content:
src-kicad/stm32_slave_bat - slave device with nrf24l01 transmitter and NEO-6M
GPS receiver, battery powered
stm32f031f6_slave - code for slave device

src-kicad/stm32_master_bt_bat - master device with nrf24l01 receiver and HM-10
bluetooth transmitter, battery powered
stm32f031f6_master - code for master device

stm32f100rb - example code for STM32VLDISCOVERY board
