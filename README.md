## CaTer

The original project is [CaTer](https://www.opppf.de/Cater/) for the C64. That project was ported to the Apple II and ATARI as part of [Telnet65](https://github.com/cc65/ip65/wiki/Telnet65).

### ATARI

The ATARI version of CaTer uses a 40x24 screen layout and writes directly to screen memory. Therefore no 80 column displays (e.g. XEP80 or [VBXE](http://spiflash.org/node/10)) are supported.

The ATARI system keyboard handler doesn't provide access to all potential control codes or special characters. Press `HELP` or `OPTION`+`H` to get help about entering those codes/characters.

The key combination `OPTION`+`Q` quits the current Telnet session.

In a shell, enter these commands for full functionality:
```
export LANG=C
export TERM=vt100
stty cols 40 rows 24
```

### Enhanced Apple //e

The Apple //e version of Cater doesn't support the VT100 character attributes _bold_ or _underline_. So you e.g. aren't able to see which files you have selected for a multi-file operation in Midnight Commander.

The four Apple //e cursor keys generate the same codes as the key combinations `Ctrl`+`H`, `Ctrl`+`J`, `Ctrl`+`K` and `Ctrl`+`U`. Therefore entering those key combinations doesn't send them to the Telnet server but instead sends the four VT100 cursor movement codes. However you can send the key combinations in question to the Telnet server by entering them while holding down the `Open Apple` key.

The key combination `Open Apple`+`Q` quits the current Telnet session.

In a shell, enter these commands for full functionality:
```
export LANG=C
export TERM=vt100
stty cols 80 rows 24
```

### C64

The C64 version of CaTer does **not** support a soft80 display. It makes use of the C64 color capabilities in two ways. Please read the [CaTer Docu - ESC Sequences](https://www.opppf.de/Cater/doku.html#esc) section _Select Graphic Rendition (SGR)_ to understand them.

You need to check out [CaTer Docu - Keyboard](https://www.opppf.de/Cater/doku.html#kbd) in order to know the mapping of C64 keys to VT100 keys. The eight C64 F-keys are mapped as well, but as the original VT100 terminal has only four F-keys the the rest of the C64 F-keys are not recognized by most remote programs.

The key combination `C=`+`Q` quits the current Telnet session.

In a shell, enter these commands for full functionality:
```
export LANG=C
export TERM=vt100
stty cols 40 rows 25
```

### Midnight Commander

The hints below are by no means specific to CaTer. They are rather just some useful general Midnight Commander usage info.

Use `ESC`+`1` to `ESC`+`0` as alternative to the ten F-keys. Press `ESC` twice to "escape". Use `Ctrl`+`T` to toggle the selection for multi-file operations.

On the C64 you can start Midnight Commander with the option `-c` to force the usage of colors although the original VT100 terminal doesn't support colors.
