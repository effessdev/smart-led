# Types in C

## Overview

In C, **types** define the kind of data a variable can hold and the operations that can be performed on it.

---

## Primitive (Built-in) Types

```c
int age = 25;           // Integer
float price = 9.99f;    // Single-precision float
double pi = 3.14159;    // Double-precision float
char grade = 'A';       // Single character
_Bool active = 1;       // Boolean (C99)
```

---

## `typedef` — Type Aliases

`typedef` gives a new name to an existing type, improving readability:

```c
typedef unsigned long uint32;
typedef float brightness_t;

brightness_t level = 75.5f;  // same as: float level = 75.5f;
```

> In ESP-IDF, types like `ledc_channel_t` and `ledc_timer_t` are defined this way.

---

## `struct` — Composite Types

Group multiple related fields into a single type:

```c
struct led_config {
    int gpio;
    int channel;
    float brightness;
};

struct led_config config;
config.gpio = 18;
config.brightness = 50.0f;
```

Combined with `typedef` for cleaner usage:

```c
typedef struct {
    int gpio;
    int channel;
    float brightness;
} led_config_t;

led_config_t config = { .gpio = 18, .channel = 0, .brightness = 50.0f };
```

---

## `enum` — Named Constants

Define a set of named integer constants:

```c
typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINKING
} led_state_t;

led_state_t state = LED_BLINKING;  // internally an int (0, 1, 2)
```

---

## `union` — Shared Memory

Overlay different types in the same memory location:

```c
typedef union {
    uint32_t value;
    uint8_t bytes[4];
} data_t;

data_t d;
d.value = 0x12345678;
// d.bytes[0] == 0x78 (on little-endian)
```

---

## Pointers

A pointer holds the memory address of another type:

```c
int x = 10;
int *ptr = &x;    // ptr holds the address of x
*ptr = 20;        // x is now 20
```

---

## Summary

| Mechanism        | Purpose                                      |
|------------------|----------------------------------------------|
| `int`, `float`, `char`, etc. | Built-in primitive types         |
| `typedef`        | Alias an existing type for readability       |
| `struct`         | Group multiple fields into one composite type|
| `enum`           | Define a set of named integer constants      |
| `union`          | Overlay different types in the same memory   |
| `*` (pointer)    | Reference the memory address of a type       |
