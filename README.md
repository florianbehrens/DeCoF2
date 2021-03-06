[![Travis CI Build Status](https://travis-ci.org/florianbehrens/DeCoF2.svg?branch=master)](https://travis-ci.org/florianbehrens/DeCoF2) [![AppVeyor Build status](https://ci.appveyor.com/api/projects/status/tc6w5v0uo3o6lele?svg=true)](https://ci.appveyor.com/project/florianbehrens/decof2) [![Coverage Status](https://coveralls.io/repos/florianbehrens/DeCoF2/badge.svg?branch=master&service=github)](https://coveralls.io/github/florianbehrens/DeCoF2?branch=master) [![Documentation Status](https://codedocs.xyz/florianbehrens/DeCoF2.svg)](https://codedocs.xyz/florianbehrens/DeCoF2/)

DeCoF2
======

Introduction
------------

DeCoF is an acronym of the phrase *Device Control Framework*. DeCoF is a
framework implemented in C++ that might be useful for a very common problem in
embedded software engineering:

You engineer an embedded device of some sort and want to remote control that
device from a PC or any other electronic device.

DeCoF is based on a, well, let's name it *data centric* approach. Data centric
in this context means that interaction between the device and the remote
control instance is purely based on modification of state of named objects
rather than sending a bunch of messages back and forth.

Concepts
--------

### System configuration

DeCoF distiguishes clearly between two distinct roles: There is a server that
'owns' a load of objects and one or more clients that want access to some or 
all of those objects.

The server consists of the server implementation that is coded against the
DeCoF framework. The clients are most probably coded with some other framework.
Both of which communicate over some connection using some protocol. Both are
not finally defined by DeCoF but some useful implementations are provided.

### Objects, parameters, events, and nodes

Objects are organized in a hierarchic tree structure. Each object has a name
and exactly one parent. The only exception is the 'root' object which does not
have a parent.

There a various different object types: Only *node* objects can hold child objects.
*Parameters* are associated a value of a predefined, fixed value type. *Events* are 
objects without value but can be used to signal some event.

Parameter values can be of one of the following builtin value types:

* boolean
* integer
* real
* string
* binary
* a sequence type of each of the above listed primitive value types
* a tuple type of any combination of the above listed primitive value types.

A node is a special parameter with value type sequence of strings that holds 
the names of its child parameters.

From the client's perspective, parameter values can be either readonly, 
writeonly, or readwrite. For readwrite parameters there is a special rule: 
Since those are for modification of the client side the framework does not 
support modification by the server implementation. As a best practice a 
modification operation on a writable parameter (i.e., writeonly and readwrite 
parameters) should be idempotent, i.e., the side effects on such an operation 
should be the same if carried out once or multiple times.

### Access control

Clients access to objects is controlled by *access control levels*. Each object
is assigned a *writelevel* for modifying operations (i.e., setting a parameter
value or signalling an event) and/or a *readlevel* for non-modifying operations.
Readonly parameters obviously only support a readlevel while writeonly
parameters only have a writelevel.

Each client is assigned a *userlevel*. If a client wants to perform an operation
on an object it must have a userlevel that is greater or equal to the objects
respective access control level (i.e., readlevel or writelevel, respectively).

The following userlevels are supported:

* Normal (0)
* Service (1)
* Maintenance (2)
* Internal (3)

The semantics of the various userlevels are project specific.

### Object dictionary

The *object dictionary* is the container of all parameter instances. The object
dictionary itself is a node with an implementation specific name. All parameters 
are children or successors of the object dictionary node (i.e., the 'root'
object).

### Interaction models

The server framework supports two distinct models for interaction between
clients and server:

* Request/response model
* Publish/subscribe model

#### Request/response model

In the 'classic' request/response model, the server framework allows the
following abstract operations from clients on each parameter in the object
dictionary:

* Get operation (readable parameters and nodes only)
* Set operation (modifiable parameters only)
* Execute operation (events only)

#### Publish/subscribe model

In the publish/subscribe model, a client takes the role of a subscriber whereas
the server has the role of a publisher. In this model the server framework
allows the following abstract operations on each readable parameter and node in
the object dictionary:

* Subscribe operation
* Unsubscribe operation
* Notify operation

Subscribers can initiate subscribe und unsubscribe operations, whereas the
notify operation is carried out on value changes by the publisher.

All those mentioned operations can be realized by messages sent back and forth
in any protocol. The framework provides some useful protocols and even allows to
write your own.

### Discovery

Discovery is a term that describes the process of finding out about the
parameters available in a server's object dictionary. This can be carried out
by traversing the parameter tree from node to node or by special discovery
means.

Implementation
--------------

### Server side API

The server side API strives to model the above described concepts in a simple
and convenient way. The following classes represent mentioned basic concepts:

* ```decof::object_dictionary``` represents the object dictionary
* ```decof::node```represents a node object
* ```decof::event```represents an event

Parameters are represented by a bunch of classes depending on their properties.
Firstly, parameters are distinguished by their value type which is determined
by a template type argument. Secondly, they are disciminated by the access mode
they provide, i.e., whether they are readonly, writeonly, or readwrite. Thirdly,
readable parameters are divided into *managed* and *external* parameters: A
managed parameter holds the associated value within the parameter object, i.e.,
the value is 'managed' (or encapsulated) by the parameter object. An external
parameter is only an empty wrapper with no value inside and provides getter and
setter functions for access to the actual value. The obvious difference is, that
managed parameters have full control over value accesses while external
parameters don't. There are no managed writeonly parameters because writeonly
parameters are considered useful for cases where the value representation
differs significantly from the 'natural' representation of the respective type.

Hence, the list of classes representing parameters is as follows:

* ```decof::managed_readonly_parameter<T>```
* ```decof::managed_readwrite_parameter<T>```
* ```decof::external_readonly_parameter<T>```
* ```decof::external_readwrite_parameter<T>```
* ```decof::writeonly_parameter<T>```

All these classes provide (runtime) polymorphic functions (i.e., virtual
functions) for the user to (re)act on parameter value accesses by clients.
There are cases, however, where it is more convenient to (re)act on parameter
value accesses by means of a non-member handler function or delegate. This
pattern is supported by the following specialized 'handler' classes:

* ```decof::managed_readwrite_handler_parameter<T>```
* ```decof::external_readonly_handler_parameter<T>```
* ```decof::external_readwrite_handler_parameter<T>```
* ```decof::writeonly_handler_parameter<T>```

These classes provide a registration function for handlers of type
```std::function<>``` for each virtual function of the respective base class.

#### Overridable handler functions

The following table lists the overridable handler functions by parameter type.

Parameter    | Readonly                 | Readwrite                | Writeonly
-------------|--------------------------|--------------------------|----------
**Managed**  | None                     | ```verify(T)```          | N/a
**External** | ```T external_value()``` | ```T external_value()```<br>```external_value(T)``` | ```value(T)```

#### Parameter value access functions

The following table lists the value access functions by parameter type.

Parameter    | Readonly                                 | Readwrite       | Writeonly
-------------|------------------------------------------|-----------------|----------
**Managed**  | ```T value()```<br>```const T& value_ref()```<br>```value(T)``` | ```T value()```<br>```const T& value_ref()``` | None
**External** | ```T value()```<br>```value_changed()``` | ```T value()``` | None

### Protocols

#### General

There are several protocol implementations provided but the framework is
intentionally open for new implementations.

#### Command line interface protocol

The command line interface (CLI) protocol is around for historical reasons and
for its simplicity and usability when it comes to manual (terminal) interaction
with a server.

The CLI protocol implements both the request/response and publish/subscribe
interaction models as well as supports some simple discovery means.

##### Client/server model

The client/server interaction model is supported typically via TCP port 1998.
Operations are encoded in ASCII text in the following manner:

`[(] <operation> [']<path> [<value>] [)]`

The parantheses and the `'` are for historical reasons and optional.

`<operation>` can be one of:

* `param-ref` or `get` for a get operation
* `param-set!` or `set` for a set operation
* `exec` or `signal`for a execute operation

The former keywords are for historical reasons.

The value is only needed for set operations.

##### Publish/subscribe model

The publish/subscribe interaction model is supported typically via TCP port
1999. Operations are encoded in ASCII text in the same way as in the
client/server model.

`<operation>` can be one of:

* `add` or `subscribe` for a subscribe operation
* `remove` or `unsunscribe` for a unsubscribe operation

The former keywords again are for historical reasons.

##### Path representation

The `path` represents an object within the object dictionary with its fully
qualified name, seperated with a colon (`:`), e.g., `root:node:my_parameter`.

##### Value encoding

Values are encoded according to their type as follows:

* boolean: `#t` for true and `#f` for false
* integer: in stringified representation, e.g., `-123`
* real: in stringified representation (may be scientific) with decimal point,
  e.g., `-123.0` or `-1.23e2`
* string: enclosed in double quotes (`"`)
* binary: prefixed with a `&`
* sequences are enclosed in square brackets (`[]`) with the individual
  elements seperated by commas (`,`)
* tuples are enclosed in curly brackets (`{}`) with the individual elements
  separated by commas (`,`).

##### Discovery

The CLI protocol supports a simple dicovery means by using the operation
`param-disp` or `browse`. The output is a textual representation of the
objects in the object dictionary.

#### SCGI protocol

##### General

The
[Simple Common Gateway Interface](http://de.wikipedia.org/wiki/Simple_Common_Gateway_Interface)
protocol is used by many webservers such as Apache, Nginx, and Lighttpd, as 
backend protocol that overcomes the efficiency issues of 'classic' CGI. It is 
similar to FastCGI but easier to implement as it is not a binary protocol.

In conjunction with a webserver this protocol can be used with DeCoF2 servers
to provide webservices for interaction with the object dictionary.

Objects are identified by the HTTP path within a HTTP GET, HTTP PUT, or HTTP 
POST request (e.g., `/root/node/my_parameter`), respectively. This protocol 
only supports the client/server model.

HTTP GET requests are used to read readable parameters. To modify any writable 
parameter use HTTP PUT. Since the latter is required to be idempotent (i.e.,
multiple invocations result in the same side effects as a single one) by the 
HTTP/1.1 spec (RFC2616, clause §9.1.2) a HTTP POST request is required to 
invoke an event which likely modifies state in a non-idempotent way.

For historical reasons, a HTTP GET request to the special path /browse returns a
proprietary XML representation of the object tree.

##### Value encoding

This chapter specifies the value encoding that this implementation produces in 
GET requests and expects in PUT requests. The value encoding is chosen for best
interaction with Javascript and the
[XMLHttpRequest](http://en.wikipedia.org/wiki/XMLHttpRequest) function as being
standardized by the [W3C](http://www.w3.org/TR/XMLHttpRequest/).

For this reason, numeric primitive (integer, real) and boolean type values are
encoded as a Javascript literal that can be converted to a Javascript number
using the Number() global function. String and binary parameters are encoded as
is.

Sequences of numeric types are encoded for use with 
[Javascript Typed Arrays](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays).
Javascript Typed Arrays are binary data buffers that require a [Typed Array 
View](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays#Typed_array_views)
for access to the individual elements of the array. Multibyte numbers are always
encoded in little-endian byte order as clients will be x86-based in most cases.

* Sequences of boolean types are encoded in one octet per value being 1 for 
true and zero for false for use with `Uint8Array`.
* Sequences of integer types are encoded with four octets per value for use with
`Int32Array`.
* Sequences of real types are encoded with eight bytes per value according to 
double precision IEEE 754 for use with `Float64Array`.

Sequences of string and binary types are encoded with their individual elements
encoded as [Bencode strings](http://en.wikipedia.org/wiki/Bencode), separated by
CR+LF.

The individual elements of a tuple type are encoded

* like non-sequence types in case of a numeric type, and
* as a Bencode string in case of string and binary types

and separated by CR+LF.

### Dependencies

DeCoF2 has the following link-time dependencies:
* Boost 1.56 (required: system, optional: thread, unit_test_framework)
