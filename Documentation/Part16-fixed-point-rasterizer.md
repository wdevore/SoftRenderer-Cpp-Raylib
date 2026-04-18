A Fixed-Point Rasterizer

I've decided to use a library that implements 16.16 fixed-point math in C, called [libfixmath](https://code.google.com/archive/p/libfixmath/). You can simply download the library files and compile them with your project.

The library exposes a new type called **fix16_t**. Since fixed-point values can be simply stored as integer numbers, this new **fix16_t** type is basically just an alias to int32_t.

In 16.16 fixed-point, the integer number 1 is stored as 0x00010000 (in hexadecimal). We use 16 bits for the integer part, and 16 bits for the fractional part of our number. That is the same as 65536 in decimal.

The beauty of using integers to store 16.16 fixed-point numbers is that most of the common arithmetic we'll need works out of the box. We can just add and subtract the integer numbers, and the result translates for 16.16 fixed-point as well.

Now, since we are using C, we don't have *operator overloading*. Therefore, we will not be able to directly use the operators +, -, *, and / with this new **fix16_t** type. We'll need to use the functions provided by the library to add, subtract, multiply, and divide 16.16 fixed-point numbers.

You can look inside the **fix16.h** header file to see all the available functions provided by the library. The most important ones we'll use are:

```cpp
fix16_add(a, b)
fix16_sub(a, b)
fix16_mul(a, b)
fix16_div(a, b)
fix16_cos(theta)
fix16_sin(theta)
```
And that is pretty much it. Replacing all the **float** numbers by **fix16_t** and using the appropriate functions to add, subtract, multiply, and divide fixed-point numbers will solve most of our problems. Especially the one with our bias offset. Using 16.16 fixed-point numbers, we know that the smallest offset we can represent is ```-0x00000001```.

The trick here is that this -1 is not the integer -1, but -1 in 16.16 fixed-point math, which is a very small fractional number. To be more precise, this fractional number is *1/65536* (or 0.00001525878).

Download code: https://github.com/gustavopezzi/triangle-rasterizer-fix16