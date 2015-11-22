UDP Load Balancer
=================
[![Build Status](https://travis-ci.org/felixcarmona/udp-load-balancer.svg?branch=master)](https://travis-ci.org/felixcarmona/udp-load-balancer)

*UDP Load Balancer* is an open source (under the *MIT* license) load balancer which can load balance any UDP service.

Installation
------------
Compile it with ``make build`` or ``make install`` if you want to place the generated binary in ``/usr/local/bin/``

Usage
-----

    udp_load_balancer [-h] [--port PORT] [--servers SERVERS]
    
    The options are as follows:
          -h, --help            show this help message and exit
          -p, --port PORT
                                Port to listen UDP messages (min 1, max 65535)
          --host HOST or IP
                                Bind the following Host or IP address to listen UDP messages (default: localhost)
          -s, --servers SERVERS
                                Servers list to balance the UDP messages
                                Example: "127.0.0.1:8123, 127.0.0.1:8124"
                                Example: "127.0.0.1:8123, localhost:8124, example.com:8123"
