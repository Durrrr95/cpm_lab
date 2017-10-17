
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>


#include "opencv2/opencv.hpp"

#include "apriltag.h"
#include "tag36h11.h"
#include "tag36h10.h"
#include "tag36artoolkit.h"
#include "tag25h9.h"
#include "tag25h7.h"
#include "common/getopt.h"
#include <chrono>

using namespace std;
using namespace cv;




std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
        result += buffer.data();
    }
    return result;
}


chrono::system_clock::time_point start;

void tic() {start = std::chrono::high_resolution_clock::now();}
long long  toc() {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
}


int main(int argc, char *argv[])
{
    getopt_t *getopt = getopt_create();

    getopt_add_bool(getopt, 'h', "help", 0, "Show this help");
    getopt_add_bool(getopt, 'd', "debug", 0, "Enable debugging output (slow)");
    getopt_add_bool(getopt, 'q', "quiet", 0, "Reduce output");
    getopt_add_string(getopt, 'f', "family", "tag36h11", "Tag family to use");
    getopt_add_int(getopt, '\0', "border", "1", "Set tag family border size");
    getopt_add_int(getopt, 't', "threads", "4", "Use this many CPU threads");
    getopt_add_double(getopt, 'x', "decimate", "1.0", "Decimate input image by this factor");
    getopt_add_double(getopt, 'b', "blur", "0.0", "Apply low-pass blur to input");
    getopt_add_bool(getopt, '0', "refine-edges", 1, "Spend more time trying to align edges of tags");
    getopt_add_bool(getopt, '1', "refine-decode", 0, "Spend more time trying to decode tags");
    getopt_add_bool(getopt, '2', "refine-pose", 0, "Spend more time trying to precisely localize tags");

    if (!getopt_parse(getopt, argc, argv, 1) ||
            getopt_get_bool(getopt, "help")) {
        printf("Usage: %s [options]\n", argv[0]);
        getopt_do_usage(getopt);
        exit(0);
    }

    // Initialize camera
    /*VideoCapture cap("/home/janis/Downloads/VID_20171005_105051.mp4");
    if (!cap.isOpened()) {
        cerr << "Couldn't open video capture device" << endl;
        return -1;
    }*/

    // Initialize tag detector with options
    apriltag_family_t *tf = NULL;
    const char *famname = getopt_get_string(getopt, "family");
    if (!strcmp(famname, "tag36h11"))
        tf = tag36h11_create();
    else if (!strcmp(famname, "tag36h10"))
        tf = tag36h10_create();
    else if (!strcmp(famname, "tag36artoolkit"))
        tf = tag36artoolkit_create();
    else if (!strcmp(famname, "tag25h9"))
        tf = tag25h9_create();
    else if (!strcmp(famname, "tag25h7"))
        tf = tag25h7_create();
    else {
        printf("Unrecognized tag family name. Use e.g. \"tag36h11\".\n");
        exit(-1);
    }
    tf->black_border = getopt_get_int(getopt, "border");

    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);
    td->quad_decimate = getopt_get_double(getopt, "decimate");
    td->quad_sigma = getopt_get_double(getopt, "blur");
    td->nthreads = getopt_get_int(getopt, "threads");
    td->debug = getopt_get_bool(getopt, "debug");
    td->refine_edges = getopt_get_bool(getopt, "refine-edges");
    td->refine_decode = getopt_get_bool(getopt, "refine-decode");
    td->refine_pose = getopt_get_bool(getopt, "refine-pose");


    string files_str = exec("find /home/janis/sciebo/CPM-Lab/04_Measurement/AprilTag_MotionBlurTest/Light_Floodlight__Exposure_500us__MotorCurrent_2A__Gain_15__Distanz_140cm -type f | sort -r");

    vector<string> files;
    {
        std::string delimiter = "\n";
        size_t pos = 0;
        while ((pos = files_str.find(delimiter)) != std::string::npos) {
            std::string token = files_str.substr(0, pos);
            std::cout << token << endl << "======" << std::endl;
            files.push_back(token);
            files_str.erase(0, pos + delimiter.length());
        }
    }

    Mat frame, gray;
    std::vector<cv::Rect> ROIs;
    while (true) {
        std::vector<cv::Rect> ROIs_new;
        //cap >> frame;
        if(files.empty()) return 0;
        frame = imread(files.back());
        files.pop_back();
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        if(ROIs.empty()) {
            ROIs.push_back(cv::Rect(0, 0, frame.cols, frame.rows));
        }

        for(int idx_roi = 0; idx_roi < ROIs.size(); idx_roi++) {
            auto ROI = ROIs[idx_roi];
            cout << "ROI: (" << ROI.x << ", " << ROI.y << ", " << ROI.width << ", " << ROI.height << ")" << endl;
            Mat gray_cropped = gray(ROI);
            //imshow("Tag Detections", gray_cropped);
            //waitKey(0);

            // Make an image_u8_t header for the Mat data
            image_u8_t im = {.width = gray_cropped.cols,
                    .height = gray_cropped.rows,
                    .stride = gray_cropped.step.buf[0],
                    .buf = gray_cropped.data
            };

            tic();
            zarray_t *detections = apriltag_detector_detect(td, &im);
            cout << zarray_size(detections) << " tags detected" << endl;
            cout << toc() << " usec" << endl;

            // Draw detection outlines
            for (int i = 0; i < zarray_size(detections); i++) {
                apriltag_detection_t *det;
                zarray_get(detections, i, &det);
                line(frame, Point(ROI.x + det->p[0][0], ROI.y + det->p[0][1]),
                     Point(ROI.x + det->p[1][0], ROI.y + det->p[1][1]),
                     Scalar(0, 0xff, 0), 2);
                line(frame, Point(ROI.x + det->p[0][0], ROI.y + det->p[0][1]),
                     Point(ROI.x + det->p[3][0], ROI.y + det->p[3][1]),
                     Scalar(0, 0, 0xff), 2);
                line(frame, Point(ROI.x + det->p[1][0], ROI.y + det->p[1][1]),
                     Point(ROI.x + det->p[2][0], ROI.y + det->p[2][1]),
                     Scalar(0xff, 0, 0), 2);
                line(frame, Point(ROI.x + det->p[2][0], ROI.y + det->p[2][1]),
                     Point(ROI.x + det->p[3][0], ROI.y + det->p[3][1]),
                     Scalar(0xff, 0, 0), 2);

                stringstream ss;
                ss << "#" << det->id;
                String text = ss.str();
                int fontface = FONT_HERSHEY_SCRIPT_SIMPLEX;
                double fontscale = 1.0;
                int baseline;
                Size textsize = getTextSize(text, fontface, fontscale, 2,
                                            &baseline);
                putText(frame, text, Point(ROI.x + det->c[0] - textsize.width / 2,
                                           ROI.y + det->c[1] + textsize.height / 2),
                        fontface, fontscale, Scalar(0xff, 0x99, 0), 2);

                // new ROI
                {
                    int roi_size = 200;
                    int new_roi_x = ROI.x + det->c[0] - roi_size/2;
                    int new_roi_y = ROI.y + det->c[1] - roi_size/2;
                    int new_roi_width = roi_size;
                    int new_roi_height = roi_size;

                    if(new_roi_x < 0) new_roi_x = 0;
                    if(new_roi_y < 0) new_roi_y = 0;
                    if(new_roi_x + new_roi_width > frame.cols) new_roi_width = frame.cols - new_roi_x;
                    if(new_roi_y + new_roi_height > frame.rows) new_roi_height = frame.rows - new_roi_y;

                    if(new_roi_width > 10 && new_roi_height > 10) {
                        ROIs_new.push_back(Rect(new_roi_x, new_roi_y, new_roi_width, new_roi_height));
                    }
                    else {
                        cerr << "ROI too small (" << new_roi_x << ", " << new_roi_y << ", " << new_roi_width << ", " << new_roi_height << ")"  << endl;
                    }
                }
            }
            zarray_destroy(detections);
        }

        imshow("Tag Detections", frame);
        waitKey(0);
        //if (waitKey(1) >= 0) break;

        ROIs = ROIs_new;
    }

    apriltag_detector_destroy(td);
    if (!strcmp(famname, "tag36h11"))
        tag36h11_destroy(tf);
    else if (!strcmp(famname, "tag36h10"))
        tag36h10_destroy(tf);
    else if (!strcmp(famname, "tag36artoolkit"))
        tag36artoolkit_destroy(tf);
    else if (!strcmp(famname, "tag25h9"))
        tag25h9_destroy(tf);
    else if (!strcmp(famname, "tag25h7"))
        tag25h7_destroy(tf);
    getopt_destroy(getopt);

    return 0;
}



/*
    double dWidth = inputVideo.get(CV_CAP_PROP_FRAME_WIDTH); 
    double dHeight = inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT); 
    double fps = inputVideo.get(CV_CAP_PROP_FPS); 

    cv::Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

    cv::VideoWriter outputVideo("out.avi", CV_FOURCC('M','J','P','G'), fps, frameSize, true);
    */