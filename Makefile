SUBDIRS = src

all : $(SUBDIRS)

$(SUBDIRS) :
	$(MAKE) -C $@ $(MAKECMDGOALS)

install :
	sudo mv ./src/cedit /usr/local/bin/
	cp -r ./ $(HOME)/.cedit

.PHONY : all $(SUBDIRS)
