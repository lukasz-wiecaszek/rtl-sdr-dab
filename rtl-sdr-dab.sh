#!/bin/bash

FIFO_FILENAME=msc.fifo

mkfifo $FIFO_FILENAME

gst-launch-1.0 filesrc location=$FIFO_FILENAME ! dabplusparse ! avdec_aac ! audioresample ! audioconvert ! autoaudiosink &
GST_LAUNCH_PID=$!

./rtl-sdr-dab -c ./stations/off_radio_krakow.cfg -o $FIFO_FILENAME

kill $GST_LAUNCH_PID

rm $FIFO_FILENAME