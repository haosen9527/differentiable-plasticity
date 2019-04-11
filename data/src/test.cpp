#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//
void rotate_arbitrarily_angle(Mat &src,Mat &dst,double angle)
{
    double radian = (double) (angle /180.0 * CV_PI);

    //填充图像
    int maxBorder =(int) (max(src.cols, src.rows)* 1.414 ); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols)/2;
    int dy = (maxBorder - src.rows)/2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);

    //旋转
    Point2f center( (float)(dst.cols/2) , (float) (dst.rows/2));
    Mat affine_matrix = getRotationMatrix2D( center, angle, 1.0 );//求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());

    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize( (int)(src.cols * cosVal +src.rows * sinVal),
                     (int)(src.cols * sinVal + src.rows * cosVal) );

    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst,rect);
}

cv::Mat rotate_arbitrarily_angle1(cv::Mat matSrc, double angle, bool direction,int height,int width) {
    double theta = angle * CV_PI / 180.0;
    int nRowsSrc = matSrc.rows;
    int nColsSrc = matSrc.cols; // 如果是顺时针旋转
    if (!direction) theta = 2 * CV_PI - theta; // 全部以逆时针旋转来计算
    // 逆时针旋转矩阵
    float matRotate[3][3]{ {
                                   std::cos(theta), -std::sin(theta), 0},
                           {std::sin(theta), std::cos(theta), 0 },
                           {0, 0, 1} };

    float pt[3][2]{
            { 0, nRowsSrc },
            {nColsSrc, nRowsSrc},
            {nColsSrc, 0} };

    for (int i = 0; i < 3; i++) {
        float x = pt[i][0] * matRotate[0][0] + pt[i][1] * matRotate[1][0];
        float y = pt[i][0] * matRotate[0][1] + pt[i][1] * matRotate[1][1];
        pt[i][0] = x; pt[i][1] = y;
    }
    // 计算出旋转后图像的极值点和尺寸
    float fMin_x = std::min(std::min(std::min(pt[0][0], pt[1][0]), pt[2][0]), (float)0.0);
    float fMin_y = std::min(std::min(std::min(pt[0][1], pt[1][1]), pt[2][1]), (float)0.0);
    float fMax_x = std::max(std::max(std::max(pt[0][0], pt[1][0]), pt[2][0]), (float)0.0);
    float fMax_y = std::max(std::max(std::max(pt[0][1], pt[1][1]), pt[2][1]), (float)0.0);
    int nRows = cvRound(fMax_y - fMin_y + 0.5) + 1;
    int nCols = cvRound(fMax_x - fMin_x + 0.5) + 1;
    int nMin_x = cvRound(fMin_x + 0.5);
    int nMin_y = cvRound(fMin_y + 0.5);
    // 拷贝输出图像
    cv::Mat matRet(nRows, nCols, matSrc.type(), cv::Scalar(0));
    for (int j = 0; j < nRows; j++) {
        for (int i = 0; i < nCols; i++) {
            // 计算出输出图像在原图像中的对应点的坐标，然后复制该坐标的灰度值
            // 因为是逆时针转换，所以这里映射到原图像的时候可以看成是，输出图像
            // 到顺时针旋转到原图像的，而顺时针旋转矩阵刚好是逆时针旋转矩阵的转置
            // 同时还要考虑到要把旋转后的图像的左上角移动到坐标原点。
            int x = (i + nMin_x) * matRotate[0][0] + (j + nMin_y) * matRotate[0][1];
            int y = (i + nMin_x) * matRotate[1][0] + (j + nMin_y) * matRotate[1][1];
            if (x >= 0 && x < nColsSrc && y >= 0 && y < nRowsSrc) {
                matRet.at<uchar>(j, i) = matSrc.at<uchar>(y, x);
            }
        }
    }
    if(direction== false){//当需要顺时针旋转回水平位置时
        int x = (matRet.cols -width) / 2;
        int y = (matRet.rows -height) / 2;

        //width和height是水平条件下图像的宽高
        cv::Rect rect(x, y, width, height);
        matRet = cv::Mat(matRet,rect);
    }
    return matRet;
}


int main(int argc, char *argv[])
{
  Mat orig;
  orig = imread("/home/micros/图片/image1.png");
  if (orig.empty())
    cout << "读入失败" << endl;
  //imshow("original", orig);
  vector<Mat>m;
  split(orig, m); //提取通道信息，但是当未把其他通道设置为0时显示为灰度图，但各通道信息不同
  vector<Mat>Rchannels, Gchannels, Bchannels;
  //提取通道信息，把orig的通道数据复制到channals
  split(orig, Rchannels);
  split(orig, Gchannels);
  split(orig, Bchannels);
  //将其他通道信息设置为0
  std::cout<<" channels:"<<orig.channels()<<std::endl;
  Rchannels[1] = 0;
  Rchannels[2] = 0;
  Gchannels[0] = 0;
  Gchannels[2] = 0;
  Bchannels[0] = 0;
  Bchannels[1] = 0;
  //合并通道信息
  merge(Rchannels, m[0]);
  merge(Gchannels, m[1]);
  merge(Bchannels, m[2]);
  //显示各通道信息
//  imshow("B",m[0]);
//  imshow("G",m[1]);
//  imshow("R",m[2]);
  //waitKey(0);



  // 0,90.180,270

  cv::Mat dst;
  //rotate_arbitrarily_angle(orig,dst,45.0);

  dst = rotate_arbitrarily_angle1(orig,90.0,false,105,105);
  cv::imshow("src",orig);
  cv::imshow("dst",dst);
  cv::waitKey(0);


  return 0;
}
