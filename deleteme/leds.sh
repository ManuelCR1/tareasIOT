#!/bin/bash

echo Hello Leds

echo out > /sys/class/gpio/gpio20/direction
echo 1 > /sys/class/gpio/gpio20/value

echo out > /sys/class/gpio/gpio21/direction
echo 1 > /sys/class/gpio/gpio21/value


