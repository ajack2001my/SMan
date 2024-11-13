copy sman.%1 sman.rss
del c:\symbian\uiq_70\epoc32\data\z\system\apps\sman\sman.rsc
call abld build armi urel
call \ex.bat
cd release
call m
cd ..
