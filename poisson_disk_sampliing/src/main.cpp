//
//  main.cpp
//  poisson_disk_sampling
//
//  Created by 山口 周悟 on 2016/04/11.
//  Copyright © 2016年 shugo. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

float calcDistance(cv::Point pt1, cv::Point pt2) {
    float x = pt1.x - pt2.x;
    float y = pt1.y - pt2.y;
    return sqrt(x*x + y*y);
}

void drawdot(cv::Mat& img, cv::Point pt, cv::Vec3b color) {
    img.at<Vec3b>(pt) = color;
}


int main() {
    
    int width = 512;
    int height = 512;
    
    float th_distance = 10;
    auto color = cv::Vec3b(0,0,0);
    
    cv::Mat canvas(width, height, CV_8UC3, cv::Scalar::all(255));
    
    std::vector<cv::Point> drawed_points;
    while (1)
    {
        cv::Point temp_pt(rand()%width, rand()%height);
        
        if (drawed_points.size() < 1) {
            drawdot(canvas, temp_pt, color);
            drawed_points.push_back(temp_pt);
        }
        else {
            bool flag = true;
            for (int i=0; i<drawed_points.size(); i++) {
                float distance = calcDistance(temp_pt, drawed_points[i]);
                if (distance < th_distance) {
                    flag = false;
                    break;
                }
            }
            if (flag == true) {
                drawdot(canvas, temp_pt, color);
                drawed_points.push_back(temp_pt);
            }
            else {
                continue;
            }
        }
        
        
        cv::imshow("canvas", canvas);
        char key = waitKey(1);
        if (key == 27) {
            break;
        } else if (key == 32) {
            waitKey();
        }
    }
    
    

    
    return 0;
}
