# ARCHIVE NOTICE

This project is being archived, left for historical reference, and
is not recommended for any production use.

Statsite-Proxy (Pre-alpha)
==========================

This is a proxy for statsite which is a stats aggregation server. 
<https://github.com/armon/statsite>

Statsite-Proxy provides cluster support for Statsite. It uses
a consistent-hash to shard metrics to any number of statsite 
instances running behind the proxy. 
 
Statsite-Proxy naturally stems from Statsite and implements the 
same protcols. Both the ascii and binary protocols are fully 
supported.

Features
--------

* Provides cluster support for statsite
* Ascii protocol support
* Binary protocol support


Background
----------

Load-balancing a cluster with several instances of statsite is can
be problematic. The main problem is that samples of any given metric 
will most likely be sent to more than one statsite instance within 
the flush interval. When the statsite instances flush there will be 
a race condition that will cause colliding metrics to overwrite one 
another. Metrics could be aggregated after the flush but this is 
very messy and would most likely result in the need to throw away 
some of the generated metrics provided by statsite 
timers ie (percentiles, standard diviation).  

Architecture
------------

Statsite-Proxy is designed to be both highly performant,
and very flexible. To achieve this, it implements the stats
proxy server in pure C, using libev to be extremely fast. 
This allows it to handle hundreds of connections,
and millions of metrics. 

Install
-------

Download and build from source::

    $ git clone https://github.com/deseretdigital/statsite-proxy.git
    $ cd statsite-proxy
    $ pip install SCons  # Uses the Scons build system, may not be necessary
    $ scons
    $ ./statsite_proxy

Building the test code may generate errors if libcheck is not available.
To build the test code successfully, do the following::

    $ cd deps/check-0.9.8/
    $ ./configure
    $ make
    # make install
    # ldconfig (necessary on some Linux distros)
    $ cd ../../
    $ scons test_runner

At this point, the test code should build successfully.

Usage
-----

Statsite-Proxy is configured using a simple INI file.
Here is an example configuration file::

    [statsite]
    port = 8150
    udp_port = 8150
    servers = servers.conf
    log_level = INFO
    
Additionally, you must provide a list of servers to be proxied.
The servers.conf file looks like this:
1.2.3.4:11211   900
5.6.7.8:11211   300
9.8.7.6:11211   1500

ip:port and weighting, \t separated, \n line endings.

Then run statsite, pointing it to that file::

    statsite_proxy -f /etc/statsite_proxy.conf

Protocols
---------

Supports the statsite protocols
<https://github.com/armon/statsite#readme>

By default, Statsite-Proxy will listen for TCP and UDP connections. 





