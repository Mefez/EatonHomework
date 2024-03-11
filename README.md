# EatonHomework
This repository is consist of the project built in Visual Studio 2022<br>
The main code file is EatonHomework.cpp, compiled by C++ 17 compiler<br>
Since this project use winsock library, the project should be compiled in Windows machine, I am using Windows 10.<br>
More instruction to use winsock are provided under "How to run" section<br>

# Problem Statement
The problem to solve is following:

                You monitor devices, which are sending data to you.

                Each device has a unique name.

                Each device produces measurements.

 

The challenge is:

                Compute the number of messages you got or read from the devices.

# Solution
The Monitor function is the solution for the problem statement, the other functions is to create and handle mock devices to connect to Monitor function. The Monitor function binds a port and listens for data using TCP/IP, extracts device ip and keep the unique devices' message counts in an unordered map. During shutdown the result in printed to the screen.<br>
The Device class creates and keeps 10 character long data, first two character are id of the device<br>
The Mock_device function creates a device and sends the data of the device to Monitor function using TCP/IP until termination flag raised<br>
The Shutdown function raises the termination flag and sends a null data to Monitor to prevent the deadlock<br>
The main function initializes winsock, calls the asynchronous threads (first thread is monitor, the rest is the mock devices) and wait untils the a keyboard input. After the keyboard input, shutdown function is called and winsock is terminated.<br>

How tu run:
1) The Virtual Studio Code 2022 should be downloaded in a Windows Machine<br>
2) Pull the project to the Virtual Studio<br>
3) Be sure the compiler is using C++17 (In Solution explorer right click to project -> properties -> Configuration properties -> C/C++ -> Language -> C++ Language Standart -> from the dropdown menu ISO C++ 17 standart -> apply and accept)<br>
4) Winsock preprocessor defitions should be added (In Solution explorer right click to project -> properties -> Configuration properties -> C/C++ -> Preprocessor -> edit Preprocessor Deifitions -> add "_WINSOCKAPI_" and "_WINSOCK_H_" to the bottom of top list-> apply and accept)<br>
5) Additional input dependencies should be added to Linker (In Solution explorer right click to project -> properties -> Configuration properties -> Linker -> Input -> edit Additional Dependencies -> add "ws2_32.lib" to the bottom of top list-> apply and accept))<br>
6) Press "F5" to start Local Windows Debugger
