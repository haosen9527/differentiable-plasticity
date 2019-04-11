#include <tensorflow/core/framework/tensor.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "tools/random.h"
#include "data/imgdata.h"
#include "plastic/config.h"
#include "plastic/plastictrain.h"

std::vector<Tensor> generateInputsLabelsAndTarget(const defaultParams &params, ImgData &imagedata, bool is_test)
{
    int nbsteps = params.nbshots * ( (params.prestime+params.ipd) * params.nbclesses ) +params.prestimetest;

    Tensor inputT(DT_FLOAT,tensorflow::TensorShape({nbsteps,1,1,params.ImgSize,params.ImgSize}));
    Tensor labelT(DT_FLOAT,tensorflow::TensorShape({nbsteps,1,params.nbclesses}));
    inputT.flat<float>().setZero(); labelT.flat<float>().setZero();

    std::vector<int> cats;
    if(params.isFixed==false)
    {
      if(is_test)
      {
        //cats = Tools::vectorShuffleInt(Tools::RandomSeed((imagedata.size()-params.NBTESTCLASSES),imagedata.size(),10));
        cats =Tools::vectorNbclasses(Tools::vectorShuffleInt(Tools::arange((imagedata.imagedata.size()-params.NBTESTCLASSES),imagedata.imagedata.size())),params.nbclesses);
      }
      else
      {
        cats =Tools::vectorNbclasses(Tools::vectorShuffleInt(Tools::arange(0,imagedata.imagedata.size())),params.nbclesses);
      }
    }
    else
    {
      //imagedata.fixedDate;
      cats = Tools::vectorNbclasses(Tools::arange(0,imagedata.imagedata.size()),params.nbclesses);
    }
    //cats = np.random.permutation(cats)
    cats = Tools::vectorShuffleInt(cats);
    //std::cout<<"cats_size"<<cats.size()<<std::endl;
    //rots = np.random.randint(4, size=len(imagedata))
    std::vector<int> rotsList;
    //std::srand(params.rngseed);
    std::cout<<"imagedata.imagedata.size:"<<imagedata.imagedata.size()<<std::endl;
    for(int i=0;i<imagedata.imagedata.size();i++)
    {
      rotsList.push_back( Tools::RandomSeed(0,4));//[0,4)   [0,1,2,3]
    }
    //testcat = random.choice(cats) # select the class on which we'll test in this episode
    //int testcat = Tools::RandomSeed(0,cats.size());
    int testcat = Tools::RandomSeed(0,3);
    //cout
    for(int test=0;test<params.nbclesses;test++)
    {
      std::cout<<" : "<<cats[test]<<" ";
    }
    std::cout<<std::endl;
    std::pair<int,std::string> imginfo;//rots,imgPath

    //# Inserting the character images and labels in the input tensor at the proper places
    int location = 0;
    for(int nc=0;nc<params.nbshots;nc++)
    {
      std::vector<int> shuffleCats = Tools::vectorShuffleInt(cats);
      //cout
      std::cout<<"shuffleCats:"<<std::endl;
      for(int test=0;test<params.nbclesses;test++)
      {
        std::cout<<" : "<<shuffleCats[test]<<" ";
      }
      std::cout<<std::endl;
      for(int ii=0;ii<shuffleCats.size();ii++)
      {
        int p = Tools::RandomSeed(0,imagedata.imagedata[shuffleCats[ii]].size());
        //imagedata[p]
         std::string imgPath = imagedata.imagedata[shuffleCats[ii]][p];
         //imginfo.first=rotsList[((ii*20)+p+1)];
         imginfo.first=rotsList[shuffleCats[ii]];
         imginfo.second = imgPath;
         tensorflow::Tensor InputImageData =imagedata.OutputImageData(imginfo);
         //InputImageData
         for(int nn=0;nn<params.prestime;nn++)
         {
           //labelT(location,0,);
           int index=0;
           auto templabelT = labelT.tensor<float,3>();
           for(int xx=0;xx<params.nbclesses;xx++)
           {
             //std::cout<<"xx:"<<shuffleCats[ii]<<std::endl;
             if(cats[xx]==shuffleCats[ii])
             {
               std::cout<<"xx:"<<xx<<std::endl;
              index = xx;
             }
           }
           for(int index_x=0;index_x<params.nbclesses;index_x++)
           {
             if(index == index_x)
             {
               //std::cout<<"index:"<<index<<std::endl;
              templabelT(location,0,index_x) = 1;
             }
             else {
              templabelT(location,0,index_x) = 0;
             }
           }
           //cout
           for(int test=0;test<params.nbclesses;test++)
           {
             std::cout<<" : "<<templabelT(location,0,test);
           }
           std::cout<<std::endl;
           auto temp = InputImageData.tensor<float,4>();
           auto inputT_temp =inputT.tensor<float,5>();
           for(int i=0;i<params.ImgSize;i++)
           {
             for(int j=0;j<params.ImgSize;j++)
             {
               inputT_temp(location,0,0,i,j) = temp(0,i,j,0);
             }
           }
           location+=1;
         }
         location+=params.ipd;
      }
    }

    //std::cout<<"-------for this------ location:"<<location<<std::endl;
    // Inserting the test character
    int pRandom = Tools::RandomSeed(0,imagedata.imagedata[cats[testcat]].size());
    std::string p_test =imagedata.imagedata[cats[testcat]][pRandom];
    std::pair<int, std::string> testImg;
    testImg.first  = rotsList[cats[testcat]];
    testImg.second = p_test;
    //std::cout<<"pRandom:"<<pRandom<<"p_test:"<<p_test<<endl;
    tensorflow::Tensor testImageData = imagedata.OutputImageData(testImg);
    //std::cout<<"-------reader over------"<<std::endl;
    for(int nn=0;nn<params.prestimetest;nn++)
    {
      auto temp = testImageData.tensor<float,4>();
      auto inputT_temp_2 =inputT.tensor<float,5>();
      for(int m=0;m<params.ImgSize;m++)
      {
        for(int n=0;n<params.ImgSize;n++)
        {
          inputT_temp_2(location,0,0,m,n) = temp(0,m,n,0);
        }
      }
      location+=1;
    }


    //Generating the test label
    //params.nbclesses
    Tensor testlabel(DT_FLOAT,tensorflow::TensorShape({params.nbclesses}));
    auto temp_testlabel = testlabel.vec<float>();
    for(int x=0;x<params.nbclesses;x++)
    {
      if(x==testcat)
      {
        std::cout<<"x:"<<x<<std::endl;
        temp_testlabel(x) = 1.0f;
      }
      else
      {
        temp_testlabel(x) = 0.0f;
      }
    }
    //cout
    for(int test=0;test<params.nbclesses;test++)
    {
      std::cout<<" : "<<temp_testlabel(test);
    }
    std::cout<<std::endl;

    if(location == nbsteps)
    {
      std::cout<<"ok!"<<std::endl;
    }
    else
    {
      std::cout<<"####ERROR#####"<<std::endl;
    }

    //vector<Tensor> labels;
    //labels.push_back(inputT);
    //labels.push_back(labelT);
    //labels.push_back(testlabel);

    return {inputT,labelT,testlabel};
    //return labels;
}
