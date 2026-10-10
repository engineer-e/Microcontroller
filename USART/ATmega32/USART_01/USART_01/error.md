/*
 * error.md
 *
 * Created: 10/10/2026 13:14:43
 *  Author: Hulk
 */ 

 Your original ATmega32 UART code has 6 main mistakes. The compiler error you reported is related to the `sendText()` function call, while the other mistakes can cause incorrect string transmission or buffer problems.

## 1. Compiler error in `sendText()`

Your original code:

c
```
void sendText(char *txt)
{
    while (*txt)
        uartTransmit(*txt++);
}
```

You call it with a string literal:

c
```
sendText("ATMEGA32 AVR UART Example 1\n\r");
```

Some AVR compiler configurations treat string literals as being in a different address space from ordinary character arrays.

Correction:

c
```
void sendText(const char *txt)
{
    while (*txt)
        uartTransmit(*txt++);
}
```

This is the likely fix for your reported compiler error. If it persists, the exact compiler and memory model need to be checked.

## 2. `tmp` is used before initialization

Your code:

c
```
char tmp, i = 0;
```

Later, you check:

c
```
if (tmp == 0x0D)
```

This check runs even when no character has been received. Initially, `tmp` contains an indeterminate value.

Correction: Move the Enter-key check inside `if (uartReady)` after `tmp = uartReceive();`.

## 3. Missing null terminator in the string

Your code stores characters like this:

c
```
tmp = uartReceive();
txt[i] = tmp;
i++;
```

But `sendText(txt)` expects the string to end with `'\0'`. Without it, the function may read beyond the array and transmit unwanted characters.

Correction:

c
```
txt[i] = '\0';
sendText(txt);
```

Do this before sending the string.

## 4. Buffer overflow

Your buffer is:

c
```
char txt[15];
```

It holds at most 14 text characters plus the null terminator. Your original code increments `i` without checking the buffer size:

c
```
txt[i] = tmp;
i++;
```

Receiving too many characters writes outside the array.

Correction:

c
```
if (i < sizeof(txt) - 1)
{
    txt[i++] = tmp;
}
```

## 5. Incorrect buffer-clearing logic

Your original code:

c
```
while (i > 0)
{
    txt[i] = '\0';
    i--;
}
```

This does not clear `txt[0]`, and it is unnecessary if you correctly terminate the string and reset the index.

Correction:

c
```
i = 0;
```

## 6. Duplicate UART receiver enable

Your code:

c
```
UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXEN);
```

You enabled `RXEN` twice. This does not normally break UART operation, but it is redundant.

Correction:

c
```
UCSRB = (1<<RXEN) | (1<<TXEN);
```

## Summary

| Mistake | Effect |
| --- | --- |
| `char *txt` used with string literal | May cause your reported compiler error |
| `tmp` used outside the receive condition | Unpredictable Enter-key check |
| Missing `'\0'` | Incorrect string transmission |
| No buffer-size check | Memory corruption |
| Incorrect buffer-clearing loop | Leaves the first character uncleared |
| Duplicate `RXEN` | Redundant configuration |

The most important fix for your reported error is changing `char *txt` to `const char *txt`. The most important runtime fix is handling the Enter key only after a character has actually been received, then terminating the string before sending it.