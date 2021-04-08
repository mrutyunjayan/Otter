#!/bin/bash

code="$PWD"
opts=-g
cd build > /dev/null
cc $opts $code/code\SDL_game.c -o game
cd $code > /dev/null
