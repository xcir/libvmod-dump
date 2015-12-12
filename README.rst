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
                  if(resp.status >= 500 && req.esi_level==0){
                    dump.req("dump error request.");
                  }
                }
                
                //Output sample.( curl -F 'file=@README.rst' http://localhost:6081/ )
                //# varnishlog -qdebug:VMD-DUMP -graw
                //         8 Debug          c "VMD-DUMP: POST%0d%0a"
                //         8 Debug          c "VMD-DUMP: /%0d%0a"
                //         8 Debug          c "VMD-DUMP: HTTP/1.1%0d%0a"
                //         8 Debug          c "VMD-DUMP: User-Agent: curl/7.35.0%0d%0a"
                //         8 Debug          c "VMD-DUMP: Host: localhost:6081%0d%0a"
                //         8 Debug          c "VMD-DUMP: Accept: */*%0d%0a"
                //         8 Debug          c "VMD-DUMP: Content-Length: 4897%0d%0a"
                //         8 Debug          c "VMD-DUMP: Content-Type: multipart/form-data; boundary=------------------------73e9dc077f7314be%0d%0a"
                //         8 Debug          c "VMD-DUMP: %0d%0a"
                //         8 Debug          c "VMD-DUMP: --------------------------73e9dc077f7314be%0d%0aContent-Disposition: form-data; name="file"; filename="README.rst"%0d%0aContent-Type: application/octet-stream%0d%0a%0d%0a============%0avmod_dump%0a============%0a%0a------------------------------------%0aDump "
                //         8 Debug          c "VMD-DUMP: the request header with body%0a------------------------------------%0a%0a:Author: Shohei Tanaka(@xcir)%0a:Date: 2015-12-11%0a:Version: develop-varnish41%0a:Support Varnish Version: 4.1.x%0a:Manual section: 3%0a%0aSYNOPSIS%0a========%0a%0aimport dump;%0a%0aDESCR"
                //         8 Debug          c "VMD-DUMP: IPTION%0a===========%0a%0aExample Varnish vmod demonstrating how to write an out-of-tree Varnish vmod.%0a%0aImplements the traditional Hello World as a vmod.%0a%0aFUNCTIONS%0a=========%0a%0areq%0a-----%0a%0aPrototype%0a        ::%0a%0a                req(STRING val"
                //         8 Debug          c "VMD-DUMP: )%0aReturn value%0a%09VOID%0aDescription%0a%09Write a request header-body to the VSL.%0aExample%0a        ::%0a%0a                import std;//use libvmod-std%0a                import dump;%0a                %0a                sub vcl_recv {%0a                 "
                //         8 Debug          c "VMD-DUMP:  std.cache_req_body(1MB);%0a                }%0a                sub vcl_deliver {%0a                  if(resp.status == 503) {%0a                    dump.req("dump error request.");%0a                  }%0a                }%0a                %0a    "
                //         8 Debug          c "VMD-DUMP:             //Output sample.( curl -F 'file=@README.rst' http://localhost:6081/ )%0a                //# varnishlog -graw -qdebug:VMOD-DUMP%0a                //     32788 Debug          c "VMOD-DUMP: dump error request."%0a                /"
                //         8 Debug          c "VMD-DUMP: /     32788 Debug          c "VMOD-DUMP: POST%0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: /%0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: HTTP/1.1%0d%0a"%0a                //     32788 Debug     "
                //         8 Debug          c "VMD-DUMP:      c "VMOD-DUMP: User-Agent: curl/7.35.0%0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: Host: localhost:6081%0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: Accept: */*%0d%0a"%0a                //  "
                //         8 Debug          c "VMD-DUMP:    32788 Debug          c "VMOD-DUMP: Content-Length: 3200%0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: Content-Type: multipart/form-data; boundary=------------------------03d94756ba140cd1%0d%0a"%0a                /"
                //         8 Debug          c "VMD-DUMP: /     32788 Debug          c "VMOD-DUMP: %0d%0a"%0a                //     32788 Debug          c "VMOD-DUMP: --------------------------03d94756ba140cd1%0d%0aContent-Disposition: form-data; name="file"; filename="README.rst"%0d%0aConten"
                //         8 Debug          c "VMD-DUMP: t-Type: application/octet-stream%0d%0a%0d%0a============%0avmod_dump%0a============%0a%0a------------------------------------%0aDump the request h"%0a                //     32788 Debug          c "VMOD-DUMP: eader with body%0a---------"
                //         8 Debug          c "VMD-DUMP: ---------------------------%0a%0a:Author: Shohei Tanaka(@xcir)%0a:Date: 2015-12-11%0a:Version: develop-varnish41%0a:Support Varnish Version: 4.1.x%0a:Manual section: 3%0a%0aSYNOPSIS%0a========%0a%0aimport dump;%0a%0aDESCRIPTION%0a==="
                //         8 Debug          c "VMD-DUMP: ========%0a%0aExampl"%0a                //...%0a%0a                %0a%0aINSTALLATION%0a============%0a%0aThe source tree is based on autotools to configure the building, and%0adoes also have the necessary bits in place to do functional unit tests%0ausi"
                //         8 Debug          c "VMD-DUMP: ng the ``varnishtest`` tool.%0a%0aBuilding requires the Varnish header files and uses pkg-config to find%0athe necessary paths.%0a%0aUsage::%0a%0a ./autogen.sh%0a ./configure%0a%0aIf you have installed Varnish to a non-standard directory, call%0a``autogen"
                //         8 Debug          c "VMD-DUMP: .sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to%0athe appropriate path. For dump, when varnishd configure was called%0awith ``--prefix=$PREFIX``, use%0a%0a PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig%0a export PKG_CONFIG_PATH%0a%0aMake ta"
                //         8 Debug          c "VMD-DUMP: rgets:%0a%0a* make - builds the vmod.%0a* make install - installs your vmod.%0a* make check - runs the unit tests in ``src/tests/*.vtc``%0a* make distcheck - run check and prepare a tarball of the vmod.%0a%0aInstallation directories%0a--------------"
                //         8 Debug          c "VMD-DUMP: ----------%0a%0aBy default, the vmod ``configure`` script installs the built vmod in%0athe same directory as Varnish, determined via ``pkg-config(1)``. The%0avmod installation directory can be overridden by passing the%0a``VMOD_DIR`` variable "
                //         8 Debug          c "VMD-DUMP: to ``configure``.%0a%0aOther files like man-pages and documentation are installed in the%0alocations determined by ``configure``, which inherits its default%0a``--prefix`` setting from Varnish.%0a%0aUSAGE EXAMPLE%0a=============%0a%0aIn your VCL you c"
                //         8 Debug          c "VMD-DUMP: ould then use this vmod along the following lines::%0a%0a        import dump;%0a%0a        sub vcl_deliver {%0a                # This sets resp.http.hello to "Hello, World"%0a                set resp.http.hello = dump.hello("World");%0a        }%0a%0a"
                //         8 Debug          c "VMD-DUMP: COMMON PROBLEMS%0a===============%0a%0a* configure: error: Need varnish.m4 -- see README.rst%0a%0a  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling%0a  ``autogen.sh`` and ``configure``%0a%0a* Incompatibilities with different Varni"
                //         8 Debug          c "VMD-DUMP: sh Cache versions%0a%0a  Make sure you build this vmod against its correspondent Varnish Cache version.%0a  For dump, to build against Varnish Cache 4.0, this vmod must be built from branch 4.0.%0a%0d%0a--------------------------73e9dc077f7314be"
                //         8 Debug          c "VMD-DUMP: --%0d%0a"

                resp(STRING val)
Return value
	VOID
Description
	Write a response header-body to the VSL.
Example
        ::

                import dump;
                
                sub vcl_deliver {
                  if(resp.status >= 500 && req.esi_level==0){
                    dump.resp("dump error response.");
                  }
                }
                
                //Output sample.( curl -F 'file=@README.rst' http://localhost:6081/ )
                //# varnishlog -qdebug:VMD-DUMP -graw
                //         2 Debug          c "VMD-DUMP: HTTP/1.1%0d%0a"
                //         2 Debug          c "VMD-DUMP: 503%0d%0a"
                //         2 Debug          c "VMD-DUMP: Backend fetch failed%0d%0a"
                //         2 Debug          c "VMD-DUMP: Date: Sat, 12 Dec 2015 18:32:28 GMT%0d%0a"
                //         2 Debug          c "VMD-DUMP: Server: Varnish%0d%0a"
                //         2 Debug          c "VMD-DUMP: Content-Type: text/html; charset=utf-8%0d%0a"
                //         2 Debug          c "VMD-DUMP: Retry-After: 5%0d%0a"
                //         2 Debug          c "VMD-DUMP: X-Varnish: 2%0d%0a"
                //         2 Debug          c "VMD-DUMP: Age: 0%0d%0a"
                //         2 Debug          c "VMD-DUMP: Via: 1.1 varnish-v4%0d%0a"
                //         2 Debug          c "VMD-DUMP: %0d%0a"
                //         2 Debug          c "VMD-DUMP: <!DOCTYPE html>%0a<html>%0a  <head>%0a    <title>503 Backend fetch failed</title>%0a  </head>%0a  <body>%0a    <h1>Error 503 Backend fetch failed</h1>%0a    <p>Backend fetch failed</p>%0a    <h3>Guru Meditation:</h3>%0a    <p>XID: 3</p>%0a    <hr>%0a    <"
                //         2 Debug          c "VMD-DUMP: p>Varnish cache server</p>%0a  </body>%0a</html>%0a"


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
