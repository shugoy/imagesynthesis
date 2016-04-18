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

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Poisson Blending OpenCV3.0\n";
    
//    if (argc == 0) {
//        std::cout << std::endl;
//        return 1;
//    }
    
//    string dstname = argv[1];
//    string srcname = argv[2];
//    string maskname = argv[3];
    
    cv::Mat src, dst, mask, blend;
    dst =  cv::imread("takedasoun.jpg",1);
    src =  cv::imread("takedayubun.jpg",1);
    mask = cv::imread("takedayubun_mask.png",0);

//    src = cv::imread("vinci_src.png",1);
//    dst = cv::imread("vinci_dst.png",1);
//    mask = cv::imread("vinci_mask.png",0);
    if (src.empty() || dst.empty() || mask.empty()) {
        return 1;
    }
    cv::imshow("src", src);
    cv::imshow("dst", dst);
    cv::imshow("mask", mask);
    
    //cv::Point p0(dst.cols/2, dst.rows/2);
    //cv::Point p_src(src.cols/2, src.rows/2);
    //cv::Point p_src(30,50);
    //cv::Point p0(150,150);
    cv::Point p(0,0);// = p0 + p_src;
    blend = dst.clone();
    
//    Mat gray = Mat(mask.size(),CV_8UC1);
//    if(mask.channels() == 3)
//        cvtColor(mask, gray, COLOR_BGR2GRAY );
//    else
//        gray = mask;
//    
//    int minx = INT_MAX, miny = INT_MAX, maxx = INT_MIN, maxy = INT_MIN;
//    int h = mask.size().height;
//    int w = mask.size().width;
//    for(int i=0;i<h;i++) {
//        for(int j=0;j<w;j++) {
//            if(gray.at<uchar>(i,j) == 255) {
//                minx = std::min(minx,j);
//                maxx = std::max(maxx,j);
//                miny = std::min(miny,i);
//                maxy = std::max(maxy,i);
//            }
//        }
//    }
//    
//    int lenx = maxx - minx;
//    int leny = maxy - miny;
//    p.x += lenx/2;
//    p.y += leny/2;
    
    time_t start;
    seamlessClone(src, dst, mask, p, blend, NORMAL_CLONE); // NORMAL_CLONE || MIXED_CLONE || FEATURE_EXCHANGE
    time_t end;
    std::cout << (double)(end - start)/CLOCKS_PER_SEC << " sec" << std::endl;
    
    imshow("seamlessClone", blend);
    cv::waitKey();
    
    return 0;
}
//
//void alphaBlending(cv::Mat src, cv::Mat dst, cv::Mat alphamat, cv::Point p, cv::Mat& output) {
//    output = cv::Mat(dst.size(), CV_8UC3);
//    for (int i=0; i<src.rows; i++) {
//        for (int j=0; j<src.cols; j++) {
//            int y = i + p.y;
//            int x = j + p.x;
//            if (x < 0 || x >= dst.cols || y < 0 || y >= dst.rows) continue;
//            
//            cv::Vec3b src_rgb = src.at<cv::Vec3b>(i,j);
//            cv::Vec3b dst_rgb = dst.at<cv::Vec3b>(y,x);
//            float alpha = (float)alphamat.at<uchar>(i,j) / 255.0;
//            
//            cv::Vec3b out_rgb = src_rgb * alpha + dst_rgb * (1.0 - alpha);
//            
//            output.at<cv::Vec3b>(y,x) = out_rgb;
//        }
//    }
//};
//
//void myBlending(cv::Mat src, cv::Mat dst, cv::Point p, cv::Mat& output) {
//    src.convertTo(src, CV_32FC3, 1/255.0);
//    dst.convertTo(dst, CV_32FC3, 1/255.0);
//    output = dst.clone();
//    for (int i=0; i<src.rows; i++) {
//        for (int j=0; j<src.cols; j++) {
//            int y = i + p.y;
//            int x = j + p.x;
//            if (x < 0 || x >= dst.cols || y < 0 || y >= dst.rows) continue;
//            
//            cv::Vec3f src_rgb = src.at<cv::Vec3f>(i,j);
//            cv::Vec3f dst_rgb = dst.at<cv::Vec3f>(y,x);
//            cv::Vec3f out_rgb;
//            if (dst.at<cv::Vec3f>(y,x) == cv::Vec3f(0,0,0)) {
//                out_rgb = src_rgb;
//            }
//            else {
//                out_rgb = src_rgb * 0.5 + dst_rgb * 0.5;
//            }
//            
//            output.at<cv::Vec3f>(y,x) = out_rgb;
//        }
//    }
//    output.convertTo(output, CV_8UC3, 255);
//};
//
//void mySeamlessClone(cv::Mat tile, cv::Mat &dst, cv::Point pt, int off = 1) {
//    cv::Rect rect(pt, tile.size());
//    cv::Mat roi = dst(rect);
//    cv:Mat mask(tile.rows+off, tile.cols+off, CV_8U, cv::Scalar::all(255));
//    
//    seamlessClone(tile, roi, mask, cv::Point(roi.cols/2, roi.rows/2), roi, NORMAL_CLONE);
//};
//
//int main () {
//    cv::Mat tile = imread("texture.png");
//    cv::Mat tile2 = imread("texture_grass.png");
//    
//    int numi, numj;
//    numi = 3;
//    numj = 3;
//    
//    // create base img (lattice)
//    cv::Mat dst = cv::Mat(tile.rows * numi - numi + 1, tile.cols * numj  - numj + 1, CV_8UC3, cv::Scalar(0,0,0));
//    for (int i=0; i<numi; i++) {
//        for (int j=0; j<numj; j++) {
//            if ((i+j)%2 == 0) {
//                myBlending(tile, dst, cv::Point((tile.cols-1)*j,(tile.rows-1)*i), dst);
//            }
//            else {
//                myBlending(tile2, dst, cv::Point((tile.cols-1)*j,(tile.rows-1)*i), dst);
//            }
//        }
//    }
//    cv::imshow("dst", dst);
//    cv::Mat dstmask(dst.size(), CV_8UC3, cv::Scalar::all(0));
//    for (int i=0; i<=numi; i++) {
//        cv::line(dstmask, cv::Point(0,(tile.rows-1)*i), cv::Point(dstmask.cols-1, (tile.rows-1)*i), cv::Scalar::all(1));
//    }
//    for (int i=0; i<=numj; i++) {
//        cv::line(dstmask, cv::Point((tile.cols-1)*i,0), cv::Point((tile.cols-1)*i,dstmask.rows-1), cv::Scalar::all(1));
//    }
//    dst = dst.mul(dstmask);
//    cv::imshow("dstmasked", dst);
//    cv::waitKey();
//    
//    // LOOP
//    for (int i=0; i<numi; i++) {
//        for (int j=0; j<numj; j++) {
//            cv::Point pt((tile.cols-1)*j,(tile.rows-1)*i);
//            
//            mySeamlessClone(tile, dst, pt);
////            if ((i+j)%2 == 0) mySeamlessClone(tile, dst, pt);
////            else mySeamlessClone(tile2, dst, pt);
//            
//            cv::imshow("output", dst);
//            cv::waitKey();
//        }
//    }
////    for (int i=0; i<numi; i++) {
////        for (int j=0; j<numj; j++) {
////            //imshow("mask"+to_string(i)+to_string(j), mask);
////            cv::Point pt((tile.cols-1)*j,(tile.rows-1)*i);
////            //            cv::Rect rect(pt, tile.size());
////            //            cv::Mat roi = dst(rect);
////            //            cv::imshow("roi0", roi);
////            //            cv:Mat mask(tile.rows+1, tile.cols+1, CV_8U, cv::Scalar::all(255));
////            //
////            if ((i+j)%2 == 0) {
////                mySeamlessClone(tile, dst, pt);
////                //seamlessClone(tile, roi, mask, cv::Point(roi.cols/2, roi.rows/2), roi, NORMAL_CLONE);
////            }
////            else {
////                mySeamlessClone(tile2, dst, pt);
////                //seamlessClone(tile2, roi, mask, cv::Point(roi.cols/2, roi.rows/2), roi, NORMAL_CLONE);
////            }
////            mySeamlessClone(tile, dst, pt);
////            //cv::imshow("roi", roi);
////            cv::imshow("output", dst);
////            cv::waitKey();
////        }
////    }
//    cv::imshow("output", dst);
//    cv::waitKey();
//    
//    
//    
//    
//    return 0;
//}








