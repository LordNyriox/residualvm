# This file contains port specific Makefile rules. It is automatically
# included by the default (main) Makefile.
#

#
# POSIX specific
#
install:
	$(INSTALL) -d "$(DESTDIR)$(bindir)"
	$(INSTALL) -c -m 755 "./$(EXECUTABLE)" "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	$(INSTALL) -d "$(DESTDIR)$(mandir)/man6/"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.6" "$(DESTDIR)$(mandir)/man6/residualvm.6"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/pixmaps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/residualvm.xpm" "$(DESTDIR)$(datarootdir)/pixmaps/residualvm.xpm"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/residualvm.svg" "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/residualvm.svg"
	$(INSTALL) -d "$(DESTDIR)$(docdir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_DOCS) "$(DESTDIR)$(docdir)"
	$(INSTALL) -d "$(DESTDIR)$(datadir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_THEMES) $(DIST_FILES_NETWORKING) $(DIST_FILES_VKEYBD) $(DIST_FILES_ENGINEDATA) "$(DESTDIR)$(datadir)/"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/applications"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.desktop" "$(DESTDIR)$(datarootdir)/applications/residualvm.desktop"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/appdata"
#	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.appdata.xml" "$(DESTDIR)$(datarootdir)/appdata/residualvm.appdata.xml"
	# ResidualVM specific
ifdef USE_OPENGL_SHADERS
	$(INSTALL) -d "$(DESTDIR)$(datadir)/shaders"
	$(INSTALL) -c -m 644 $(DIST_FILES_SHADERS) "$(DESTDIR)$(datadir)/shaders"
endif
ifdef DYNAMIC_MODULES
	$(INSTALL) -d "$(DESTDIR)$(libdir)/residualvm/"
	$(INSTALL) -c -m 644 $(PLUGINS) "$(DESTDIR)$(libdir)/residualvm/"
endif

install-strip:
	$(INSTALL) -d "$(DESTDIR)$(bindir)"
	$(INSTALL) -c -s -m 755 "./$(EXECUTABLE)" "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	$(INSTALL) -d "$(DESTDIR)$(mandir)/man6/"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.6" "$(DESTDIR)$(mandir)/man6/residualvm.6"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/pixmaps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/residualvm.xpm" "$(DESTDIR)$(datarootdir)/pixmaps/residualvm.xpm"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/"
	$(INSTALL) -c -m 644 "$(srcdir)/icons/residualvm.svg" "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/residualvm.svg"
	$(INSTALL) -d "$(DESTDIR)$(docdir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_DOCS) "$(DESTDIR)$(docdir)"
	$(INSTALL) -d "$(DESTDIR)$(datadir)"
	$(INSTALL) -c -m 644 $(DIST_FILES_THEMES) $(DIST_FILES_NETWORKING) $(DIST_FILES_VKEYBD) $(DIST_FILES_ENGINEDATA) "$(DESTDIR)$(datadir)/"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/applications"
	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.desktop" "$(DESTDIR)$(datarootdir)/applications/residualvm.desktop"
	$(INSTALL) -d "$(DESTDIR)$(datarootdir)/appdata"
#	$(INSTALL) -c -m 644 "$(srcdir)/dists/residualvm.appdata.xml" "$(DESTDIR)$(datarootdir)/appdata/residualvm.appdata.xml"
	# ResidualVM specific
ifdef USE_OPENGL_SHADERS
	$(INSTALL) -d "$(DESTDIR)$(datadir)/shaders"
	$(INSTALL) -c -m 644 $(DIST_FILES_SHADERS) "$(DESTDIR)$(datadir)/shaders"
endif
ifdef DYNAMIC_MODULES
	$(INSTALL) -d "$(DESTDIR)$(libdir)/residualvm/"
	$(INSTALL) -c -s -m 644 $(PLUGINS) "$(DESTDIR)$(libdir)/residualvm/"
endif

uninstall:
	rm -f "$(DESTDIR)$(bindir)/$(EXECUTABLE)"
	rm -f "$(DESTDIR)$(mandir)/man6/residualvm.6"
	rm -f "$(DESTDIR)$(datarootdir)/pixmaps/residualvm.xpm"
	rm -f "$(DESTDIR)$(datarootdir)/icons/hicolor/scalable/apps/residualvm.svg"
	rm -rf "$(DESTDIR)$(docdir)"
	rm -rf "$(DESTDIR)$(datadir)"
	rm -f "$(DESTDIR)$(datarootdir)/applications/scummvm.desktop"
	rm -f "$(DESTDIR)$(datarootdir)/appdata/scummvm.appdata.xml"
ifdef DYNAMIC_MODULES
	rm -rf "$(DESTDIR)$(libdir)/residualvm/"
endif

#ResidualVM specific:
deb:
	ln -sf dists/debian;
	debian/prepare
	fakeroot debian/rules binary

# Special target to create a application wrapper for Mac OS X

ifdef USE_DOCKTILEPLUGIN

ScummVMDockTilePlugin.o:
	$(CXX) -O2 -c $(srcdir)/backends/taskbar/macosx/dockplugin/dockplugin.m -o ScummVMDockTilePlugin.o

ScummVMDockTilePlugin: ScummVMDockTilePlugin.o
	$(CXX) -bundle -framework Foundation -framework AppKit -fobjc-link-runtime ScummVMDockTilePlugin.o -o ScummVMDockTilePlugin

residualvm.docktileplugin: ScummVMDockTilePlugin
	mkdir -p residualvm.docktileplugin/Contents
	cp $(srcdir)/dists/macosx/dockplugin/Info.plist residualvm.docktileplugin/Contents
	mkdir -p residualvm.docktileplugin/Contents/MacOS
	cp ScummVMDockTilePlugin residualvm.docktileplugin/Contents/MacOS/ResidualVMDockTilePlugin
	chmod 644 residualvm.docktileplugin/Contents/MacOS/ResidualVMDockTilePlugin

clean-docktileplugin:
	rm -rf ScummVMDockTilePlugin.o ScummVMDockTilePlugin residualvm.docktileplugin/

clean: clean-docktileplugin
endif

bundle_name = ResidualVM.app
bundle: all
ifdef USE_DOCKTILEPLUGIN
bundle: residualvm.docktileplugin
endif
bundle:
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	sed -e 's/$$(PRODUCT_BUNDLE_IDENTIFIER)/org.residualvm.residualvm/' $(srcdir)/dists/macosx/Info.plist >$(bundle_name)/Contents/Info.plist
ifdef USE_SPARKLE
	mkdir -p $(bundle_name)/Contents/Frameworks
	cp $(srcdir)/dists/macosx/dsa_pub.pem $(bundle_name)/Contents/Resources/
	rm -rf $(bundle_name)/Contents/Frameworks/Sparkle.framework
	cp -R $(SPARKLEPATH)/Sparkle.framework $(bundle_name)/Contents/Frameworks/
endif
	cp $(srcdir)/icons/residualvm.icns $(bundle_name)/Contents/Resources/
	cp $(DIST_FILES_DOCS) $(bundle_name)/Contents/Resources/
	cp $(DIST_FILES_THEMES) $(bundle_name)/Contents/Resources/
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(bundle_name)/Contents/Resources/
endif
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(bundle_name)/Contents/Resources/
endif
ifdef USE_OPENGL_SHADERS
	mkdir -p $(bundle_name)/Contents/Resources/shaders
	cp $(DIST_FILES_SHADERS) $(bundle_name)/Contents/Resources/shaders/
endif
	$(srcdir)/devtools/credits.pl --rtf > $(bundle_name)/Contents/Resources/AUTHORS.rtf
	rm $(bundle_name)/Contents/Resources/AUTHORS
	mv $(bundle_name)/Contents/Resources/README.md $(bundle_name)/Contents/Resources/README # ResidualVM
	cp $(bundle_name)/Contents/Resources/COPYING.LGPL $(bundle_name)/Contents/Resources/COPYING-LGPL
	cp $(bundle_name)/Contents/Resources/COPYING.FREEFONT $(bundle_name)/Contents/Resources/COPYING-FREEFONT
	cp $(bundle_name)/Contents/Resources/COPYING.OFL $(bundle_name)/Contents/Resources/COPYING-OFL
	cp $(bundle_name)/Contents/Resources/COPYING.BSD $(bundle_name)/Contents/Resources/COPYING-BSD
	chmod 644 $(bundle_name)/Contents/Resources/*
ifdef USE_OPENGL_SHADERS
	chmod 755 $(bundle_name)/Contents/Resources/shaders
endif
	cp residualvm $(bundle_name)/Contents/MacOS/residualvm
	chmod 755 $(bundle_name)/Contents/MacOS/residualvm
	$(STRIP) $(bundle_name)/Contents/MacOS/residualvm
ifdef USE_DOCKTILEPLUGIN
	mkdir -p $(bundle_name)/Contents/PlugIns
	cp -r residualvm.docktileplugin $(bundle_name)/Contents/PlugIns/
endif

iphonebundle: iphone
	mkdir -p $(bundle_name)
	cp $(srcdir)/dists/iphone/Info.plist $(bundle_name)/
	cp $(DIST_FILES_DOCS) $(bundle_name)/
	cp $(DIST_FILES_THEMES) $(bundle_name)/
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(bundle_name)/
endif
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(bundle_name)/
endif
ifdef DIST_FILES_VKEYBD
	cp $(DIST_FILES_VKEYBD) $(bundle_name)/
endif
	$(STRIP) residualvm
	ldid -S residualvm
	chmod 755 residualvm
	cp residualvm $(bundle_name)/ResidualVM
	cp $(srcdir)/dists/iphone/icon.png $(bundle_name)/
	cp $(srcdir)/dists/iphone/icon-72.png $(bundle_name)/
	cp $(srcdir)/dists/iphone/Default.png $(bundle_name)/

ifdef USE_SPARKLE
ifdef MACOSX
ifneq ($(SPARKLEPATH),)
LDFLAGS += -F$(SPARKLEPATH)
endif
LDFLAGS += -framework Sparkle -Wl,-rpath,@loader_path/../Frameworks
endif
endif

# Special target to create a static linked binary for the iPhone (legacy, and iOS 7+)
iphone: $(OBJS)
	$(CXX) $(LDFLAGS) -o residualvm $(OBJS) \
		-framework UIKit -framework CoreGraphics -framework OpenGLES \
		-framework CoreFoundation -framework QuartzCore -framework Foundation \
		-framework AudioToolbox -framework CoreAudio -lobjc -lz

publish-appcast:
	scp dists/macosx/residualvm_appcast.xml www.residualvm.org:/var/www/appcasts/macosx/release.xml


#
# Special target to generate project files for various IDEs
# Mainly Win32-specific
#

# The release branch is in form 'heads/branch-1-4-1', for this case
# $CUR_BRANCH will be equal to '1', for the rest cases it will be empty
CUR_BRANCH := $(shell cd $(srcdir); git describe --all |cut -d '-' -f 4-)

ideprojects: devtools/create_project
ifeq ($(VER_DIRTY), -dirty)
	$(error You have uncommitted changes)
endif
ifeq "$(CUR_BRANCH)" "heads/master"
	$(error You cannot do it on master)
else ifeq "$(CUR_BRANCH)" ""
	$(error You must be on a release branch)
endif
	@echo Creating Code::Blocks project files...
	@cd $(srcdir)/dists/codeblocks && ../../devtools/create_project/create_project ../.. --codeblocks >/dev/null && git add -f engines/plugins_table.h *.workspace *.cbp
	@echo Creating MSVC project files...
	@cd $(srcdir)/dists/msvc && ../../devtools/create_project/create_project ../.. --msvc-version 12 --msvc >/dev/null && git add -f engines/plugins_table.h *.sln *.vcxproj *.vcxproj.filters *.props
	@echo
	@echo All is done.
	@echo Now run
	@echo "\tgit commit -m 'DISTS: Generated Code::Blocks and MSVC project files'"


# Mark special targets as phony
.PHONY: deb bundle osxsnap install uninstall
