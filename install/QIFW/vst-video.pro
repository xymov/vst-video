TEMPLATE = aux

INSTALLER = installer

INPUT = $$PWD/config/config.xml $$PWD/packages
example.input = INPUT
example.output = $$INSTALLER
example.commands = binarycreator -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
example.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += example

OTHER_FILES =

DISTFILES += \
    ../../../Qt/QtIFW-3.2.2/doc/ifw.qch \
    packages/net.xymov.ifw/meta/installscript.qs
