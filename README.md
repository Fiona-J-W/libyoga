
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

TODO

License
--------
The library is licensed under GPLv3 or any later version. If this is incompatible to your prefered
FLOSS-license contact the author to see what can be done for you.

If you want to use libyoga in a proprietary product, contact the author to purchase a nonfree license.
