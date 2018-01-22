#!/usr/bin/sh

ffmpeg -i /dev/video0 -c:v mpeg2video -q:v 20 -pix_fmt yuv420p -g 1 -threads 2 -f mpegts - | nc -lt 9000
