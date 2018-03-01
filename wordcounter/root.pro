TEMPLATE = subdirs

SUBDIRS += \
    wordcounter \
    textproccore \
    magictextproc

wordcounter.depends = magictextproc
magictextproc.depends = textproccore
