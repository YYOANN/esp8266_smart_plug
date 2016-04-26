# Smart Plug Project for ESP8266

## Info
This is a home automation project to control your electrical appliances.
The project currently consist of a small HTTP server receiving requests, parsing them and responding.
I currently toggle one of the GPIO (16) of the ESP to turn on or off a solid state relay wich activate or deactivate the plug.

## Compilation
You need to have your toolchain configured. I use [esp-open-sdk][1].
To make the firmware you will need [esptool-ck][2]
Then you can modify the Makefile with good paths for SDK_PATH and ESPTOOL_CK_PATH.
You may need to modify the serial port on which your ESP is connected.

Then you can compile with the following sequence:
```
make driver
make firmware
```

To flash the ESP.
```
make burn
```

To clean you can do
```
make clean        # Clean only firmware
make clean-all    # Clean firmware and driver
```


## Usage
You need to modify the file main.c to statically configure you access point. Then the ESP should connect to your WiFi network and acquire a IP via DHCP.
Then you can access it on port 80, the '/' page should toggle the GPIO 16 and display a message on it's state.
[1]: https://github.com/pfalcon/esp-open-sdk
[2]: https://github.com/igrr/esptool-ck


