find_package (Compiz REQUIRED)

include (CompizPlugin)

compiz_plugin (accessibility PKGDEPS atspi-2)
