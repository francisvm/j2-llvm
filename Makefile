SRC=index.head content.md index.foot
SLIDES=content.mdpp basics.mdpp
SOURCES=$(shell find sources)

all: index.html

content.md: $(SLIDES) $(SOURCES)
	markdown-pp -e latexrender -o $@ $<

index.html: $(SRC)
	cat $^ > $@

clean:
	rm -rf content.md index.html
