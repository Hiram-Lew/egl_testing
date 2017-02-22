#!/bin/bash

gcc egl2.cpp -pthread -L/usr/lib/nvidia-367 -lGL -L/usr/lib/nvidia-367 -lEGL
echo "Compile done."

killall -9 a.out
sleep 3
for i in {0..1000}
do
      /bin/bash -c ./a.out 2>/dev/null 2>/dev/null &
done

sleep 3
./a.out
killall a.out
