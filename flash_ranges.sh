cat <<EOF >/dev/null
#	Area		Address		Length	
[00]	Boot		0xB0000000	256K	0x40000
[01]	Boot Params	0xB0040000	64K	0x10000
[02]	Certificate	0xB0050000	64K	0x10000
[03]	Special Area	0xB0060000	64K	0x10000
[04]	Reserve 0	0xB0070000	64K	0x10000
[05]	Code Image 0	0xB0080000	7680K	0x780000
[06]	Primary Setting	0xB4000000	64K	0x10000
[07]	Configuration	0xB4010000	384K	0x60000
[08]	Reserve 1	0xB4070000	64K	0x10000
[09]	Code Image 1	0xB4080000	7680K	0x780000
[10]	1st Flash	0xB0000000	8192K	0x800000
[11]	2nd Flash	0xB4000000	8192K	0x800000
EOF

dd if=10_1st_Flash.img of=00_Boot.img			bs=1 skip=0		count=262144
dd if=10_1st_Flash.img of=01_Boot_Params.img		bs=1 skip=262144	count=65536
dd if=10_1st_Flash.img of=02_Certificate.img		bs=1 skip=327680	count=65536
dd if=10_1st_Flash.img of=03_Special_Area.img		bs=1 skip=393216	count=65536
dd if=10_1st_Flash.img of=04_Reserve_0.img		bs=1 skip=458752	count=65536
dd if=10_1st_Flash.img of=05_Code_Image_0.img		bs=1 skip=524288	count=7864320
dd if=11_2nd_Flash.img of=06_Primary_Settings.img	bs=1 skip=0		count=65536
dd if=11_2nd_Flash.img of=07_Configuration.img		bs=1 skip=65536		count=393216
dd if=11_2nd_Flash.img of=08_Reserve_1.img		bs=1 skip=458752	count=65536
dd if=11_2nd_Flash.img of=09_Code_Image_1.img		bs=1 skip=524288	count=7864320
