Netstring for C
===============
[![Build Status](https://travis-ci.org/liteserver/netstring-c.svg?branch=master)](https://travis-ci.org/liteserver/netstring-c)

A [netstring](http://en.wikipedia.org/wiki/Netstring) is a way of encoding a sequence of bytes for transmission over a network, or for serialization. They're very easy to work with. They encode the data's length, and can be concatenated trivially. The format was [defined by D. J. Bernstein](http://cr.yp.to/proto/netstrings.txt) and is used in various software. Examples of netstrings:

    "12:hello, world!,"
    "3:foo,"
    "0:,"

To specify a list of strings, just concatenate the netstrings. The list ["hey", "everyone"] can be encoded as

    "3:hey,8:everyone,"

This is some code in C for netstring serialization and deserialization. It checks netstrings for validity when parsing them, and guards against malicious input. It's also fast.

Basic API
---------

All the code is in `netstring.c` and `netstring.h`, and these have no external dependencies. To use them, just include them in your application. Include `netstring.h` and link with the C code.

Creating netstrings
-------------------

You can create your netstrings manually like in this example:

```C
sprintf(buf, "%lu:%s,", strlen(str), str);
```
    
This code provides a convenience function for creating netstrings:

```C
size_t netstring_add(char **netstring, char *data);
```

Here is how to use it:

```C
char *netstring=0;  /* we must initialize it to zero */

netstring_add(&netstring, "first");
netstring_add(&netstring, "second");
netstring_add(&netstring, "third");

do_something(netstring);

free(netstring);    /* we must free after using it */
```

The extended version `netstring_add_ex` accepts a string length as the last argument:

```C
size_t netstring_add_ex(char **netstring, char *data, size_t len);
```

This allocates and creates a netstring containing the first `len` bytes of `data`. If `len` is 0 then no data will be read from `data`, and it may be null.

Parsing netstrings
------------------

To parse a netstring use `netstring_read()`:

```C
int netstring_read(char **buffer_start, size_t *buffer_length,
                   char **netstring_start, size_t *netstring_length);
```

It reads a netstring from `buffer_start` of initial `buffer_length` and writes
to `netstring_start` a pointer to the beginning of the string in the
buffer and to `netstring_length` the length of the string. It also updates
the `buffer_start` to the start of the next netstring item and `buffer_length`
to the number of remaining bytes not processed in the buffer.

It does not allocate any memory.

### Return Value

If it reads successfully then it returns 0. If there is an error then the
return value will be negative. The error values are:

    NETSTRING_ERROR_TOO_LONG      More than 999999999 bytes in a field
    NETSTRING_ERROR_NO_COLON      No colon was found after the number
    NETSTRING_ERROR_TOO_SHORT     Number of bytes greater than buffer length
    NETSTRING_ERROR_NO_COMMA      No comma was found at the end
    NETSTRING_ERROR_LEADING_ZERO  Leading zeros are not allowed
    NETSTRING_ERROR_NO_LENGTH     Length not given at start of netstring

Usage Example:

```C
char  *str, *base = buffer;
size_t len,  size = bytes_read;

while(netstring_read(&base, &size, &str, &len) == 0) {
  do_something(str, len);
}
```

We can replace the comma with a null terminator when reading (zero copy):

```C
while(netstring_read(&base, &size, &str, &len) == 0) {
  str[len] = 0;
  puts(str);
  str[len] = ',';   /* and optionally restore it */
}
```

If you're sending messages with more than 999999999 bytes (about 2
GB) then you probably should not be doing so in the form of a single
netstring. This restriction is in place partially to protect from
malicious or erroneous input, and partly to be compatible with
D. J. Bernstein's reference implementation.

Message Framing on stream-based connections (sockets, pipes...)
---------------------------------------------------------------

On stream-based connections the messages can arrive coalesced or fragmented.

Here is an example of reading those messages using netstring for message framing:
```C
char buffer[1024], *buffer_base, *str;
int bytes_read, buffer_used = 0, len;

while(1) {
  /* read data from socket */
  bytes_read = recv(sock, &buffer[buffer_used], sizeof(buffer) - buffer_used);
  if (bytes_read < 0) break; if (bytes_read == 0) continue;
  buffer_used += bytes_read;

  /* parse the strings from the read buffer */
  buffer_base = buffer;
  while(netstring_read(&buffer_base, &buffer_used, &str, &len) == 0) {
    do_something(str, len);
  }

  /* if there are remaining bytes, move to the beginning of buffer */
  if (buffer_base > buffer && buffer_used > 0)
    memmove(buffer, buffer_base, buffer_used);
}
```
Note: this example is lacking error checking from netstring_read function and it does not allocate memory for bigger messages.

Additional Functions
--------------------

### netstring_list_size

Retrieves the size of the netstring list (concatenated netstrings) discarding trailing spaces.

```C
int netstring_list_size(char *buffer, size_t size, size_t *ptotal);
```

### netstring_list_count

Retrieves the number of items in a netstring list.

```C
int netstring_list_count(char *buffer, size_t size, int *pcount);
```

Contributing
------------

All this code is Public Domain. If you want to contribute, you can send bug reports, or fork the project on GitHub. Contributions are welcomed with open arms.
