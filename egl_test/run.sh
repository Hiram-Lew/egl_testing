#!/bin/bash

gcc egl.cpp -L/usr/lib/nvidia-367 -lGL -L/usr/lib/nvidia-367 -lEGL -o egl

for i in {1..130}
do
      /bin/bash -c ./egl &
done
