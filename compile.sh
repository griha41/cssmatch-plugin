if [ $# -eq 1 ]
then
	make -f $1 2>debug_make.txt
	more debug_make.txt | grep "\(error\|erreur\)"
else
	echo "Please specify the name of the makefile"
fi
