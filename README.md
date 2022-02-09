# ESP MESH remote control app
Project contains source codes for: embedded devices based on ESP32, desktop application for communication and control.

The esp32 software organizes the devices into a mesh network, raises the ASIO server on the root device, configures the DAC output specified in project_defs.h on each device, and uses the messages received by the server to control the operation of the DAC. The desktop application establishes a connection with the ASIO server of the root device, allows you to get data on all devices in the mesh network, configure the DAC of each device to work with the desired duty cycle, and assign DAC on/off control to the keyboard keys. After completing all the settings, the user enters the control mode by pressing the <start> button in the application.
