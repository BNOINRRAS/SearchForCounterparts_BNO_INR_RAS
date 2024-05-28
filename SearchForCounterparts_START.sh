#!/bin/bash
rm SearchForCounterparts
g++ -o SearchForCounterparts main.cpp checkfile.cpp comparison.cpp readfile.cpp sendemail.cpp starcoordinates.cpp checkfile.hpp comparison.hpp readfile.hpp sendemail.hpp starcoordinates.hpp -lcurl
./SearchForCounterparts 43200
