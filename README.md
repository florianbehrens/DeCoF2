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
and exactly one parent object except the 'root' object.

Only *node* objects can hold child objects. *Parameters* have a value of a 
given, fixed value type. *Events* are objects without value but can be used so 
signal some event.

Parameters can hold values of the following builtin value types:

* boolean
* integer
* real
* string
* binary
* and a sequence type for each of the above listed primitive value types.

A node is a special parameter with value type sequence of strings that holds 
the names of its child parameters.

### Access control

Two *userlevel* are assigned to each object which are used for access control. 
One userlevel - the *readlevel* - is used for modifying operations (i.e., 
setting a parameter value or signalling an event) and the other one - the 
*writelevel* - is used for non-modifying operations.

The following userlevels are supported:

* Readonly
* Normal
* Service
* Maintenance
* Internal

The semantics of the various userlevels are project specific.

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
