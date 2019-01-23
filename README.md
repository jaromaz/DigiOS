# DigiOS - mini OS for Digispark

The sketch allows you to log in to Digispark, execute several commands, and then log out. It works on the default Digispark with micronucleus bootloader installed, and uses the DigiCDC module to emulate USB communication, because Digispark itself does not have any additional USB chip and everything is done in the AtTiny85 software.

When powering the Digispark via the VIN pin, you can connect to it via USB at any time, and after performing the commands, disconnect the terminal and the device – Digispark will continue to execute the code taking into account to sent commands - as in [the video][1]:

[<img src="https://jm.iq.pl/wp-content/uploads/2019/01/digios-1-740x221@2x.jpg" width="100%">][1]
### Available commands

* **p[0–2] [on|off]** sends HIGH and LOW signals to individual pins
* **uptime** displays the time from launching the Digispark in the linux format uptime pretty
* **vcc** gives the power supply voltage of Digispark in millivolts
* **reboot** software restart of Digispark
* **clear** clears a display
* **temp** specifies the temperature of the chip
* **login** prompts you to enter your password
* **clock [0-7]** reduces clock timing
* **help** displays the help screen
* **logout, exit** logs out the user

Sketch was intended to take up as little space as possible – instead of strings I used character arrays, instead of pinMode/digitalWrite registers and bit operations, thanks to which it was possible to push so many functions on such a small device. All three components together occupy almost 100% of Digispark’s memory, but to increase the amount of available space for your own code, it is enough to remove the reference to unnecessary functions (e.g. temp, uptime, vcc). These references are marked in a special code block – after deleting them, more than 30% of the memory will be available (excluding bootloader).

You can log in to Digispark under Windows with Putty (after installing Digistump drivers), to Linux with Minicom application and to Android with Serial USB Terminal. Compilation of DigiOS sketch requires installation of an earlier version of Arduino IDE – 1.8.6 and correct installation of Digispark in IDE.

DigiLx is a Linux-style DigiOS - requires a root login and displays a linux prompt.

DigiOS is licensed under GNU General Public License v3.0

More information: [https://jm.iq.pl/digios][2]

[1]: https://www.youtube.com/watch?v=Ns_7tm4fF6s
[2]: https://jm.iq.pl/digios/

