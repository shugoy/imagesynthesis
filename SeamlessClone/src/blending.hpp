//
//  main.cpp
//  opencvtest
//
//  Created by 山口 周悟 on 2015/12/05.
//  Copyright © 2015年 shugo. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


namespace my {
    
    void AlphaBlending(cv::Mat src, cv::Mat dst, cv::Mat alphamat, cv::Point p, cv::Mat& output) {
        output = cv::Mat(dst.size(), CV_8UC3);
        for (int i=0; i<src.rows; i++) {
            for (int j=0; j<src.cols; j++) {
                int y = i + p.y;
                int x = j + p.x;
                if (x < 0 || x >= dst.cols || y < 0 || y >= dst.rows) continue;
                
                cv::Vec3b src_rgb = src.at<cv::Vec3b>(i,j);
                cv::Vec3b dst_rgb = dst.at<cv::Vec3b>(y,x);
                float alpha = (float)alphamat.at<uchar>(i,j) / 255.0;
                
                cv::Vec3b out_rgb = src_rgb * alpha + dst_rgb * (1.0 - alpha);
                
                output.at<cv::Vec3b>(y,x) = out_rgb;
            }
        }
    };
    
    void Blending(cv::Mat src, cv::Mat dst, cv::Point p, cv::Mat& output) {
        assert(src.size() == dst.size());
        src.convertTo(src, CV_32FC3, 1/255.0);
        dst.convertTo(dst, CV_32FC3, 1/255.0);
        //cv::Mat mat_count(dst.size(), CV_32S, cv::Scalar(1));
        output = dst.clone();
        for (int i=0; i<src.rows; i++) {
            for (int j=0; j<src.cols; j++) {
                int y = i + p.y;
                int x = j + p.x;
                if (x < 0 || x >= dst.cols || y < 0 || y >= dst.rows) continue;
                
                cv::Vec3f src_rgb = src.at<cv::Vec3f>(i,j);
                cv::Vec3f dst_rgb = dst.at<cv::Vec3f>(y,x);
                cv::Vec3f out_rgb;
                if (dst.at<cv::Vec3f>(y,x) == cv::Vec3f(0,0,0)) {
                    out_rgb = src_rgb;
                }
                else {
                    out_rgb = src_rgb * 0.5 + dst_rgb * 0.5;
                }
                
                output.at<cv::Vec3f>(y,x) = out_rgb;
            }
        }
        output.convertTo(output, CV_8UC3, 255);
    };
    
    void addBlending(cv::Mat src, cv::Mat dst, cv::Point p, cv::Mat& output) {
        //assert(src.size() == dst.size());
        assert(src.type() == CV_32FC3 && dst.type() == CV_32FC3);
        
        output = dst.clone();
        for (int i=0; i<src.rows; i++) {
            for (int j=0; j<src.cols; j++) {
                int y = i + p.y;
                int x = j + p.x;
                if (x < 0 || x >= dst.cols || y < 0 || y >= dst.rows) continue;
                
                cv::Vec3f src_rgb = src.at<cv::Vec3f>(i,j);
                cv::Vec3f dst_rgb = dst.at<cv::Vec3f>(y,x);
                
                output.at<cv::Vec3f>(y,x) = dst_rgb + src_rgb;
            }
        }
    };
    
    void SeamlessClone(cv::Mat tile, cv::Mat &dst, cv::Point pt) {
        cv::Rect rect(pt, tile.size());
        cv::Mat roi = dst(rect);
        cv:Mat mask(tile.rows, tile.cols, CV_8U, cv::Scalar::all(255));
        
        Mat gray = Mat(mask.size(),CV_8UC1);
        if(mask.channels() == 3)
            cvtColor(mask, gray, COLOR_BGR2GRAY );
        else
            gray = mask;
        
        int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
        int h = mask.size().height;
        int w = mask.size().width;
        for(int i=0;i<h;i++)
        {
            for(int j=0;j<w;j++)
            {
                if(gray.at<uchar>(i,j) == 255)
                {
                    minx = std::min(minx,j);
                    maxx = std::max(maxx,j);
                    miny = std::min(miny,i);
                    maxy = std::max(maxy,i);
                }
            }
        }
        
        int lenx = maxx - minx;
        int leny = maxy - miny;
        pt.x += lenx/2;
        pt.y += leny/2;
        
        seamlessClone(tile, roi, mask, pt, roi, NORMAL_CLONE);
    };
    
    void SeamlessClone(cv::Mat tile, cv::Mat &dst, cv::Mat mask, cv::Point pt) {
        cv::Rect rect(pt, tile.size());
        cv::Mat roi0 = dst(rect);
        cv::Mat roi = dst(rect).clone();
        
        Mat gray = Mat(mask.size(),CV_8UC1);
        if(mask.channels() == 3)
            cvtColor(mask, gray, COLOR_BGR2GRAY );
        else
            gray = mask;
        
        int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
        int h = mask.size().height;
        int w = mask.size().width;
        for(int i=0;i<h;i++)
        {
            for(int j=0;j<w;j++)
            {
                if(gray.at<uchar>(i,j) == 255)
                {
                    minx = std::min(minx,j);
                    maxx = std::max(maxx,j);
                    miny = std::min(miny,i);
                    maxy = std::max(maxy,i);
                }
            }
        }
        
        int lenx = maxx - minx;
        int leny = maxy - miny;
        pt.x += lenx/2 + minx ;
        pt.y += leny/2 + miny ;
        
        seamlessClone(tile, roi, mask, pt, roi, NORMAL_CLONE);
        roi.copyTo(roi0, mask);
    };
    
    
}





