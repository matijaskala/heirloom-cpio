all: cpio pax

cpio: cpio.o flags.o pax.o

pax: cpio
	ln -sf $< $@

clean:
	$(RM) cpio pax cpio.o flags.o pax.o
