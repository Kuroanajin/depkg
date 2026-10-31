#!/bin/bash

gamesDir="PATH_TO_GAMES_DIR"
addonsDir="PATH_TO_ADDONS_DIR"

if [ "$gamesDir" == "PATH_TO_GAMES_DIR" ]; then
	echo You need to update gamesDir with the games path used by ShadPs4
	echo "Press [enter] to close"
	if [ "$2" != "--batch" ]; then read ; fi
	exit
fi

if [ "$addonsDir" == "PATH_TO_ADDONS_DIR" ]; then
	echo You need to update addonsDir with the addons path used by ShadPs4
	echo "Press [enter] to close"
	if [ "$2" != "--batch" ]; then read ; fi
	exit
fi

extractor=`dirname $0`/depkg
if [ ! -x "$extractor" ]; then
	extractor=`dirname $0`/depkg.AppImage
fi

"$extractor" "$1" --check-type

ret="$?"

if [ "$ret" -lt 101 ] || [ "$ret" -gt 103 ]; then
	echo "An error has occurred."
else
	if [ "$ret" -eq 101 ]; then
		echo The file is a base game, installing to $gamesDir
	elif [ "$ret" -eq 102 ]; then
		echo The file is a game update, installing to $gamesDir
	elif [ "$ret" -eq 103 ]; then
		echo The file is a dlc, installing to $addonsDir
		gamesDir=$addonsDir
	fi
	
	"$extractor" "$1" $gamesDir
fi

echo "Press [enter] to close"
if [ "$2" != "--batch" ]; then read ; fi

