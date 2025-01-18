all: Tracheophyta
all: Aves
all: Amphibia
all: Mammalia
all: Reptilia
all: Insecta

.PHONY: update
# see https://www.gbif.org/dataset/de8934f4-a136-481c-a87a-b0b202b80a31
update: unpack
	curl -fL \
	"https://api.artdatabanken.se/taxonservice/v1/DarwinCore/Download?subscription-key=4b068709e7f2427d9fc76bf42d8e2b57" \
	| ./unpack dwc

SHELL=/bin/bash
CXXFLAGS=-Wall -Wextra -pedantic -std=c++17 -g -Os
CPPFLAGS=
ARFLAGS=rTP

unpack: unpack.o libtaxa.a
	$(CXX) $(CXXFLAGS) -L. -o $@ $< -ltaxa -larchive

libtaxa.a: split.o
libtaxa.a: cleanup.o
	$(AR) $(ARFLAGS) $@ $^

# tests

.PHONY: check checkv
check: test/test
	./test/test
checkv: test/test
	valgrind -q ./test/test -v

test/test: test/test.o libtest.a libtaxa.a
	$(CXX) $(CXXFLAGS) -o $@ test/test.o -L. -ltest -ltaxa

test/test.cc: libtest.a
	orchis -o $@ $^

libtest.a: test/split.o
	$(AR) $(ARFLAGS) $@ $^

test/%.o: CPPFLAGS+=-I.

# other

.PHONY: clean
clean:
	$(RM) unpack
	$(RM) test/test test/test.cc
	$(RM) *.o lib*.a
	$(RM) test/*.o
	$(RM) -r dep
	$(RM) -r TAGS

.PHONY: tags
tags: TAGS
TAGS:
	ctags --exclude='test' -eR . --extra=q

love:
	@echo "not war?"

# DO NOT DELETE

$(shell mkdir -p dep/test)
DEPFLAGS=-MT $@ -MMD -MP -MF dep/$*.Td
COMPILE.cc=$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

%.o: %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	@mv dep/$*.{Td,d}

dep/%.d: ;
dep/test/%.d: ;
-include dep/*.d
-include dep/test/*.d
