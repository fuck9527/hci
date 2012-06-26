#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <iostream>
#include <QApplication>
#include <QVector>
#include <cmath>

using namespace std;

int getxy( IplImage* p )
{
    IplImage *dst = cvCreateImage(cvSize(p->width, p->height),  IPL_DEPTH_8U,1);
    cvErode( p, dst, NULL, 20 );
    //cvSaveImage("test.jpg", dst, 0 );
    for ( int i = 0; i < dst->width; i ++ )
    {
//        cout << i << endl;
        for ( int j = 0; j < dst->height; j ++ )
            if ( cvGet2D( dst, j ,i ).val[0] == 255 )
                return i * 1000 + j;
    }

    return 0;
}


int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    //声明IplImage指针
    IplImage* pFrame = NULL;
    IplImage* pFrImg = NULL;
    IplImage* pBkImg = NULL;

    CvMat* pFrameMat = NULL;
    CvMat* pFrMat = NULL;
    CvMat* pBkMat = NULL;

    CvCapture* pCapture = NULL;

    QVector<int> imgs_pos;

    int times = 0;


    int nFrmNum = 0;

    //创建窗口
    cvNamedWindow("video", 1);
    cvNamedWindow("background",1);
    cvNamedWindow("foreground",1);
    //使窗口有序排列
    cvMoveWindow("video", 30, 0);
    cvMoveWindow("background", 360, 0);
    cvMoveWindow("foreground", 690, 0);


    if( !(pCapture = cvCaptureFromCAM(0)))
    {
        fprintf(stderr, "Can not open camera.\n");
        return -2;
    }

    //打开视频文件
//    if(argc == 2)
//    {
//        if( !(pCapture = cvCaptureFromFile(argv[1])))
//        {
//            fprintf(stderr, "Can not open video file %s\n", argv[1]);
//            return -2;
//        }
//    }

    //pCapture = cvCaptureFromFile("F:\\1.mp4");
    //逐帧读取视频
    BOOLEAN init = false;

    while(pFrame = cvQueryFrame( pCapture ))
    {
        nFrmNum++;
        if (init)
            times ++;

        //如果是第一帧，需要申请内存，并初始化
        if(nFrmNum == 1)
        {
            pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
            pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);

            pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
            pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
            pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

            //转化成单通道图像再处理
            cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
            cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

            cvConvert(pFrImg, pFrameMat);
            cvConvert(pFrImg, pFrMat);
            cvConvert(pFrImg, pBkMat);

            //翻转图像
            cvFlip(pFrame, pFrame, 1);
            cvFlip(pFrImg, pFrImg, 1);
            cvFlip(pBkImg, pBkImg, 1);
        }
        else
        {
            cvFlip(pFrame, pFrame, 1);
            cvFlip(pFrImg, pFrImg, 1);
            cvFlip(pBkImg, pBkImg, 1);

            cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
            cvConvert(pFrImg, pFrameMat);
            //高斯滤波先，以平滑图像
            cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);

            //当前帧跟背景图相减
            cvAbsDiff(pFrameMat, pBkMat, pFrMat);

            //二值化前景图
            cvThreshold(pFrMat, pFrImg, 60, 255.0, CV_THRESH_BINARY);

            //进行形态学滤波，去掉噪音
            for(int i = 0; i < 10; i ++)
            {
                cvErode(pFrImg, pFrImg, 0, 1);
                cvDilate(pFrImg, pFrImg, 0, 1);
            }

            //更新背景
            cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
            //将背景转化为图像格式，用以显示
            cvConvert(pBkMat, pBkImg);

            //显示图像
            cvShowImage("video", pFrame);
            cvShowImage("background", pBkImg);
            cvShowImage("foreground", pFrImg);

            //如果有按键事件，则跳出循环
            //此等待也为cvShowImage函数提供时间完成显示
            //等待时间可以根据CPU速度调整
            if( cvWaitKey(2) == 27 )
                break;
        }

        int count = 0;
        for ( int i = 0; i < pFrImg->width; i ++ )
        {
            for ( int j = 0; j < pFrImg->height; j ++ )
            {
                if ( cvGet2D( pFrImg, j, i ).val[0] == 255 )
                    count ++;
            }
        }
//        cout << count << endl;
//        cout << times << endl;
//        cout << endl;
        if ( count < pFrame->width * pFrame->height / 20  && count != 0 && init == false )
        {
            init = true;
            cout << "ready" << endl;
        }
        if ( times % 5 == 0 )
        {
//            cvSaveImage("test.jpg", pFrImg, 0 );
            imgs_pos.push_back( getxy( pFrImg ) );
//            cout<<"x" << getxy(pFrImg)/1000 <<endl;
            if ( imgs_pos.size() == 4)
            {
                imgs_pos.pop_front();
                BOOLEAN up = false;
                if ( abs( imgs_pos[0] / 1000 - imgs_pos[2] / 1000 ) + 50 <
                     abs( imgs_pos[0] % 1000 - imgs_pos[2] % 1000 ))
                    up = true;
                else
                    up = false;
                if (imgs_pos[0] < imgs_pos[1] && imgs_pos[1] < imgs_pos[2] && !up )
                {
                    imgs_pos.clear();
                    keybd_event(0x27, 0, 0, 0);
                    cout << "right" << endl;
                }
                else if (imgs_pos[0] > imgs_pos[1] && imgs_pos[1] > imgs_pos[2] && !up )
                {
                    imgs_pos.clear();
                    cout << "left" << endl;
                    keybd_event(0x25, 0, 0, 0);
                }
                else if (imgs_pos[0] % 1000 > imgs_pos[1] % 1000 && imgs_pos[1] % 1000 > imgs_pos[2] % 1000 && up )
                {
                    imgs_pos.clear();
                    cout << "up" << endl;
                    keybd_event(0x26, 0, 0, 0);
                }
                else if (imgs_pos[0] % 1000 < imgs_pos[1] % 1000 && imgs_pos[1] % 1000 < imgs_pos[2] % 1000 && !up )
                {
                    imgs_pos.clear();
                    cout << "down" << endl;
                    keybd_event(0x28, 0, 0, 0);
                }
            }

            if ( times % 300 == 0 )
                imgs_pos.clear();
            if ( times == 100000 )
                times = 0;
        }


    }

    //销毁窗口
    cvDestroyWindow("video");
    cvDestroyWindow("background");
    cvDestroyWindow("foreground");

    //释放图像和矩阵
    cvReleaseImage(&pFrImg);
    cvReleaseImage(&pBkImg);

    cvReleaseMat(&pFrameMat);
    cvReleaseMat(&pFrMat);
    cvReleaseMat(&pBkMat);

    cvReleaseCapture(&pCapture);

    return 0;
}
