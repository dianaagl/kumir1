TEMPLATE = subdirs
SUBDIRS = TaskControl/task Kumir PluginStarter Addons TaskEdit/task
unix {
	QMAKE_CXXFLAGS += -Wno-class-memaccess
}
