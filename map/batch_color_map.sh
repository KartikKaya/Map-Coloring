#!/bin/sh


sudo apt-get install evince
sudo apt-get install graphviz


compile=$(g++ final_color_map.cpp -o final_colour -std=c++11)

./final_colour 1
