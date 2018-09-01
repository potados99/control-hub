# control-hub

An Arduino sketch and a small C app for serial communication.

## Summary

Arduino gets order via UART from control machine.

It can manipulate GPIO on 'set' order and can return the status of specific GPIO pin on 'get' order.

All commands are delivered by 'control' app, which is the only recommended entry for controlling arduino on the machine.


## Safely connect one-by-one

It takes some time sending and reading data through serial.

It is safe to make it synchronous so that only one control task is allowed at one time.

When control process is running, others must wait for the task is done.

This is implemented using pid file. 

On startup, the app appends its pid to the file just like a queue, and waits for its turn comes.

At the end of the task, the app removes its pid from file, so the others can continue.

## Commands

GPIO controlled devices can have two characteristics, *power* and *pwm*.

Currently arduino supports three devices: `LIT`, `LED`, `FAN`.

These devices can be controlled using following commands.

**Set commands**
~~~
[device] [ON | OFF]
~~~
~~~
[device] [BRT | SPD] [arguments for brightness or speed (0-100)] // pwm characteristic.
~~~

**Get commands**
~~~
[device] ST [PWR | BRT | SPD]
~~~

**Returns**

Returned value for set commands is T or F.
For get commands is ON / OFF or number 0-100.

