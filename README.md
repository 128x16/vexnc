# vexnc
Dumb VNC server that simply displays a static image of your choosing to all clients on port 5900.
It can also log keypresses to a file. Useful for catching bots that scan the entire internet for exposed VNC servers.

## Usage
Requires libvncserver.

```
$ make CFLAGS="-O2 -march=native"
$ ./vexnc dipshit.png 'insert funny desktop name here' ~/vexnclogs
```

## TODO
- Allow choosing other ports than 5900
- Add compression to log files (you can just handle this with a cronjob though)
