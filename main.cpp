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

    //����IplImageָ��
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

    //��������
    cvNamedWindow("video", 1);
    cvNamedWindow("background",1);
    cvNamedWindow("foreground",1);
    //ʹ������������
    cvMoveWindow("video", 30, 0);
    cvMoveWindow("background", 360, 0);
    cvMoveWindow("foreground", 690, 0);


    if( !(pCapture = cvCaptureFromCAM(0)))
    {
        fprintf(stderr, "Can not open camera.\n");
        return -2;
    }

    //����Ƶ�ļ�
//    if(argc == 2)
//    {
//        if( !(pCapture = cvCaptureFromFile(argv[1])))
//        {
//            fprintf(stderr, "Can not open video file %s\n", argv[1]);
//            return -2;
//        }
//    }

    //pCapture = cvCaptureFromFile("F:\\1.mp4");
    //��֡��ȡ��Ƶ
    BOOLEAN init = false;

    while(pFrame = cvQueryFrame( pCapture ))
    {
        nFrmNum++;
        if (init)
            times ++;

        //����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��
        if(nFrmNum == 1)
        {
            pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
            pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);

            pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
            pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
            pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

            //ת���ɵ�ͨ��ͼ���ٴ���
            cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
            cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

            cvConvert(pFrImg, pFrameMat);
            cvConvert(pFrImg, pFrMat);
            cvConvert(pFrImg, pBkMat);

            //��תͼ��
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
            //��˹�˲��ȣ���ƽ��ͼ��
            cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);

            //��ǰ֡������ͼ���
            cvAbsDiff(pFrameMat, pBkMat, pFrMat);

            //��ֵ��ǰ��ͼ
            cvThreshold(pFrMat, pFrImg, 60, 255.0, CV_THRESH_BINARY);

            //������̬ѧ�˲���ȥ������
            for(int i = 0; i < 10; i ++)
            {
                cvErode(pFrImg, pFrImg, 0, 1);
                cvDilate(pFrImg, pFrImg, 0, 1);
            }

            //���±���
            cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
            //������ת��Ϊͼ���ʽ��������ʾ
            cvConvert(pBkMat, pBkImg);

            //��ʾͼ��
            cvShowImage("video", pFrame);
            cvShowImage("background", pBkImg);
            cvShowImage("foreground", pFrImg);

            //����а����¼���������ѭ��
            //�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ
            //�ȴ�ʱ����Ը���CPU�ٶȵ���
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

    //���ٴ���
    cvDestroyWindow("video");
    cvDestroyWindow("background");
    cvDestroyWindow("foreground");

    //�ͷ�ͼ��;���
    cvReleaseImage(&pFrImg);
    cvReleaseImage(&pBkImg);

    cvReleaseMat(&pFrameMat);
    cvReleaseMat(&pFrMat);
    cvReleaseMat(&pBkMat);

    cvReleaseCapture(&pCapture);

    return 0;
}
