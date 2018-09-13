all: Tracheophyta
all: Aves
all: Amphibia
all: Mammalia
all: Reptilia
all: Insecta

%: %.xlsx
	./xls2txt $< >$@
