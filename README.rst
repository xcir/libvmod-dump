============
vmod_dump
============

------------------------------------
Dump the header with body
------------------------------------

:Author: Shohei Tanaka(@xcir)
:Date: 2015-12-16
:Version: develop-varnish41
:Support Varnish Version: 4.1.x
:Manual section: 3

SYNOPSIS
========

import dump;

DESCRIPTION
===========

Header and body output to the VSL.

FUNCTIONS
=========

req
-----

Prototype
        ::

                VOID req(STRING val)
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
                
                //Output sample.( curl -F 'file=@LICENSE' http://localhost:6081/ )
                //# varnishlog -qdebug:VMD-DUMP -graw
                //        21 Debug          c "VMD-DUMP: POST "
                //        21 Debug          c "VMD-DUMP: / "
                //        21 Debug          c "VMD-DUMP: HTTP/1.1%0d%0a"
                //        21 Debug          c "VMD-DUMP: User-Agent: curl/7.35.0%0d%0a"
                //        21 Debug          c "VMD-DUMP: Host: localhost:6081%0d%0a"
                //        21 Debug          c "VMD-DUMP: Accept: */*%0d%0a"
                //        21 Debug          c "VMD-DUMP: Content-Length: 1419%0d%0a"
                //        21 Debug          c "VMD-DUMP: Content-Type: multipart/form-data; boundary=------------------------bde0f94dfa359d32%0d%0a"
                //        21 Debug          c "VMD-DUMP: %0d%0a"
                //        21 Debug          c "VMD-DUMP: --------------------------bde0f94dfa359d32%0d%0aContent-Disposition: form-data; name="file"; filename="LICENSE"%0d%0aContent-Type: application/octet-stream%0d%0a%0d%0aRedistribution and use in source and binary forms, with or without%0amodification, a"
                //        21 Debug          c "VMD-DUMP: re permitted provided that the following conditions%0aare met:%0a1. Redistributions of source code must retain the above copyright%0a   notice, this list of conditions and the following disclaimer.%0a2. Redistributions in binary form must re"
                //        21 Debug          c "VMD-DUMP: produce the above copyright%0a   notice, this list of conditions and the following disclaimer in the%0a   documentation and/or other materials provided with the distribution.%0a%0aTHIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS"
                //        21 Debug          c "VMD-DUMP:  IS'' AND%0aANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE%0aIMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE%0aARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE%0aFOR ANY DI"
                //        21 Debug          c "VMD-DUMP: RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL%0aDAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS%0aOR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)%0aHOWEVER CAUSED AND ON ANY "
                //        21 Debug          c "VMD-DUMP: THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT%0aLIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY%0aOUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF%0aSUCH DAMAGE.%0d%0a-------------------------"
                //        21 Debug          c "VMD-DUMP: -bde0f94dfa359d32--%0d%0a"

resp
-----

Prototype
        ::

                VOID resp(STRING val)
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
                
                //Output sample.( curl -F 'file=@LICENSE' http://localhost:6081/ )
                //# varnishlog -qdebug:VMD-DUMP -graw
                //        21 Debug          c "VMD-DUMP: HTTP/1.1 "
                //        21 Debug          c "VMD-DUMP: 503 "
                //        21 Debug          c "VMD-DUMP: Backend fetch failed%0d%0a"
                //        21 Debug          c "VMD-DUMP: Date: Tue, 15 Dec 2015 07:27:23 GMT%0d%0a"
                //        21 Debug          c "VMD-DUMP: Server: Varnish%0d%0a"
                //        21 Debug          c "VMD-DUMP: Content-Type: text/html; charset=utf-8%0d%0a"
                //        21 Debug          c "VMD-DUMP: Retry-After: 5%0d%0a"
                //        21 Debug          c "VMD-DUMP: X-Varnish: 21%0d%0a"
                //        21 Debug          c "VMD-DUMP: Age: 0%0d%0a"
                //        21 Debug          c "VMD-DUMP: Via: 1.1 varnish-v4%0d%0a"
                //        21 Debug          c "VMD-DUMP: %0d%0a"
                //        21 Debug          c "VMD-DUMP: <!DOCTYPE html>%0a<html>%0a  <head>%0a    <title>503 Backend fetch failed</title>%0a  </head>%0a  <body>%0a    <h1>Error 503 Backend fetch failed</h1>%0a    <p>Backend fetch failed</p>%0a    <h3>Guru Meditation:</h3>%0a    <p>XID: 22</p>%0a    <hr>%0a    "
                //        21 Debug          c "VMD-DUMP: <p>Varnish cache server</p>%0a  </body>%0a</html>%0a"

TOOLS
=========

varnishdump
----------------

Description
	Write the dump data to file.

OPTION
        ::

         -p -o [logdir] --sopath [libvarnishapi.so] -D -n [instance-name] -P [pid-file] -V

Example(dump.req("hogehoge"))
        ::

         xcir@varnish41-1:~/ex/libvmod-dump/tools$ sudo ./varnishdump.py
         vxid:32788 type:req time:20151215-164353 file:/home/xcir/ex/libvmod-dump/tools/log/20151215-164353_32788_req.dump val:hogehoge 1stline:POST / HTTP/1.1
         ^Cxcir@varnish41-1:~/ex/libvmod-dump/tools$ cat /home/xcir/ex/libvmod-dump/tools/log/20151215-164353_32788_req.dump
         POST / HTTP/1.1
         User-Agent: curl/7.35.0
         Host: localhost:6081
         Accept: */*
         Content-Length: 1419
         Content-Type: multipart/form-data; boundary=------------------------c72d24d105d8f2d7
         
         --------------------------c72d24d105d8f2d7
         Content-Disposition: form-data; name="file"; filename="LICENSE"
         Content-Type: application/octet-stream
         
         Redistribution and use in source and binary forms, with or without
         modification, are permitted provided that the following conditions
         are met:
         1. Redistributions of source code must retain the above copyright
            notice, this list of conditions and the following disclaimer.
         2. Redistributions in binary form must reproduce the above copyright
            notice, this list of conditions and the following disclaimer in the
            documentation and/or other materials provided with the distribution.
         
         THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
         ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
         IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
         ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
         FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
         DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
         OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
         HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
         LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
         OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
         SUCH DAMAGE.
         --------------------------c72d24d105d8f2d7--

Example(dump.resp("magemage"))
        ::

         xcir@varnish41-1:~/ex/libvmod-dump/tools$ sudo ./varnishdump.py
         vxid:32788 type:resp time:20151215-164353 file:/home/xcir/ex/libvmod-dump/tools/log/20151215-164353_32788_resp.dump val:magemage 1stline:HTTP/1.1 503 Backend fetch failed
         ^Cxcir@varnish41-1:~/ex/libvmod-dump/tools$ cat /home/xcir/ex/libvmod-dump/tools/log/20151215-164353_32788_resp.dump
         HTTP/1.1 503 Backend fetch failed
         Date: Tue, 15 Dec 2015 07:43:53 GMT
         Server: Varnish
         Content-Type: text/html; charset=utf-8
         Retry-After: 5
         X-Varnish: 32788
         Age: 0
         Via: 1.1 varnish-v4
         
         <!DOCTYPE html>
         <html>
           <head>
             <title>503 Backend fetch failed</title>
           </head>
           <body>
            <h1>Error 503 Backend fetch failed</h1>
             <p>Backend fetch failed</p>
             <h3>Guru Meditation:</h3>
             <p>XID: 32789</p>
             <hr>
             <p>Varnish cache server</p>
           </body>
         </html>

Performs reproduction test using the dump file(req)
        ::

         xcir@varnish41-1:~/ex/libvmod-dump/tools/log$ cat 20151215-164657_32791_req.dump | nc localhost 6081
         HTTP/1.1 503 Backend fetch failed
         Date: Tue, 15 Dec 2015 17:14:33 GMT
         Server: Varnish
         Content-Type: text/html; charset=utf-8
         Retry-After: 5
         X-Varnish: 32776
         Age: 0
         Via: 1.1 varnish-v4
         Content-Length: 282
         Connection: keep-alive
         
         <!DOCTYPE html>
         <html>
           <head>
             <title>503 Backend fetch failed</title>
           </head>
           <body>
             <h1>Error 503 Backend fetch failed</h1>
             <p>Backend fetch failed</p>
             <h3>Guru Meditation:</h3>
             <p>XID: 32777</p>
             <hr>
             <p>Varnish cache server</p>
           </body>
         </html>



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
