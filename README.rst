============
vmod_dump
============

------------------------------------
Dump the request header with body
------------------------------------

:Author: Shohei Tanaka(@xcir)
:Date: 2015-12-11
:Version: develop-varnish41
:Support Varnish Version: 4.1.x
:Manual section: 3

SYNOPSIS
========

import dump;

DESCRIPTION
===========

Example Varnish vmod demonstrating how to write an out-of-tree Varnish vmod.

Implements the traditional Hello World as a vmod.

FUNCTIONS
=========

req
-----

Prototype
        ::

                req(STRING val)
Return value
	VOID
Description
	Write a request header-body to the VSL.
Example
        ::

                import std;//use libvmod-std
                import dump;
                
                sub vcl_recv {
                  std.cache_req_body(1MB);
                }
                sub vcl_deliver {
                  if(resp.status == 503) {
                    dump.req("dump error request.");
                  }
                }
                
                //Output sample.( curl -F 'file=@README.rst' http://localhost:6081/ )
                //# varnishlog -graw -qdebug:VMOD-DUMP
                //     32788 Debug          c "VMOD-DUMP: dump error request."
                //     32788 Debug          c "VMOD-DUMP: POST%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: /%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: HTTP/1.1%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: User-Agent: curl/7.35.0%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: Host: localhost:6081%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: Accept: */*%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: Content-Length: 3200%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: Content-Type: multipart/form-data; boundary=------------------------03d94756ba140cd1%0d%0a"
                //     32788 Debug          c "VMOD-DUMP: %0d%0a"
                //     32788 Debug          c "VMOD-DUMP: --------------------------03d94756ba140cd1%0d%0aContent-Disposition: form-data; name="file"; filename="README.rst"%0d%0aContent-Type: application/octet-stream%0d%0a%0d%0a============%0avmod_dump%0a============%0a%0a------------------------------------%0aDump the request h"
                //     32788 Debug          c "VMOD-DUMP: eader with body%0a------------------------------------%0a%0a:Author: Shohei Tanaka(@xcir)%0a:Date: 2015-12-11%0a:Version: develop-varnish41%0a:Support Varnish Version: 4.1.x%0a:Manual section: 3%0a%0aSYNOPSIS%0a========%0a%0aimport dump;%0a%0aDESCRIPTION%0a===========%0a%0aExampl"
                //...

                

INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the ``varnishtest`` tool.

Building requires the Varnish header files and uses pkg-config to find
the necessary paths.

Usage::

 ./autogen.sh
 ./configure

If you have installed Varnish to a non-standard directory, call
``autogen.sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to
the appropriate path. For dump, when varnishd configure was called
with ``--prefix=$PREFIX``, use

 PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
 export PKG_CONFIG_PATH

Make targets:

* make - builds the vmod.
* make install - installs your vmod.
* make check - runs the unit tests in ``src/tests/*.vtc``
* make distcheck - run check and prepare a tarball of the vmod.

Installation directories
------------------------

By default, the vmod ``configure`` script installs the built vmod in
the same directory as Varnish, determined via ``pkg-config(1)``. The
vmod installation directory can be overridden by passing the
``VMOD_DIR`` variable to ``configure``.

Other files like man-pages and documentation are installed in the
locations determined by ``configure``, which inherits its default
``--prefix`` setting from Varnish.

USAGE EXAMPLE
=============

In your VCL you could then use this vmod along the following lines::

        import dump;

        sub vcl_deliver {
                # This sets resp.http.hello to "Hello, World"
                set resp.http.hello = dump.hello("World");
        }

COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling
  ``autogen.sh`` and ``configure``

* Incompatibilities with different Varnish Cache versions

  Make sure you build this vmod against its correspondent Varnish Cache version.
  For dump, to build against Varnish Cache 4.0, this vmod must be built from branch 4.0.