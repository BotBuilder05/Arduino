 * Installation des modules esp32 dans l'IDE d'Arduino
 https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/

 * Installation pour compilation avec command line sous linux (tester) mac/windows ?
 ** 
 https://github.com/plerup/makeEspArduino
 https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/debian_ubuntu.md
 ** A ajouter au .bashrc 
 alias espmake32="make -f ~/Arduino/hardware/makeEspArduino/makeEspArduino.mk CHIP=esp32 ESP_ROOT=../espressif/esp32 CHIP=esp32"

Penser a source le .bashrc pour utilisaiton sans le shell courant
