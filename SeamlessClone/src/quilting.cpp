//
//  quilting.cpp
//  ImageQuilting
//
//  Created by 山口 周悟 on 2016/01/27.
//  Copyright © 2016年 shugo. All rights reserved.
//

#include "quilting.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>

#include "blending.hpp"


void quilting(cv::Mat roi1, cv::Mat roi2, std::vector<cv::Point>& path, bool flag_rot = false) {
    
    clock_t start = clock();
    
    assert(roi1.size() == roi2.size());
    
    cv::Mat temp1, temp2;
    if (flag_rot) {
        temp1 = roi1.t();
        temp2 = roi2.t();
    }
    else {
        temp1 = roi1.clone();
        temp2 = roi2.clone();
    }
    
    temp1.convertTo(temp1, CV_32FC3, 1.0/255);
    temp2.convertTo(temp2, CV_32FC3, 1.0/255);
    cvtColor(temp1, temp1, CV_BGR2Lab);
    cvtColor(temp2, temp2, CV_BGR2Lab);
    
    cv::Mat temp = temp1 - temp2;
    temp = temp.mul(temp);
    vector<cv::Mat> planes;
    cv::split(temp, planes);
    
    cv::Mat mat_e = planes[0] + planes[1] + planes[2];
    cv::Mat mat_E = mat_e.clone();
    cv::Mat mat_dir(mat_e.size(), CV_32S, cv::Scalar(0));
    
    // get E mat
    for (int i=1; i<mat_e.rows; i++) {
        for (int j=0; j<mat_e.cols; j++) {
            // calc min(E(i-1,j-1), E(i-1,j), E(i-1,j+1))
            vector<float> v;
            for (int jj=-1; jj<=1; jj++) {
                if (j+jj < 0 || j+jj >= mat_e.cols) continue;
                v.push_back(mat_E.at<float>(i-1,j+jj));
            }
            std::vector<float>::iterator min_itr = std::min_element(std::begin(v), std::end(v));
            
            // update E
            mat_E.at<float>(i,j) = mat_e.at<float>(i,j) + *min_itr;
            
            // get direction
            if (j == 0) mat_dir.at<int>(i,j) = j + (int)std::distance(std::begin(v), min_itr);
            else    mat_dir.at<int>(i,j) = j - 1 + (int)std::distance(std::begin(v), min_itr);
        }
    }
    // get min location on E(i);
    std::vector<float> v;
    mat_E.row(mat_E.rows-1).copyTo(v);
    std::vector<float>::iterator min_itr = std::min_element(std::begin(v), std::end(v));
    int j = (int)std::distance(std::begin(v), min_itr);
    
    path = std::vector<cv::Point>(mat_E.rows);
    for (int i=mat_E.rows-1; i>=0; i--) {
        if (flag_rot) {
            path.at(i) = cv::Point(i,j);
        }
        else path.at(i) = cv::Point(j,i);
        j = mat_dir.at<int>(i,j);
    }
    
    clock_t end = clock();
    std::cout << (double)(end-start)/CLOCKS_PER_SEC * 1000 << "ms" << std::endl;

};

void drawPath(cv::Mat& dst, std::vector<cv::Point> path, cv::Scalar color) {
    for (int i=0; i<path.size(); i++) {
        dst.at<cv::Vec3b>(path.at(i)) = cv::Vec3b(color[0],color[1],color[2]);
    }
};


int main (int argc, char* argv[]) {
    
    std::string imgname1 = argv[1];
    std::string imgname2 = argv[2];
    
    int numi = atoi(argv[3]);
    int numj = atoi(argv[4]);
    int overlap = atoi(argv[5]);
    
    cv::Mat tile = cv::imread(imgname1);
    cv::Mat tile2 = cv::imread(imgname2);
    
    cv::imshow("tile", tile);
    cv::imshow("tile2", tile2);
    
    assert(tile.size() == tile2.size());
    
    cv::Mat dst = cv::Mat((tile.rows-overlap) * numi + overlap,
                          (tile.cols-overlap) * numj + overlap,
                          CV_8UC3, cv::Scalar(0,0,0));
    
    cv::Mat tile_f, tile2_f, dst_f;
    tile.convertTo(tile_f, CV_32FC3, 1/255.0);
    tile2.convertTo(tile2_f, CV_32FC3, 1/255.0);
    dst.convertTo(dst_f, CV_32FC3, 1/255.0);
    cv::Mat mat_count(dst_f.size(), CV_32F, cv::Scalar(0));
    // get base image
    for (int i=0; i<numi; i++) {
        for (int j=0; j<numj; j++) {
            if ((i+j)%2 == 0) {
                my::addBlending(tile_f, dst_f, cv::Point((tile_f.cols-overlap)*j,(tile_f.rows-overlap)*i), dst_f);
            }
            else {
                my::addBlending(tile2_f, dst_f, cv::Point((tile_f.cols-overlap)*j,(tile_f.rows-overlap)*i), dst_f);
            }
            cv::Rect rect((tile_f.cols-overlap)*j,(tile_f.rows-overlap)*i, tile_f.cols, tile_f.rows);
            cv::Mat roi = mat_count(rect);
            roi += 1.0;
        }
    }
    for (int y=0; y<dst_f.rows; y++) {
        for (int x=0; x<dst_f.cols; x++) {
            for (int c=0; c<3; c++) {
                dst_f.at<cv::Vec3f>(y,x)[c] = dst_f.at<cv::Vec3f>(y,x)[c] / mat_count.at<float>(y,x);
            }
        }
    }
    dst_f.convertTo(dst, CV_8UC3, 255);
    cv::imshow("dst", dst);
    cv::Mat dst0 = dst.clone();
    cv::Mat dst_poisson = dst.clone();
    cv::rectangle(dst_poisson, cv::Point(1,1), cv::Point(dst_poisson.cols-2, dst_poisson.rows-2), cv::Scalar(0,0,0), -1);
    //cv::waitKey();
    
    // get quilting path
    std::vector<std::vector<cv::Point>> vec_path_x(0,std::vector<cv::Point>());
    for (int i=0; i<numi; i++) {
        for (int j=0; j<numj-1; j++) {
            cv::Mat roi, roi2;
            std::vector<cv::Point> path;
            cv::Rect rect((tile.cols-overlap)*(j+1),(tile.rows-overlap)*i, overlap, tile.rows);
            cv::Mat roi_dst = dst(rect);
            cv::Mat roi_dp = dst_poisson(rect);
            
            if ((i+j)%2 == 0) {
                roi = tile(cv::Rect(tile.cols-overlap,0,overlap,tile.rows)).clone();
                roi2 = tile2(cv::Rect(0,0,overlap,tile.rows)).clone();
                quilting(roi, roi2, path);
                drawPath(roi_dst, path, cv::Scalar(0,0,255));
            }
            else {
                roi = tile2(cv::Rect(tile.cols-overlap,0,overlap,tile.rows)).clone();
                roi2 = tile(cv::Rect(0,0,overlap,tile.rows)).clone();
                quilting(roi, roi2, path);
                drawPath(roi_dst, path, cv::Scalar(0,255,0));
            }
            vec_path_x.push_back(path);
            
            for (int k=0; k<path.size(); k++) {
                roi_dp.at<cv::Vec3b>(path[k]) = dst0.at<cv::Vec3b>(path[k]);
            }
        }
    }
    std::vector<std::vector<cv::Point>> vec_path_y(0,std::vector<cv::Point>());
    for (int j=0; j<numj; j++) {
        for (int i=0; i<numi-1; i++) {
            cv::Mat roi, roi2;
            std::vector<cv::Point> path;
            cv::Rect rect((tile.cols-overlap)*j,(tile.rows-overlap)*(i+1), tile.cols, overlap);
            cv::Mat roi_dst = dst(rect);
            cv::Mat roi_dp = dst_poisson(rect);
            
            if ((i+j)%2 == 0) {
                roi = tile(cv::Rect(0, tile.rows-overlap,tile.cols,overlap)).clone();
                roi2 = tile2(cv::Rect(0,0,tile.cols,overlap)).clone();
                quilting(roi, roi2, path, true);
                drawPath(roi_dst, path, cv::Scalar(255,255,0));
            }
            else {
                roi = tile2(cv::Rect(0, tile.rows-overlap,tile.cols,overlap)).clone();
                roi2 = tile(cv::Rect(0,0,tile.cols,overlap)).clone();
                quilting(roi, roi2, path, true);
                drawPath(roi_dst, path, cv::Scalar(0,255,255));
            }
            vec_path_y.push_back(path);
            
            for (int k=0; k<path.size(); k++) {
                roi_dp.at<cv::Vec3b>(path[k]) = dst0.at<cv::Vec3b>(path[k]);
            }
        }
    }
    cv::imshow("quilting line", dst);
    cv::imshow("poisson blending with quilting", dst_poisson);
    cv::waitKey();
    
    // synthesize
    for (int i=0; i<numi; i++) {
        for (int j=0; j<numj; j++) {
            std::vector<cv::Point> pathx0,pathx1,pathy0,pathy1;
//            if (j > 0)      pathx0 = vec_path_x.at(i*(numj-1)+j-1);
//            if (j < numj-1) pathx1 = vec_path_x.at(i*(numj-1)+j);
//            if (i > 0)      pathy0 = vec_path_y.at(j*(numi-1)+i-1);
//            if (i < numi-1) pathy1 = vec_path_y.at(j*(numi-1)+i);
            if (j > 0)      copy(vec_path_x.at(i*(numj-1)+j-1).begin(), vec_path_x.at(i*(numj-1)+j-1).end(), back_inserter(pathx0) );
            if (j < numj-1) copy(vec_path_x.at(i*(numj-1)+j).begin(),   vec_path_x.at(i*(numj-1)+j).end(),   back_inserter(pathx1) );
            if (i > 0)      copy(vec_path_y.at(j*(numi-1)+i-1).begin(), vec_path_y.at(j*(numi-1)+i-1).end(), back_inserter(pathy0) );
            if (i < numi-1) copy(vec_path_y.at(j*(numi-1)+i).begin(),   vec_path_y.at(j*(numi-1)+i).end(),   back_inserter(pathy1) );
            
            
            // get mask
            cv::Mat mask(tile.size(), CV_8U, cv::Scalar(255));
            for (int y=0; y<mask.rows; y++) {
                for (int x=0; x<mask.cols; x++) {
                    if (j > 0)      if (x < pathx0.at(y).x) mask.at<uchar>(y,x) = 0;
                    if (j < numj-1) if (x > pathx1.at(y).x + mask.cols - overlap) mask.at<uchar>(y,x) = 0;
                    if (i > 0)      if (y < pathy0.at(x).y) mask.at<uchar>(y,x) = 0;
                    if (i < numi-1) if (y > pathy1.at(x).y + mask.rows - overlap) mask.at<uchar>(y,x) = 0;
                }
            }
            
            cv::Rect rect((tile.cols-overlap)*j,(tile.rows-overlap)*i, tile.cols, tile.rows);
            cv::Mat roi_dst = dst(rect);
            cv::Mat roi_dst0 = dst0(rect);
            cv::Mat roi_dstp = dst_poisson(rect);
            if ((i+j)%2 == 0) {
                tile.copyTo(roi_dst, mask);
                tile.copyTo(roi_dst0, mask);
                my::SeamlessClone(tile, roi_dstp, mask, cv::Point(0,0));
            } else {
                tile2.copyTo(roi_dst, mask);
                tile2.copyTo(roi_dst0, mask);
                my::SeamlessClone(tile2, roi_dstp, mask, cv::Point(0,0));
            }
            
            //cv::imshow("mask"+to_string(i)+to_string(j), mask);
            cv::imshow("mask", mask);
            cv::imshow("quilting", dst0);
            cv::imshow("quilting wiht line", dst);
            cv::imshow("poisson blending with quilting", dst_poisson);
            cv::waitKey();
        }
    }
    
    return 0;
}