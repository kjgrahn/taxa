all: Tracheophyta
all: Aves
all: Amphibia
all: Mammalia
all: Reptilia
all: Insecta

.PHONY: update
update: # see https://www.gbif.org/dataset/de8934f4-a136-481c-a87a-b0b202b80a31
	curl -fL -o dyntaxa.zip \
	"https://api.artdatabanken.se/taxonservice/v1/DarwinCore/Download?subscription-key=4b068709e7f2427d9fc76bf42d8e2b57"
	unzip -od dwc dyntaxa.zip
