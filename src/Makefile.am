AM_CPPFLAGS = @VMOD_INCLUDES@ -Wall -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_dump.la

libvmod_dump_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_dump_la_SOURCES = \
	vmod_dump.c

nodist_libvmod_dump_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_dump.lo: vcc_if.c vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_dump.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_dump.vcc

VMOD_TESTS = $(top_srcdir)/src/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/tests/*.vtc: libvmod_dump.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_topbuild=$(abs_top_builddir) $@

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_dump.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_dump.rst \
	$(builddir)/vmod_dump.man.rst
