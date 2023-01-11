# Digit Adaptor

> It is brilliancy, disguised as bullshit, disguised as brilliancy.
-[John Byrd](https://www.quora.com/How-do-I-find-the-count-of-all-different-digits-in-a-number-in-C-C/answer/Joe-Zbiciak?comment_id=124725133&comment_type=2)

## License

Everything in this project is authored by me (Joe Zbiciak,
joe.zbiciak@leftturnonly.info), and is licensed under the Creative Commons
Attribution-ShareAlike 4.0 International license, aka.
[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

## Background

Quora receives an endless stream of homework questions.  Some time back, I
found myself frustrated with a spate of homework questions that asked students
to manipulate digits in a decimal number in a manner that's more consistent
with manipulating a string.  For example:

* [How do you switch the first and last digit of a number in C++?](https://www.quora.com/How-do-you-switch-the-first-and-last-digit-of-a-number-in-C)
* [How can you swap the first and last digit of a number without using any functions?](https://www.quora.com/How-can-you-swap-the-first-and-last-digit-of-a-number-without-using-any-functions)

...and so on.

Now in many cases, the best answer (in real life) is to convert the number to
a string (or leave it as a string if you're receiving it from a user), and
manipulate its digits as characters in a string.

_However,_ that doesn't mean there isn't a more, erm, _creative_ approach.

My `digit_adaptor` class template will bind a variable with integer type, or
a type that behaves sufficiently like an integer type, and treat it as a
container of digits in a specified radix.  The default radix is 10 (decimal).

As a random-access container, you can obtain random-access iterators to hand
to standard algorithms, access elements with `operator[]`, etc.

## The Adaptor: How It Works

This adaptor obtains a reference to the underlying integer type.

By default, it will set the container size based on the number of digits in the
integer's current value.  The value 0 gets one digit.  Alternately, you can
specify the number of digits the container should hold.

For signed types, the adaptor tries to preserve the sign; however, if you zero
out all the digits, the resulting integer is positive from that point forward.

The adaptor uses division and modulo to read individual digits, and to replace 
them.  It's not particularly optimized at present, preferring correctness to
performance.  However, it is largely `constexpr`, and I've observed the C++
compiler reducing many programs that use `digit_adaptor` to a compile-time
constant expression.


____

Copyright © 2023, Joe Zbiciak <joe.zbiciak@leftturnonly.info>  
`SPDX-License-Identifier:  CC-BY-SA-4.0`
