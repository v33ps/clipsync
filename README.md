# clipsync
sync your clipboard contents between computers
This program works in two parts. There is a server, and a client. The server listens on a given port for new clipboard content and sets the clipboard accordingly. The client reads the clipboard content and sends it accordingly.

## Support
At the moment the only platform supported is Linux using `Gtk2.0+`

## Build Instructions
To build the client run:
```
gcc -g client/main.c `pkg-config --cflags --libs gtk+-2.0`
```

To build the server run:
```
gcc -g server/*.c `pkg-config --cflags --libs gtk+-2.0`
```

If these fail, you probably need to install the `gtk-dev` package:
```
sudo apt-get install build-essential libgtk2.0-dev
```

To make sure you have this package, run:
```
pkg-config --cflags --libs gtk+-2.0
```