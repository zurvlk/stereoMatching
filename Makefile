#Makefile

all:
	cd ab-swp-str; make; cd ../
	cd alpha-expan-str; make; cd ../
	cd GRSA_stereo; make; cd ../
clean:
	cd ab-swp-str; make clean; cd ../
	cd alpha-expan-str; make clean; cd ../
	cd GRSA_stereo; make clean; cd ../
