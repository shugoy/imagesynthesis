//
//  main.cpp
//  opencvtest
//
//  Created by 山口 周悟 on 2015/12/05.
//  Copyright © 2015年 shugo. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>

using namespace std;
using namespace cv;


void alphaBlending(cv::Mat src, cv::Mat dst, cv::Mat alphamat, cv::Point p, cv::Mat& output) {
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

void myBlending(cv::Mat src, cv::Mat dst, cv::Point p, cv::Mat& output) {
    src.convertTo(src, CV_32FC3, 1/255.0);
    dst.convertTo(dst, CV_32FC3, 1/255.0);
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

void mySeamlessClone(cv::Mat tile, cv::Mat &dst, cv::Point pt, int off = 1) {
    cv::Rect rect(pt, tile.size());
    cv::Mat roi = dst(rect);
    cv:Mat mask(tile.rows+off, tile.cols+off, CV_8U, cv::Scalar::all(255));
    
    seamlessClone(tile, roi, mask, cv::Point(0, 0), roi, NORMAL_CLONE);
};

int main (int argc, char* argv[]) {
    
    string imgname1 = argv[1];
    string imgname2 = argv[2];
    
    cv::Mat tile = imread(imgname1);
    cv::Mat tile2 = imread(imgname2);
    
    int numi, numj;
    numi = atoi(argv[3]);
    numj = atoi(argv[4]);
    
    // create base img (lattice)
    cv::Mat dst = cv::Mat(tile.rows * numi - numi + 1, tile.cols * numj  - numj + 1, CV_8UC3, cv::Scalar(0,0,0));
    for (int i=0; i<numi; i++) {
        for (int j=0; j<numj; j++) {
            if ((i+j)%2 == 0) {
                myBlending(tile, dst, cv::Point((tile.cols-1)*j,(tile.rows-1)*i), dst);
            }
            else {
                myBlending(tile2, dst, cv::Point((tile.cols-1)*j,(tile.rows-1)*i), dst);
            }
        }
    }
    cv::imshow("dst", dst);
    cv::Mat dstmask(dst.size(), CV_8UC3, cv::Scalar::all(0));
    for (int i=0; i<=numi; i++) {
        cv::line(dstmask, cv::Point(0,(tile.rows-1)*i), cv::Point(dstmask.cols-1, (tile.rows-1)*i), cv::Scalar::all(1));
    }
    for (int i=0; i<=numj; i++) {
        cv::line(dstmask, cv::Point((tile.cols-1)*i,0), cv::Point((tile.cols-1)*i,dstmask.rows-1), cv::Scalar::all(1));
    }
    dst = dst.mul(dstmask);
    cv::imshow("dstmasked", dst);
    cv::waitKey();
    
    // LOOP
    for (int i=0; i<numi; i++) {
        for (int j=0; j<numj; j++) {
            cv::Point pt((tile.cols-1)*j,(tile.rows-1)*i);
            
            //cv::seamlessClone(tile, dst, <#InputArray mask#>, <#Point p#>, <#OutputArray blend#>, <#int flags#>)
            mySeamlessClone(tile, dst, pt);
//            if ((i+j)%2 == 0) mySeamlessClone(tile, dst, pt);
//            else mySeamlessClone(tile2, dst, pt);
            
            cv::imshow("output", dst);
            cv::waitKey();
        }
    }
//    for (int i=0; i<numi; i++) {
//        for (int j=0; j<numj; j++) {
//            //imshow("mask"+to_string(i)+to_string(j), mask);
//            cv::Point pt((tile.cols-1)*j,(tile.rows-1)*i);
//            //            cv::Rect rect(pt, tile.size());
//            //            cv::Mat roi = dst(rect);
//            //            cv::imshow("roi0", roi);
//            //            cv:Mat mask(tile.rows+1, tile.cols+1, CV_8U, cv::Scalar::all(255));
//            //
//            if ((i+j)%2 == 0) {
//                mySeamlessClone(tile, dst, pt);
//                //seamlessClone(tile, roi, mask, cv::Point(roi.cols/2, roi.rows/2), roi, NORMAL_CLONE);
//            }
//            else {
//                mySeamlessClone(tile2, dst, pt);
//                //seamlessClone(tile2, roi, mask, cv::Point(roi.cols/2, roi.rows/2), roi, NORMAL_CLONE);
//            }
//            mySeamlessClone(tile, dst, pt);
//            //cv::imshow("roi", roi);
//            cv::imshow("output", dst);
//            cv::waitKey();
//        }
//    }
    cv::imshow("output", dst);
    cv::waitKey();
    
    
    
    
    return 0;
}








