SOURCE_FILES =  tcljava0.4a/TclTest.java \
	tcljava0.4a/configure \
	tcljava0.4a/configure.in \
	tcljava0.4a/makefile.in \
	tcljava0.4a/makefile.vc \
	tcljava0.4a/tkJava.c

PATCHED_FILES =  tcljava0.4/TclTest.java \
	tcljava0.4/configure \
	tcljava0.4/configure.in \
	tcljava0.4/makefile.in \
	tcljava0.4/makefile.vc \
	tcljava0.4/tkJava.c \
	tcljava0.4/tcljava0.4a.html

all: tcljava0.4a.patch tcljava0.4a.tar.gz itcl2.2a.tar.gz
clean:
	rm -f tcljava0.4a.patch tcljava0.4a.tar.gz itcl2.2a.tar.gz
	rm -rf tcljava0.4 tmp

tcljava0.4a.patch: $(SOURCE_FILES)
	rm -rf tcljava0.4
	gtar -zxf tcljava0.4.tar.gz
	touch $(PATCHED_FILES)
	-gdiff -c tcljava0.4 tcljava0.4a > $@

tcljava0.4a.tar.gz:
	rm -rf tmp
	mkdir tmp
	(cd tmp; gtar -zxf ../tcljava0.4.tar.gz)
	(cd tmp/tcljava0.4; /usr/sww/bin/patch -p1 < ../../tcljava0.4a.patch)
	(mv tmp/tcljava0.4 tmp/tcljava0.4a)
	chmod a+x tmp/tcljava0.4a/configure
	rm -f tmp/tcljava0.4a/configure.in.orig \
		tmp/tcljava0.4a/makefile.in.orig \
		tmp/tcljava0.4a/makefile.vc.orig \
		tmp/tcljava0.4a/tcljava0.4a.html.orig
	(cd tmp;gtar -zcf ../tcljava0.4a.tar.gz tcljava0.4a)
	cp tcljava0.4a.tar.gz /users/cxh/public_html/ptpub

ITCL_SOURCES= \
	itcl2.2/makefile.vc \
	itcl2.2/itcl/win/makefile.vc \
	itcl2.2/itk/win/makefile.vc \
	itcl2.2/iwidgets2.2.0/win/makefile.vc \
	itcl2.2/tcl7.6/win/makefile.vc \
	itcl2.2/tk4.2/win/makefile.vc

itcl2.2a.tar.gz:
	/usr/sww/bin/gtar -zcf $@ $(ITCL_SOURCES)
	cp $@ /users/cxh/public_html/ptpub

test:
	(cd tmp/tcljava0.4a; configure; make)
testhppa:
	cd tmp/tcljava0.4a; configure --with-java=/usr/sww/lang/java --with-tclinclude=/usr/sww/tcl/include --with-tcllib=/usr/sww/tcl/lib; \
	make OTHERCFLAGS="-DTCLEXECFIX -Dexecenv=javaexecenv -D_INT32_T -D_UINT32_T -D_INT16_T -D_UINT16_T -D_INT8_T -D_UINT8_T, -Dint32_t=long")

