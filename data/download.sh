#!/bin/sh

download_cthead ()
{

	wget https://graphics.stanford.edu/data/voldata/CThead.tar.gz

	mkdir tmp

	tar xf CThead.tar.gz -Ctmp/

	for x in `seq 1 113`
	do
		cat tmp/CThead.$x >> cthead.bin
	done

	rm -rf tmp/ CThead.tar.gz

}

download_bunny ()
{

	wget https://graphics.stanford.edu/data/voldata/bunny-ctscan.tar.gz

	tar xf bunny-ctscan.tar.gz

	for x in `seq 1 361`
	do
		cat bunny/$x >> bunny.bin
	done

	rm -rf bunny bunny-ctscan.tar.gz

}

download_cthead
download_bunny

