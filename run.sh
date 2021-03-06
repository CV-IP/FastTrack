#!/bin/bash

if [ "$1" = "full" ]; then
  set -e
  qmake CONFIG+=released src/FastTrack.pro
  make clean
  make
  cd build
  ./FastTrack
  cd ..
  make clean
fi

if [ "$1" = "partial" ]; then
  set -e
  qmake CONFIG+=released src/FastTrack.pro
  make
  cd build
  ./FastTrack
  cd ..
fi

if [ "$1" = "debug" ]; then
  set -e
  qmake CONFIG+=debug src/FastTrack.pro
  make
  cd build
  ./FastTrack
  cd ..
fi


if [ "$1" = "profile" ]; then
  set -e
  qmake QMAKE_CXXFLAGS+=-pg QMAKE_LFLAGS+=-pg CONFIG+=debug src/FastTrack.pro
  make
  cd build
  ./FastTrack
  cd ..
  gprof build/FastTrack build/gmon.out > profiling.txt
fi

if [ "$1" = "ci" ]; then
  set -e
  qmake CONFIG+=released src/FastTrack.pro
  make clean
  make
  cd build
fi

if [ "$1" = "cli" ]; then
  set -e
  qmake CONFIG+=released src/FastTrack-Cli.pro
  make clean
  make
  cd build_cli
fi
