Idea is to replace the local controller of Gardena hydro-valve by ESP32 module in order to control Gardena valves with Wifi and Domoticz or any other Domotoc sofware

* How to compile/upload with Linux

cp ~/Arduino/hardware/espressif/esp32/tools/partitions/default.csv /tmp/mkESP/blink_esp32/partitions.csv
espmake32 SKETCH="~/GIT/Arduino/Esp32BasicProgram/LoRa/loraSenderDefaultDTH11.ino" flash


* How to monitor output
sudo minicom /dev/ttyUSB0


* Add LoRa library 
in ~/Arduino/hardware/espressif/esp32/libraries$
git clone https://github.com/sandeepmistry/arduino-LoRa LoRa

