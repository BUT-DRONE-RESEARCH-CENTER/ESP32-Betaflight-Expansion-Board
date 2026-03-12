

# Small checks and steps
```
1. fixed the solder issue.
2. ELRS Voltage between TX and GND is 3.2-3.3v. it can directly be connected to esp32 rx and gnd.
```



# Updates
```
Setup the ESP32 Uart sniffing logic. The ESP32 DEVKIT V1 has a common ground with the ELRS module connected to the FCU,
and the ELRS modules' TX pin is also routed to the RX pin (GPIO 16) of the ESP32 DEVKIT V1.

The ESP32 DEVKIT V1 utilizes the AlfredoCRSF protocol to decode the crossfire protocol,
and then, it outputs it into the serial monitor (or plotter) in a channel:value format.

This was merely a proof of concept to verify packet sniffing of ELRS is functional from a secondary microcontroller,
as this is the ony way to ensure the peripheral expansion board will not be interfering with the flight computers operation.

Thankfully due to the high impedance of the ESP32 DEVKIT V1's UART RX pin, it is possible.
```

# Next Steps
```
Main goal for the next week is setting up a web based firmware flashing tool prototype.
The idea will be setting up a very simple website, with statically generated BIN files (No Dynamic code generation needed yet. It's just a proof of functionality).
The user will then select the USB port of the microcontroller, and by selecting any arbitrary button which corresponds to a given BIN file, flash the firmware.
```
