find_package (Compiz REQUIRED)

include (CompizPlugin)

#add_definitions (
#    -DHAVE_CONFIG_H
#    -DPLUGINDIR=\\\"${compiz_plugindir}\\\"
#    -DMETADATADIR=\\\"${compiz_metadatadir}\\\"
#)

compiz_plugin (accessibility PLUGINDEPS composite PKGDEPS dbus-1)
