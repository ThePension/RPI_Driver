obj-m += drvTest.o
.PHONY: all clean
all:
$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' modules
clean:
$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean
