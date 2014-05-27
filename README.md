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
control instance is based on messages sent back and forth. Instead interaction
is purely done by modification of named parameter values.

Concepts
--------

### System configuration

DeCoF distiguishes clearly between two distinct roles: There is a server that
'owns' a load of named parameters and one or more clients that want access
(read, modify) to some or all of those parameters' values.

The server consists of the server implementation that is coded against the
DeCoF framework. The clients are most probably coded with some other framework.
Both of which communicate over some connection using some protocol. Both are
not finally defined by DeCoF but some useful implementations are provided.

### Parameters, commands, and nodes

As said, *parameters* are named values of a given, fixed type. *Commands* are
special parameters without a value. They can be used so signal some event.
Since parameters must have a parent, there are also *nodes* around, that can
hold child parameters.

Parameters can hold values of the following builtin types:

* bool
* integer
* real
* string
* and a sequence type for each of the above listed primitive types.

A node is a special parameter of type sequence<string> that holds the names of
its child parameters.

### Object dictionary

The *object dictionary* is the container of all parameter instances. The object
dictionary itself is a node. It has the name 'root'. All parameters are
children or successors of the root parameter.

### Operations between clients and server

The server framework allows the following abstract operations from clients on
each parameter in the object dictionary:

* Get operation (client/server model)
* Set operation (client/server model)
* Subscribe operation (publish/subscribe model)
* Unsubscribe operation (publish/subscribe model)

These operations can be realized by messages sent back and forth in any format.
The framework provides some useful protocols and even allows to write your own.

Parameters may either be modified by the server implementation (readonly
parameters) or by the client side (readwrite parameters). It is not possible
(or at least conceptually not allowed) to alter readwrite parameters by the
server implementation.

### Discovery

Discovery is a term that describes the process of finding out about the
parameters available in a server's object dictionary. This can be carried out
by tranversing the parameter tree from node to node or by special discovery
means.
