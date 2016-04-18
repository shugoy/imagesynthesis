//
//  video.cpp
//  PoissonBlending
//
//  Created by 山口 周悟 on 2015/04/22.
//  Copyright (c) 2015年 山口 周悟. All rights reserved.
//

#include "video.h"

vector<Mat> video::videocapture(const char* videoname){
    // （1）動画ファイルを開くための準備を行う
    cv::VideoCapture cap(videoname);
    
    // （2）動画ファイルが正しく開けているかをチェックする（正しく開けていなければエラー終了する）
    if(!cap.isOpened())
        exit(0);
    
    // 画像表示用のウィンドウを生成する
    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    
    // 画像データを格納するための変数を宣言する
    vector<Mat> video(cap.get(CV_CAP_PROP_FRAME_COUNT));
    
    //cout << cap.get(CV_CAP_PROP_FRAME_COUNT) << endl;
    
    
    int i=0;
    while (1)
    {
        cv::Mat frame;
        // （3）動画ファイルから1フレーム分の画像データを取得して、変数frameに格納する
        cap >> frame;
        
        // 画像データ取得に失敗したらループを抜ける
        if (frame.empty()) break;
        
        // 取得した画像データをウィンドウ表示する
        video[i] = frame.clone();
        //cv::imshow("image", video[i]);
        //if(cv::waitKey(30) >= 0) break;
        i++;
    }
    //    for(int i=0; i<video.size(); i++)
    //    {
    //        // 取得した画像データをウィンドウ表示する
    //        //video.push_back(frame);
    //        cv::imshow("image", video[i]);
    //        cout << i << endl;
    //        waitKey(30);
    //    }
    cv::destroyWindow("image");
    
    return video;
}


void video::videopreview(vector<Mat> &video, int duration, int fps) {
    cv::namedWindow("video_preview");
    cout << "no of frames " << video.size() << endl;
    float spf = 1000.0 / fps;
    int i=0;
    while (1) {
        cout << "i " << i << endl;
        cv::imshow("video_preview", video[i]);
        
        int c;
        c = cv::waitKey(spf);
        //cout << "aho" << i << endl;
        if (c == 27) break; //esc
        else if (c == 0x73)
        { // 's'キー入力
            printf( "'s'キーが押された\n" );
            cv::waitKey(); //一時停止
        }
        //
        i++;
        if (i >= duration ) i=0;
        
    }
    cv::destroyWindow("video_preview");
}


void video::videowriter(string filename, vector<Mat> &video, int duration, int fps) {
    // 幅320px、高さ240px、3チャンネルのインスタンスを生成する
    //cv::Mat img(cv::Size(320, 240), CV_8UC3, cv::Scalar(0, 0, 255));
    
    // （1）動画ファイルを書き出すの準備を行う
    //cv::VideoWriter writer("videofile.avi", CV_FOURCC_MACRO('X', 'V', 'I', 'D'), 30.0, cv::Size(video[0].cols, video[0].rows), true);
    cv::VideoWriter writer(filename+".mov", CV_FOURCC_MACRO('m', 'p', '4', 'v'), fps, cv::Size(video[0].cols, video[0].rows), true);
    
    // （2）動画ファイル書き出しの準備に成功したかチェックする（失敗していればエラー終了する）
    if(!writer.isOpened())
        exit(0);
    
    for(int i = 0; i < duration; i++)
    {
        // （3）画像データを動画ファイルに書き出す
        writer << video[i];
    }
    
    cout << "video has written!" << endl;
}
