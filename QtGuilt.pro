TEMPLATE = subdirs

#Fail if we are in the sources...
PRO_FILE=$$_FILE_
PRO_FILE=$$basename(PRO_FILE)
PRO_FILE=$$OUT_PWD/$$PRO_FILE

exists($$PRO_FILE):error("Cannot build inside the sources")

SUBDIRS += \
    QtGuiltModel \
    QtGuiltView \
    QtGuilt 
