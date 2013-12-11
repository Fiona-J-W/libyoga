
libyoga - modern output for C++
===============================


Motivation
----------
If C++ programmers want to print something to the commandline the basicaly have the choice between
the old C-functions from cstdio or the slightely newer C++-streams. Both interfaces have big problems:

The printf-family is extremly dangerous and stupid:
* Using a slightely wrong format-string results in undefined-behaviour.
* The functions are unable to infer the types of their arguments, they have to be passed via
	the format-string.
* Writing portable code with typedefed integer-types get's ridicolous. The correct solution is to
	include inttypes.h which provides macros for the right types. this results in calls like
	this: `printf("foo%" PRI64 "bar, uint64_t{0});`


While streams solve these problems they create several themselves:
* Formatting output with streams is extremely verbose. To print an uint64\_t with 16 characters
	(leading zeros) in hex, one has to write this:
	`stream << std::setw(16) << std::setfill('0') << std::hex << uint64_t{2};`
* Some of these manipulaters keep persistent until they get explicitaly reset. Others will be reset
	after the next call. This is almost as inconsistent as php.
* Since formating can change the way how stuff will be printed forever, streams may either get messed
	up or have to be cleaned manually. This means that the solution in the first point is actually
	incomplete. The real ssolution would be this:
	`stream << std::setw(16) << std::setfill('0') << std::hex << uint64_t{2} << std::setw(0) << std::dec;`

Since this situation is clearly less then great, libyoga was invented to get rid of all of these
disadvantages and create few new ones.

Name
----
Naming is sometimes considered one of the hardest things in software-engeneering. Therefore the author
of this library ran pwgen to create a suitable name that doesn't clash. The output was 'YOG'. Since
yoga is only one letter longer, very easy to pronounce, doesn't clash with existing libraries and is
often associated with relaxation (which is what this library is intended to bring to programmers) it
was choosen as name.

Functionality
-------------
The library provides a function-template yoga::format(format, ...) that creates a string based on
the provided formatstring and the further arguments (somwhat like sprintf). All supported types can be
printed with the identifier %s:

```c++
yoga::format("foo%sbar%sbaz%%", 1, "bla") == "foo1barblabaz%"
```

Some types, for instance char\*, also provide other identifiers which will print them in other ways:

```c++
yoga::format("char* as str: %s, char* as pointer: %p", "foo", "bar")
	== "char* as str: foo, char* as pointer: 000000000000fe25"
```

Sometimes one want's to change the way something is printed only to a small degree. This is what
format-arguments are for:

```c++
yoga::format("11 in hex: %_16*0~2s", 11) == "0b"
```

As you see it is possible to combine them too. Every format-argument is identified by a special
character followed by one or more characters that set it's value. Since letters are used to end the
list of format-arguments they are not allowed here.

The following format-arguments exist:
* '\_' =  base: The base in which integers are printed, default is 10
* '@' = index: The index of the argument. Normally the function-arguments are used for printing in the
	order, in which they were provided; this format-argument can be used to disable this 
	functionality for one format-specifier.
* '\*' = fill: If an argument wouldn't fill the requested length, the missing characters will be
	filled up with this symbol. Default = ' '
* '~' = width1: The minimum amount of characters that should be printed. Missing ones will be filled up
	with the fill-character. Default = 0
* '.' = width2: A second width-argument, mainly inted for floating-point-numbers to specify the number
	of decimals. Default = 10

The functionality is probaly best shown by some examples:

```c++
using yoga::format;
format("%s, %@0p, %s", "foo", 3) == "foo, 000000000000fe25, 3";
format("a string with no args") == "a string with no args";
format("char: %s, unsigned char: %s, signed char: %s", 'c', uint8_t{1},
	int8_t{2}) == "char: c, unsigned char: 1, signed char: 2";
```

Target
------
libyoga tries to be an easy to use and extend library for printing stuff. Performance is one but not
the only goal, since safety is considered more important.

libyoga tries hard to be compatible with the C++-standards-library, *except for stdio and streams*.
This is done to both achieve performance and discourage mixing of several libraries that serve the same
purpose.

libyoga can be built using only what the STL-provides, but it profits enormously from also being
allowed to use posix-features.

License
--------
The library is licensed under GPLv3 or any later version. If this is incompatible to your prefered
FLOSS-license contact the author to see what can be done for you.

If you want to use libyoga in a proprietary product, contact the author to purchase a nonfree license.
