# 
# No modifications of this Makefile should be necessary.
#
# This file contains the build instructions for installing OMF files.  It is
# generally called from the makefiles for particular formats of documentation.
#
# Note that you must configure your package with --localstatedir=/var
# so that the scrollkeeper-update command below will update the database
# in the standard scrollkeeper directory.
#
# If it is impossible to configure with --localstatedir=/var, then
# modify the definition of scrollkeeper_localstate_dir so that
# it points to the correct location. Note that you must still use 
# $(localstatedir) in this or when people build RPMs it will update
# the real database on their system instead of the one under RPM_BUILD_ROOT.
#
# Note: This make file is not incorporated into xmldocs.make because, in
#       general, there will be other documents install besides XML documents
#       and the makefiles for these formats should also include this file.
#
# About this file:
#	This file was derived from scrollkeeper_example2, a package
#	illustrating how to install documentation and OMF files for use with
#	ScrollKeeper 0.3.x and 0.4.x.  For more information, see:
#		http://scrollkeeper.sourceforge.net/	
# 	Version: 0.1.3 (last updated: March 20, 2002)
#

omf_dest_dir=$(datadir)/omf/@PACKAGE@-${VERSION}
scrollkeeper_localstate_dir = $(localstatedir)/scrollkeeper

# At some point, it may be wise to change to something like this:
# scrollkeeper_localstate_dir = @SCROLLKEEPER_STATEDIR@

omf: omf_timestamp

omf_timestamp: $(omffile)
if SK
	-for file in $(omffile); do \
	  scrollkeeper-preinstall $(docdir)/$(docname).xml $(srcdir)/$$file $$file.out; \
	done; \
	touch omf_timestamp
endif

install-data-hook-omf:
if SK
	$(mkinstalldirs) $(DESTDIR)$(omf_dest_dir)
	for file in $(omffile); do \
		$(INSTALL_DATA) $$file.out $(DESTDIR)$(omf_dest_dir)/$$file; \
	done
	-scrollkeeper-update -p $(scrollkeeper_localstate_dir) -o $(DESTDIR)$(omf_dest_dir)
endif        

uninstall-local-omf:
if SK
	-for file in $(srcdir)/*.omf; do \
		basefile=`basename $$file`; \
		rm -f $(omf_dest_dir)/$$basefile; \
	done
	-rmdir $(omf_dest_dir)
	-scrollkeeper-update -p $(scrollkeeper_localstate_dir)
endif        

clean-local-omf:
if SK
	-for file in $(omffile); do \
		rm -f $$file.out; \
	done
endif        
