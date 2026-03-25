.PHONY = all clean run
CURDIR_TMP := $(CURDIR)

all:
	@echo Make WiiFlow Main
	@$(MAKE) --no-print-directory -C $(CURDIR_TMP) -f $(CURDIR_TMP)/Makefile.main

clean:
	@echo Cleanup WiiFlow Main
	@$(MAKE) --no-print-directory -C $(CURDIR_TMP) -f $(CURDIR_TMP)/Makefile.main clean
run:
	@$(MAKE) --no-print-directory -C $(CURDIR_TMP) -f $(CURDIR_TMP)/Makefile.main run
