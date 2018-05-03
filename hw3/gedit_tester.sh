#!/bin/bash

export DISPLAY=':0.0'
export XAUTHORITY="/home/username/.Xauthority"

TEXT="xdotool lets you programatically (or manually) simulate keyboard input and mouse activity, move	and resize windows, etc. It does this using X11's XTEST extension and other Xlib functions."

rm new_test_file 2>/dev/null
touch new_test_file
CURDIR=$(pwd)
TERMWID=$(xdotool getactivewindow)
xdotool key "ctrl+alt+t"
sleep 1

TERM2WID=$(xdotool getactivewindow)
xdotool type "cd $CURDIR"
xdotool key Return
xdotool type "gedit new_test_file"
xdotool key Return
sleep 1

GEDITWID=$(xdotool search "new_test_file" 2>/dev/null)
xdotool windowactivate --sync $GEDITWID
xdotool windowfocus --sync $GEDITWID
xdotool key Home
xdotool type "$TEXT"
xdotool key "ctrl+s"
xdotool key "ctrl+q"

xdotool windowactivate --sync $TERM2WID
xdotool windowfocus --sync $TERM2WID
xdotool key "ctrl+c"
xdotool type "exit"
xdotool key Return

RES=$(cat new_test_file)
echo "file: $RES"
echo "----------------------"
if [[ $RES = $TEXT ]]; then 
    echo "gedit works correctly"
else 
    echo "error"
fi
echo "----------------------"
rm new_test_file 2>/dev/null

