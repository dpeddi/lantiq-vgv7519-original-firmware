#!/usr/bin/perl
#./split.pl <05_Code_Image_0.img

undef $/;
$_ = <>;
$n = 0;

#(?=\xcd\xab\x89\x67/
#for $match (split(/(?=\x32\x54\x76\x98)|(?=\xcd\xab\x89\x67)|(?=\x5d\x00\x00\x80)|(?=\xff\xff\xff\xff\xff\xff\xff\xff)/)) {
for $match (split(/(?=\x32\x54\x76\x98)|(?=\x5d\x00\x00\x80)|(?=\xff\xff\xff\xff\xff\xff\xff\xff)/)) {
      if ($match =~ /\x32\x54\x76\x98.*/) {
        $fname=sprintf("%02d",++$n)."_code".".obfuscated";
#      } elsif ($match =~ /\xcd\xab\x89\x67.*/) {
#        $fname=sprintf("%02d",++$n)."_sep.obfuscated";    
      } elsif ($match =~ /\x5d\x00\x00\x80.*/) {
        $fname=sprintf("%02d",++$n)."_firmware.lzma";
      } else {
        #$fname=sprintf("%02d",++$n)."_unknown.txt";
        undef $fname;
      }
      if (defined($fname)) {
        open(O, ">$fname");
        printf("size: %08x fname: %-20s\n",length($match),$fname);
        print O $match;
        close(O);
      }
}
