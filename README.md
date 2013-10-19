mlbrowser
=========

This is the respository for the mlbrowser that has been developed by [Metrological](http://www.metrological.com). It can be used with the buildroot fork at https://github.com/albertd/buildroot-rpi or its fork https://github.com/msieben/buildroot-rpi.git.

You can add this package to your own fork of buildroot by adding the Config.in and mlbrowser.mk files.

The mlbrowser is a Qt port WebKit 1 based browser with support for the remote WebInspector.

*Config.in*

	config BR2_PACKAGE_MLBROWSER
	bool "mlbrowser"
	help
	  Metrological's simple (accelerated) browser implementation
	  http://www.metrological.com

*mlbrowser.mk*

	#############################################################
	#
	# mlbrowser
	#
	#############################################################

	MLBROWSER_VERSION = 5641590312af70482eb95dec72016e670f195d74
	MLBROWSER_SITE_METHOD = git
	MLBROWSER_SITE = https://github.com/msieben/mlbrowser.git

	# mlbrowser requires qt + qtwebkit or qt5webkit but usually you do not build both versions so we have to differentiate between the dependencies.
	MLBROWSER_NOBUILD = n 

	ifeq ($(BR2_PACKAGE_QT5WEBKIT),y)
	MLBROWSER_DEPENDENCIES = qt5webkit
	MLBROWSER_NOBUILD = y 
	endif

	ifeq ($(BR2_PACKAGE_QT_WEBKIT),y)
	MLBROWSER_DEPENDENCIES = qt gstreamer
	MLBROWSER_NOBUILD = y 
	endif

	define MLBROWSER_CONFIGURE_CMDS
	        if [ $(MLBROWSER_NOBUILD) = "n" ]; then \
			echo ""; \
			echo "To build mlbrowser enable the Qt WebKit port in your configuration.";\
			echo ""; \
			exit -1; \
		else \
			(cd $(@D); \
				$(TARGET_MAKE_ENV) \
				$(HOST_DIR)/usr/bin/qmake \
					DEFINES+=_BROWSER_ \
					./src/mlbrowser.pro \
			); \
		fi
	endef

	define MLBROWSER_BUILD_CMDS
		$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
	endef

	define MLBROWSER_INSTALL_TARGET_CMDS
		$(INSTALL) -D -m 0755 $(@D)/mlbrowser $(TARGET_DIR)/usr/bin
	endef

	define MLBROWSER_UNINSTALL_TARGET_CMDS
		rm -f $(TARGET_DIR)/usr/bin/mlbrowser
	endef

	$(eval $(generic-package))

Have fun!
