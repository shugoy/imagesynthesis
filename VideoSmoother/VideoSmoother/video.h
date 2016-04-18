//
//  video.h
//  PoissonBlending
//
//  Created by 山口 周悟 on 2015/02/07.
//  Copyright (c) 2015年 山口 周悟. All rights reserved.
//

#ifndef PoissonBlending_video_h
#define PoissonBlending_video_h

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace video {
    vector<Mat> videocapture(const char* videoname);
    void videopreview(vector<Mat> &video, int duration, int fps);
    void videowriter(string filename, vector<Mat> &video, int duration, int fps);
}
#endif
