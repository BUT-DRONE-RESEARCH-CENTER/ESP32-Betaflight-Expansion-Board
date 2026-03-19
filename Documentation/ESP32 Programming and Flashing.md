
This file goes over the necessary tools, libraries, commands and precautions for flashing ESP32 micro controllers. It is specifically aimed at Linux computers.

### 1. Necessary Tools and Libraries

First, ensure the `arduino-cli` tool is installed. It is necessary for compiling the code and building binaries for flashing the ESP32.
```bash
# Download and install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Add it to your local bin if it's not there
sudo mv bin/arduino-cli /usr/local/bin/
```

Then, make sure your user has the necessary permissions to talk to the USB-to-Serial chip on the ESP32.
```bash
sudo usermod -a -G dialout $USER
# Note: You MUST log out and log back in for this to work!!!!
```

Once you log out and log back in, open a new terminal, and setup the ESP-32 core to allow `arduino-cli` to be able to program ESP32 micro-controllers.
```bash
arduino-cli core update-index
arduino-cli core install esp32:esp32
```

Now you have the tools necessary to compile your  .ino/.c/.cpp files. You must now install the tool to be able to flash them to your micro-controller. For this, use the `esptool` python library.
```bash
# Go to your project directory, where your main.ino/main.c/main.cpp file is.
cd ./myproject
# Setup a python virtual environment
python3 -m venv .venv
source .venv/bin/activate
# Install the esptool library
pip install esptool
# Note: Always make sure you are using the virtual environment when working!
```


### 2. Compilation and Flashing

Compiling your .ino file to a .bin file:
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 --output-dir ./build project.ino
# make sure you are in the virtual environment!

# compile: tells the tool to simply compile the binary

# --fqbn: Fully Qualified Board Name <platform>:<architecture>:<board>
# The first 2 arguments esp32:esp32 are generally non-variable for ESP32 development. The third argument is the specific board name, i.e: esp32dev, esp32s3, esp32c3, esp32cam, etc.

# ---output-dir: simply the directory for the output bin files.
```

Flashing your .bin file to the micro-controller
```bash
esptool --chip esp32 --port /dev/ttyUSB0 write_flash -z 0x10000 ./build/project.ino.bin

# --chip argument specifies the chipset you are building for
# --port argument specifies the serial port your chip is on
# --z argument specifies the starting memory location for your code in the flash memory of hte microcontroller. 0x10000 is the default starting location for (most) ESP32 chips.
```

The above commands should hopefully build your code and flash it, with a clear success indicator on the terminal.



### 3. Testing, Debugging

Find all serial connections your computer has:
```bash
ll /sys/class/tty
```

Check the serial connection to ESP32 and get its system information:
```bash
esptool --port /dev/ttyUSB0 flash_id
```

Monitor the serial communication between your computer and the ESP32
``` bash
python3 -m serial.tools.miniterm /dev/ttyUSB0 115200

# Use the appropriate baudrate as the one on your microcontrollers code!
```

Clear all serial connections to a specific serial port in case of errors such as "Serial port is already in use" when you try to access the serial port:
```bash
sudo fuser -k /dev/ttyUSB0
```
