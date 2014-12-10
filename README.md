# AVRFORTH

This is a proto-Forth REPL for the ATmega series (aimed at the
ATmega328P). It enables interactive exploration of the connections
to the microcontroller. It operates on a stack of unsigned 16-bit
integers.

Not supported yet:

* Defining new words
* Conditionals

Limitations:

* Stack is limited to 16 values
* Only the GPIO ports are supported right now


## REPL

Words are separated by spaces or newlines.

The REPL may be interacted with via a 9600-baud serial connection. It is
not very user-friendly; on most types of error, it resets the processor
to keep the software from getting into an out-of-control state (see the
ERROR messages below for details).


## IMMEDIATES AND PORTS

Immediates come in two varieties:

* #NNNNN is a decimal number
* $NNNN is a hexadecimal number

Read and write locations are specified as

* DDRA: 0
* PORTA: 1
* DDRB: 2
* PORTB: 3
* DDRC: 4
* PORTC: 5
* DDRD: 6
* PORTD: 7


## DICTIONARY

Note on the stack diagrams: the stack grows to the right. Therefore,
given

```
a b c
```

`c` is the most recently pushed value.



### `+`

```
+ : x y -- z
```

`+` takes two arguments from the stack and adds them together.

### `-`

```
- : x y -- z
```


### `R`

```
R : x -- y
```

`R` reads the port `y` as specified in the IMMEDIATES AND PORTS section.

### `W`

```
W : x y --
```

`W` writes `y` to the port `x` as specified in the IMMEDIATES AND PORTS section.

### `~`

```
~ : x -- y
```

`~` performs a bitwise inversion on `x`.

### `&`

```
& : x y -- 
```

`&` performs `x &= y` on port `x`.

### `|`

```
| : x y -- 
```

`|` performs `x |= y` on port `x`.

### `!`

```
! : x y --
```

`!` performs `x ^= y` on port `x`.

### `<<`

```
<< : x y -- z
```

`<<` is the logical left-shift, shifting `y` left by `x` places.

### `>>`

```
>> : x y -- z
```

`>>` is the logical right-shift, shifting `y` right by `x` places.

### `.`

```
. : x --
```

`.` peeks at and displays the top value in the stack.

### `DUP`

```
DUP : x -- x x
```

`DUP` duplicates the top value on the stack.


## ERROR MESSAGES

The error messages are necessarily terse to preserve memory.

* NOMEM: no memory; a malloc failed. The processor will reset.
* SUN: stack underflow. The processor will reset.
* SOV: stack overflow. The processor will reset.
* EIMM: error in immediate; the immediate is either too large for an
  unsigned 16-bit integer, or it was formatted wrong
* BADMEM: the memory location referred to is invalid.
* UNK: the word is not in the interpreter's dictionary.


## EXAMPLES

Given an LED hooked up to pin 14 (PB0, or digital pin 8 on the
Arduino), the following sequence will turn on the LED.

```
$2
$1
$3
#1
W
W
```

To turn off the LED:

```
$3
#0
W
```

As words are accepted, the REPL will display "OK":

```
Terminal ready
OK
READY
$2 OK
$1 OK
$3 OK
#1 OK
W OK
W OK
$3 OK
#0 OK
W OK
#1 OK
#7 OK
<< OK
PRN OK
80
```


## OUTRO

This REPL is fairly primitive, and was hacked together pretty
quickly. It needs a lot of cleaning up, and probably some
reworking. That being said, it accomplishes its goal of letting me
explore the microcontroller's peripherals.

Current program size:

```
avr-size avrforth.hex
   text    data     bss     dec     hex filename
      0    3288       0    3288     cd8 avrforth.hex
```


