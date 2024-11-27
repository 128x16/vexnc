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

## Fan Mail
Here are some raving reviews from satisfied customers of VexNC™.
As you can see, many people find VexNC to be useful and reliable software.

- "can you not" (from litevex)
- "kill yourself" (from some guy running a vnc scanner)
- "Another absolutely incomprehensible one: this server is that of @mastodon.staropa.net@1024x2, and it seems like the hacker is Litevex, but he claims to have hacked htwins.net. 😵‍💫" (from [A Vanity VNC Server](https://ache.one/articles/a-vanity-vnc-server))
