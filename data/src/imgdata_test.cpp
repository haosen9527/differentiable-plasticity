#include <iostream>
#include <data/imgdata.h>
#include <tools/random.h>

int main()
{
  ImgData imgdata("./src/differentiable_plasticity/data/data_sets/");
  std::multimap<std::string, std::vector<std::string> > ClassInfoImg;

  ClassInfoImg = imgdata.GetClassinfo();
  //std::cout<<imgdata.ImgPathToClassInfo.size()<<std::endl;

  std::multimap<std::string, std::vector<std::string> >::iterator it;
  for(it=imgdata.ImgPathToClassInfo.begin();it!=imgdata.ImgPathToClassInfo.end();it++)
  //for(auto it:imgdata.ImgPathToClassInfo)
  {/*
    cout<<"-------"<<it->second.size()<<endl
        <<it->second[0]<<endl//className
        <<it->second[1]<<endl//language
        <<it->second[2]<<endl//character
        <<it->second[3]<<endl;//imgName
        cout<<it->first<<endl;*/
  }
  std::cout<<imgdata.ImgClassinfo.size()<<std::endl;

  ///random.h test
  std::pair<int, int> range;
  range.first = 0;
  range.second = imgdata.ImgPathToClassInfo.size()-1;

  std::cout<<Tools::RandomIntTool(range)<<std::endl;

  //test pair<cv::mat,vector<string> >
  std::pair<cv::Mat, std::vector<std::string> > someone_img;
  someone_img = imgdata.RandomSomeoneImgData();
  //opencv show
  //imshow("RandomImgdata", someone_img.first);
  //waitKey(0);

  std::cout<<someone_img.second[0]<<std::endl
           <<someone_img.second[1]<<std::endl
           <<someone_img.second[2]<<std::endl
           <<someone_img.second[3]<<std::endl;
  std::cout<<someone_img.second.size()<<std::endl;

  std::pair<cv::Mat, std::vector<std::string> > someoneRotateImg;
  //someoneRotateImg = imgdata.rotate_image();

  //for(int i=0;i<20;i++)
  //{
  //  std::pair<int, std::string> some_img;
  //  some_img.first = 3;
  //  some_img.second = "src/differentiable_plasticity/data/data_sets/images_evaluation/Keble/character18/1263_03.png";
  //  tensorflow::Tensor InputImgData = imgdata.OutputImageData(some_img);
  //}

  imgdata.fixedDataset("/home/micros/catkin_ws/src/differentiable_plasticity/data/gazeboDataSet/train/");
  std::cout<<"size:"<<imgdata.filesnumber<<std::endl;
  std::cout<<"size:"<<imgdata.fixedDate.size()<<std::endl;

  for(int i=0;i<imgdata.fixedDate.size();i++)
  {
    std::cout<<"path:"<<imgdata.fixedDate[i].size()<<std::endl;
  }

  //opencv show
  //imshow("RandomRotateImgdata", someoneRotateImg.first);
  //waitKey(0);

  return 0;
}
