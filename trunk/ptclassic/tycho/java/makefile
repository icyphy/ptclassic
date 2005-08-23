makefiles all sources install docs depend checkjunk sccsinfo sccsclean test extraclean clean: mk/ptII.mk
	cd ptolemy/plot; make $@
# Generate ptII.mk by running configure
mk/ptII.mk: configure mk/ptII.mk.in
	./configure
realclean itcldocs tests idocs weblint dists:
	echo "Nothing need be done for  in ptolemy/plot)"
