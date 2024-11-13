Here are the sources to SMan v1.3

To install,
1) Make sure you have the Symbian v7.0 SDK installed.
2) Create a folder called C:\p800dev\SMAN
3) Extract the contents of the ZIP file to C:\p800dev\SMAN
4) Move EX.BAT in C:\p800dev\SMAN to the root of your C: drive
5) Copy C:\p800dev\SMAN\RELEASE\SMan.AIF to C:\ (you only need to do this once unless you change the AIF contents)
6) Create the following files in C:\
- SMan.cfg, 0 bytes
- SMan.snp, 0 bytes
- SMan.log.txt, 0 bytes
- SMan.boot, 0 bytes
7) Move SMan.vcf to C:\ 
7) Type "BLDMAKE BLDFILES" from the command line (you only need to do this once)
8) Type "L" from C:\p800dev\SMAN and it should compile. But it will fail in making SMAN.SIS

You will still need to package the output manually using makesis.exe (part of the SDK). I have included sman.pkg.1.3 for your reference. 

Note, I've got the tool chain setup to assume:
1) The SDK is installed in C:\Symbian
2) The SMAN sources is installed in C:\p800dev\SMAN

A few more things to be aware of: 
---------------------------------
v1.3 is quite a complete restructuring of the view management code for flip closed. SMan is getting more and more complicated but is also maturing nicely. There are some things I would like to do differently (in retrospect) but I suspect it will probably be held off indefinitely. :)

It looks like SMan is fast approaching the 'limits' of what can be done with available / public APIs. There is so much more I want to put in but the lack of published APIs is hindering progress. Reverse engineering LIB files has never really been my liking since it's risky (DLLs may change) plus it reduces compatibility across different hardware. However, users are still pouring in with suggestions - which is a good thing.

Other than that, happy learning. :)

Regards,
the drollercoaster. :)

