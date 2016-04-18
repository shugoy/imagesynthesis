//
//  main.cpp
//  VideoSmoother
//
//  Created by 山口 周悟 on 2015/08/05.
//  Copyright (c) 2015年 山口 周悟. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "video.h"

using namespace std;
using namespace cv;


//void swap(int* x, int* y)
//{
//    int z = 0;
//    z = *x;
//    *x = *y;
//    *y = z;
//}

float BGR2GRAY(Vec3f bgr) {
    float luminancef;
    luminancef = 0.299*(float)bgr[0] + 0.587*(float)bgr[1] + 0.114*(float)bgr[2];
    return luminancef;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    vector<Mat> video1 = video::videocapture("kumo2anime1.mov");
    //video::videopreview(video1, (int)video1.size(), 30);
    vector<Mat> newvideo = video1;
    
    int frames = (int)video1.size();
    
    vector<Mat> videof = video1;
    for (int i=0; i<frames; i++) {
        videof[i].convertTo(videof[i], CV_32FC3);
    }
    vector<Mat> newvideof = videof;
    
    int count = 0;
    
    for (int f=0; f<frames; f++) {
        
        for (int y=0; y<videof[0].rows; y++) {
            
            for (int x=0; x<videof[0].cols; x++) {
                
                //cv::Vec3b col = video1[f].at<Vec3b>(y,x);
                
                //std::cout << col << std::endl;
                //std::cout << (int)col[0] << std::endl;
                
                // costract median block
                vector<Vec3f> medianblock;
                vector<float> medianblock_luminance;
                vector<int> medianblock_id;
                for (int i=-2; i<=2; i++) {
                    if (f+i >= 0 && f+i < frames) {
                        Vec3f col = videof[f+i].at<Vec3f>(y,x);
//                        vector<int> vcol(3);
//                        for (int c=0; c<3; c++) vcol[c] = (int)col[c];
                        medianblock.push_back(col);
                        medianblock_luminance.push_back(BGR2GRAY(col));
                        medianblock_id.push_back(i);
                    }
                }
                
                // sort
                for (int i=0; i<medianblock.size()-1; i++) {
                    for (int j=0; j<medianblock.size()-1-i; j++) {
                        if (medianblock_luminance[j] < medianblock_luminance[j+1]) {
                            Vec3f temp = medianblock[j];
                            //for (int c=0; c<3; c++) temp[c] = medianblock[j][c];
                            medianblock[j] = medianblock[j+1];
                            medianblock[j+1] = temp;
                            
                            float tempL = medianblock_luminance[j];
                            medianblock_luminance[j] = medianblock_luminance[j+1];
                            medianblock_luminance[j+1] = tempL;
                            
                            int tempid = medianblock_id[j];
                            medianblock_id[j] = medianblock_id[j+1];
                            medianblock_id[j+1] = tempid;
                        }
                    }
                }
                
                // update
                int id0 = (int)medianblock.size()/2;
                int id = medianblock_id[id0];
                newvideof[f].at<Vec3f>(y,x) = videof[f+id].at<Vec3f>(y,x);
            }
        }
        
        newvideof[f].convertTo(newvideo[f], CV_8UC3);
        
        cv::imshow("currentframe"+to_string(f), newvideo[f]);
        cv::imwrite("output/currentframe"+to_string(f)+".png", newvideo[f]);
        cv::destroyWindow("currentframe"+to_string(f-1));
        
        count ++;
    }
    
    for (int i=0; i<(int)video1.size(); i++) {
        newvideof[i].convertTo(newvideo[i], CV_8UC3);
    }
    
    video::videopreview(newvideo, (int)newvideo.size(), 30);
    video::videowriter("newvideo1", newvideo, frames, 30);
    
    
    return 0;
}
