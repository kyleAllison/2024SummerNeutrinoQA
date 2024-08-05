#############################################################
#
#  set up XML configuration

XMLS := $(patsubst %.xml.in,%.xml,$(wildcard *.xml.in))

.PHONY: all clean

SHINEOFFLINECONFIG := shine-offline-config
CONFIGFILES := $(shell $(SHINEOFFLINECONFIG) --config)
DBFILES := $(shell $(SHINEOFFLINECONFIG) --db-path)
DOCDIR := $(shell $(SHINEOFFLINECONFIG) --doc)
TESTFILE := $(DOCDIR)/SampleEvents/run-041826x01045_vbuf1_PbPb150_2exampleevent.pteraw
OUTFILE := $(notdir $(patsubst %.pteraw,%.root,$(TESTFILE)))
LOGFILE := $(notdir $(patsubst %.pteraw,%.log,$(TESTFILE)))

all: $(XMLS)

%: %.in
	@echo -n "Generating $@ file..."
	@ sed -e 's!@''CONFIGDIR@!$(CONFIGFILES)!g;s!@''SHINEDBDIR@!$(DBFILES)!g' $< >$@
	@echo "done"

clean:
	- rm -f run $(XMLS) *.log *.root

#############################################################
# 'make run' will run the thing

run: $(XMLS)
	chmod +x runQA.sh
	./runQA.sh -i $(TESTFILE) -o $(OUTFILE) -b bootstrap_static.xml >& $(LOGFILE)
	touch $@
