# KeyStrokeLAN
### Authors: Kentalives & midtxwn @ Github
Lightweight an ultra-minimalistic CLI Application implemented through Windows API to replicate keystrokes from a client to a server node, enabling remote keyboard control. Originally created to play Split-Screen videogames using two computers. Keystroke data is sent through TCP protocol and lacks encryption.

# USAGE 
Application contains two components: client and server.  Client will send keystrokes, server will receive them and replicate them as if they were his.
## SERVER
To launch, run SERVER/NidhoggRemoteKeyboard.exe <p> , where <p> is the port where you wish the server to listen to (e.g, 5555).
## CLIENT
To launch, run CLIENT/x64/Release/NidhoggClientReal.exe <ip> <p>, where <ip> is the IP address of the server and <p> is the port where the server is listening.

# ARCHITECTURE
The server part is written in plain C using CLion and the client is written in c++ using Visual Studio.
