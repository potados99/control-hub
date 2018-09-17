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

Currently arduino supports these devices: 
- `LIT`
- `LED`
- `FAN`
- `ALM`

Power charateristic can be called `PWR`.

The property has two states `ON` and `OFF`.

PWM characteristic can be called these words: 
- `BRT` (for LED device)
- `SPD` (for FAN device)
- `VOL` (for ALM device)

Those properties can have state of number from `0` to `100`.

And the SET ONLY characteristics:
- `RPD` (rapid, power property) + seconds from `0`~
- `FADE` (pwm property) + `IN` or `OUT`

These devices can be controlled using following commands.

### Set

**Power control**
Setting power property doesn't require parameter `PWR`.
~~~
[device] [PWR] [arg]
~~~
or
~~~
[device] [arg]
~~~

Examples:
~~~
LIT ON
~~~
~~~
LED PWR OFF
~~~

**PWM control**
Those PWM characteristics are compatible each other.

For any device, any parameters such as `BRT` or `SPD` are available.
~~~
[device] [BRT | SPD | VOL | RPD | FAD ] [arg]
~~~

Examples:
~~~
LED BRT 50
~~~
~~~
ALM VOL 80
~~~

It is also POSSIBLE.
~~~
FAN BRT 100
~~~
It work same as `FAN SPD 100`.

For set only characteristics:
~~~
LIT RPD 2 // Rapid fire the light for 2 seconds
~~~
~~~
LED FADE IN
~~~

### Get
~~~
[device] ST [Power or PWM property]
~~~

### Returns

Returned value for set commands is `T\n` or `F\n`.

For get commands is `ON\n` or `OFF\n` or number `{0-100}\n`.

Every data recieved should have Line Feed character at the end of it.

### Errors

There are two errors it can handle.

One occurs when parameter is empty, with 3 times of beep.

The other occurs when the parameter is wrong, with 2 beeps.

