obj-m := scull.o
clean:
	rm  scull.ko scull.mod.c scull.mod.o scull.o modules.order Module.symvers a.out
