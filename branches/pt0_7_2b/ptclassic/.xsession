#! /bin/csh -f
xrdb ~/.Xresources
twm &
xclock -geometry 120x120-0+0 &
xterm -geometry 80x30+0-0 -fn 9x15 -name "bottom left" &
exec xterm -geometry +0+0 -fn 9x15 -name "login" -ls
