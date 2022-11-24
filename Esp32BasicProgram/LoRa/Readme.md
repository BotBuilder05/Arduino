Idea is to replace the local controller of Gardena hydro-valve by ESP32 module in order to control Gardena valves with Wifi and Domoticz or any other Domotoc sofware

* How to compile/upload with Linux

cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/blink_esp32/partitions.csv
espmake32 SKETCH="~/GIT/Arduino/Esp32BasicProgram/LoRa/loraSenderDefaultDTH11.ino" flash


* How to monitor output
sudo minicom /dev/ttyUSB0


* Add LoRa library 
in ~/Arduino/hardware/espressif/esp32/libraries$
git clone https://github.com/sandeepmistry/arduino-LoRa LoRa

esptool --chip esp32 -p /dev/ttyUSB0 erase_flash
esptool --chip esp32 -p /dev/ttyUSB0 run
cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/SenderGate_esp32/partitions.csv
cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/SenderGate_esp32/partitions.csv
cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/ReceiverGate_esp32/partitions.csv
cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/loraReceiverMqtt_esp32/partitions.csv
cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/loraSenderTeleInfo_esp32/partitions.csv

* Add in .bash_rc
alias espmake32="make -f ~/Arduino/hardware/makeEspArduino/makeEspArduino.mk CHIP=esp32 ESP_ROOT=~/Arduino/hardware/espressif/esp32 CHIP=esp32"
export EDITOR=/usr/bin/vim
