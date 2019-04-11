#include <data/imgdata.h>
#include <tools/random.h>

ImgData::ImgData(std::string ParamDir)
{
  Input_param = ParamDir ;
  filesnumber = 0;
}
ImgData::~ImgData()
{

}

void ImgData::GetImgPath(std::string Input_param_dir)
{
  struct dirent *ptr;
  DIR *dir;
  dir = opendir(Input_param_dir.data());
  //20X*
  int count=0;
  std::vector<std::string> temp_imagePath;

  while ((ptr=readdir(dir))!=NULL)
  {
    if(ptr->d_name[0] == '.')
      continue;
    if(int(ptr->d_type)==4)
    {
      std::string dir_name = Input_param_dir;
      std::string files_name = ptr->d_name;
      std::string search_path = dir_name+files_name+"/";
      GetImgPath(search_path);
    }
    else {
      std::string full_path = Input_param_dir+ptr->d_name;
      std::string ImgName = ptr->d_name;
      if(ImgName.find("png"))
      {
        count++;
        temp_imagePath.push_back(full_path);
        if(count==600)
        {
          imagedata.push_back(temp_imagePath);
          temp_imagePath.clear();
          count=0;
        }

        ImgPathList.push_back(full_path);
        ImgClassinfo.push_back(SplitString(full_path,"/"));
      }
    }
  }
  closedir(dir);
}
std::multimap<std::string, std::vector<std::string> > ImgData::GetClassinfo()
{
  this->GetImgPath(Input_param);

  for(int it=0;it<ImgPathList.size();it++)
  {
    ImgPathToClassInfo.insert(std::pair<std::string, std::vector<std::string> >(ImgPathList[it],ImgClassinfo[it]));
  }

  return ImgPathToClassInfo;
}

std::vector<std::string> ImgData::SplitString(const std::string &s, const std::string &c)
{
  std::vector<std::string> v;
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));


  std::vector<std::string>::iterator start = v.end()-4;
  std::vector<std::string>::iterator end   = v.end();

  std::vector<std::string> Getlast4File(start,end);
  return Getlast4File;
}

std::pair<Mat, std::vector<std::string> > ImgData::RandomSomeoneImgData()
{
  this->GetImgPath(Input_param);

  std::pair<int,int> range;
  range.first = 0;
  range.second = ImgPathToClassInfo.size();
  int random = Tools::RandomIntTool(range);
  cv::Mat RandomImgdata = cv::imread(ImgPathList[random],CV_LOAD_IMAGE_COLOR);
  //norm
  RandomImgdata.convertTo(RandomImgdata,CV_32FC3,1/255.0);

  std::pair<Mat, std::vector<std::string> > result;
  result.first  = RandomImgdata;
  result.second = ImgClassinfo[random];

  return result;

}

cv::Mat ImgData::rotate_arbitrarily_angle(Mat &src,int Inputangle)
{
  double angle;
  switch (Inputangle) {
  case 0:
    angle = 0;
    break;
  case 1:
    angle = 90.0;
    break;
  case 2:
    angle = 180.0;
    break;
  case 3:
    angle = 270.0;
    break;
  default:
    angle = 0;
  }
  std::cout<<"angle :"<<angle<<std::endl;

    Mat dst;
    double radian = (double) (angle /180.0 * CV_PI);

    //填充图像
    int maxBorder =(int) (max(src.cols, src.rows)* 1.414 ); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols)/2;
    int dy = (maxBorder - src.rows)/2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_REPLICATE,cv::Scalar(255, 255, 255));

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

    return dst;
}
std::pair<cv::Mat, std::vector<std::string> > ImgData::rotate_image()
{
  //random
  std::pair<double,double> rotate_angle;
  rotate_angle.first=0;
  rotate_angle.second = 360;
  std::pair<cv::Mat,std::vector<std::string> > acceptRandom ;
  acceptRandom = RandomSomeoneImgData();
  Mat dst = rotate_arbitrarily_angle(acceptRandom.first,Tools::RandomDoubleTool(rotate_angle));

  std::pair<cv::Mat, std::vector<std::string> > result;
  result.first = dst;
  result.second = acceptRandom.second;

  return result;
}
cv::Mat ImgData::rotate(int rotate ,cv::Mat src)
{
  cv::Mat dts;
  if(rotate==0)
  {
    return src;
  }
  else if (rotate==1) {
      // 矩阵转置
      transpose(src, dts);
      flip(src, dts, 1);
      return dts;
  }
  else if (rotate==2) {
      flip(src, dts, 0);
      flip(src, dts, 1);
      return dts;
  }
  else if (rotate==3) {
    transpose(src, dts);
      flip(src, dts, 0);
      return dts;
  }
}
tensorflow::Tensor ImgData::OutputImageData(std::pair<int, std::string> imageinfo)
{
 // cout<<"*************"<<imageinfo.second<<"*************************"<<endl;
  //read image
  cv::Mat Imgdata = cv::imread(imageinfo.second,CV_LOAD_IMAGE_COLOR);
  //norm
  cv::Mat NormImgData;
  Imgdata.convertTo(NormImgData,CV_32FC3,1/255.0);
  //rotate
  cv::Mat RateImgData =rotate_arbitrarily_angle(NormImgData,imageinfo.first);
  //resize
  int height = 46;
  int width = 46;
  cv::Mat dts;
  cv::resize(RateImgData,dts,cv::Size(height,width));
  //std::cout<<"*******reader ok**********"<<std::endl;
  //toTensor
  float *image_float_data =(float*)dts.data;

  tensorflow::Tensor image_tensor;

  tensorflow::TensorShape image_shape = tensorflow::TensorShape{1, dts.rows, dts.cols, dts.channels()};
  image_tensor = tensorflow::Tensor(tensorflow::DT_FLOAT, image_shape);
  std::copy_n(image_float_data, image_shape.num_elements(), image_tensor.flat<float>().data());

  //std::cout<<"*******reader ok**********"<<std::endl;

  return image_tensor;
}

void ImgData::fixedDataset(std::string fixedDatasetDir)
{
  struct dirent *ptr;
  DIR *dir;
  dir = opendir(fixedDatasetDir.data());

  while ((ptr=readdir(dir))!=NULL)
  {
    if(ptr->d_name[0] == '.')
      continue;
    if(int(ptr->d_type)==4)
    {
      filesnumber++;
      std::string dir_name = fixedDatasetDir;
      std::string files_name = ptr->d_name;
      std::string search_path = dir_name+files_name+"/";
      std::cout<<filesnumber<<std::endl;
      fixedDataset(search_path);
      imagedata.push_back(temp_imagePath);
      temp_imagePath.clear();
    }
    else {
      std::string full_path = fixedDatasetDir+ptr->d_name;
      std::string ImgName = ptr->d_name;
      if(ImgName.find("png"))
      {
        temp_imagePath.push_back(full_path);
      }
    }
  }
  closedir(dir);
}
