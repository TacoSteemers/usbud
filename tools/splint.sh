find ../code/ -type f -name *.c~ -exec rm {} \;;
find ../code/ -type f -name *.h~ -exec rm {} \;;
splint ../code/* -warnposix;
