// MIT License
// 
// Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// This file is part of cpm_lab.
// 
// Author: i11 - Embedded Software, RWTH Aachen University

#include <stdlib.h>
#include <iostream>
#include <csignal>
#include <string>
#include "labcam/LabCamIface.hpp"

//Suppress warning for unused parameter of main
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


static LabCamIface labcam;

void sigterm_handler(int signal){
    labcam.stopRecording();
};

int main(int argc, char *argv[])
{
    // Connect signal handler for termination signal
    signal(SIGTERM, sigterm_handler);

    // Start recording BY USING INPUT PARAMETERS
    labcam.startRecording(".", "awesome_recording1");

    while(1);
}

#pragma GCC diagnostic pop
