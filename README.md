# Water Level Sensor

This project contains the code for using an ultrasonic distance sensor to determine the water level in a tank.

An Arduino Uno reads from the sensor, sends the data to a 4-digit display, and uses an ESP8266 to log the data to a remote ThingsBoard instance hosted on Google Cloud Platform.

The ESP8266 is programmed to handle all the web traffic, receiving wake signal from the Uno and data via serial.

## Hardware

1. Arduino Uno R3
2. WiFi ESP-01 (ESP8266) (with simple breakout board)
3. Ultrasonic Sensor AJ-SR04M
4. Display TM1637
5. Bi-directional Logic Level Controller

## Gotchas

To program the ESP8266 using the Arduino Uno (this can be programmed but the one channel in my LLC broke, so I couldn't automate the GPI0 High/Low).

1. Make sure Uno Sketch is empty (select Uno board).
1. Select ESP8266 board.
1. Connect Rx to Rx, Tx to Tx.
1. Put ESP in Flash Mode
    1. ESP GPI0 -> GND
    1. ESP RST -> GND then 3.3V (just a quick tap)
    1. ESP GPI0 -> 3.3V (or float) (GPI0 must not be GND for Upload).
1. Upload sketch.

### FatalException and IllegalInstruction

I had lots and lots of trouble with the ESP throwing all sorts of exceptions and stack traces, which I never fully nailed down. I believe it had to do with the breadboard connections and the iffy serial communication, also possibly power issues, although I would watch it work flawlessly for a few uploads and then just start crashing, and then fix itself and then crash again. I ended up moving stuff around on the breadboard, made sure the wires were fasted, erased the flash a number of times, and tried putting the original firmware on, and NodeCMU, all of which were unsuccessful. The last post on [this page](https://esp32.com/viewtopic.php?t=4061) helped me get things back on track.

- erase flash -> `esptool.py --chip ESP8266 --baud 115200 --port /dev/ttyACM0 erase_flash`
- nodecmu -> `esptool.py --chip ESP8266 --baud 115200 --port /dev/ttyACM0 write_flash -z --flash_mode dio --flash_freq 80m --flash_size 1MB 0x0 ~/Downloads/nodemcu-release-7-modules-2022-07-15-21-56-08-float.bin` (or --flash_mode qio/--flash_mode dout)

### WiFi fails.

I tried using the [WiFiAutoSelector.h](https://gist.github.com/AndiSHFR/e9c46890af7cddff6cb5ea7d4f1c5c49) which did seem to work, but it coincided with the time I started getting all the crashes and FatalExceptions, so I reverted back to a simple hardcoded cycle through known WiFis. Not pretty, but does the job. Will make updating/changing the WiFi settings a mission, but I don't foresee that happening anytime soon, so we should be good.

## Wiring

![circuit](images/circuit.png)