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
given, fixed value type. *Events* are objects without value but can be used to
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

Parameter's values can be either readwrite or readonly. I.e., they may either
be modified by the server implementation (readonly parameters) or by a client
(readwrite parameters). It is not possible (or at least conceptually not
allowed) to alter readwrite parameters by the server implementation.

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

### Interaction models

The server framework supports two distinct models for interaction between
clients and server:

* Client/server model
* Publish/subscribe model

#### Client/server model

In the 'classic' client/server model, the server framework allows the following
abstract operations from clients on each parameter in the object dictionary:

* Get operation (parameters and nodes only)
* Set operation (parameters only)
* Execute operation (events only)

#### Publish/subscribe model

In the publish/subscribe model, the clients take the role of a subscriber
whereas the server have the role of the publisher. In this model the server
framework allows the following abstract operations on each parameter in the
object dictionary:

* Subscribe operation
* Unsubscribe operation
* Notify operation

Subscribers can initiate subscribe und unsubscribe operations, whereas the
notify operation is carried out on value changes by the publisher.

All those mentioned operations can be realized by messages sent back and forth
in any format. The framework provides some useful protocols and even allows to
write your own.

### Discovery

Discovery is a term that describes the process of finding out about the
parameters available in a server's object dictionary. This can be carried out
by traversing the parameter tree from node to node or by special discovery
means.

Implementation
--------------

### Protocols

#### General

There are several protocol implementations provided but the framework is
intentionally open for new implementations.

#### Text protocol

The text protocol is around for historical reasons and for its simplicity and
usability when it comes to manual (keyboard) interaction with a server.

The text protocol implements both the client/server and publish/subscribe
interaction models as well as supports some simple discovery means.

##### Client/server model

The client/server interaction model is supported typically via TCP port 1998.
Operations are encoded in ASCII text in the following manner:

`[(] <operation> [']<uri> [<value>] [)]`

The paratheses and the `'` are for historical reasons and optional.

`<operation>` can be one of:

* `param-ref` or `get` for a get operation
* `param-set!` or `set` for a set operation

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

##### URI representation

The `uri` represents an object within the object dictionary with its fully
qualified name, seperated with a colon (`:`), e.g., `root:node:my_parameter`.
The `root:` prefix is optional.

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

##### Discovery

The text protocol supports a simple dicovery means by using the operation
`param-disp` or `browse`. The output is a textual representation of the
objects in the object dictionary.
