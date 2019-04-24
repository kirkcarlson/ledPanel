flash : ledPanel.bin
	particle flash Panel1 ledPanel.bin
ledPanel.bin : src/ledPanel.ino src/*h
	particle compile photon . --saveTo ledPanel.bin

print : src/ledPanel.ino
	vim -c 'hardcopy > output.ps'  -c quit src/ledPanel.ino && ps2pdf output.ps >output.pdf
