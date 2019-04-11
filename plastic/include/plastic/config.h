#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <string>

struct defaultParams
{
  enum activ
  {
    tanh,
    selu
  }Activ = tanh;
  enum relu
  {
    oja,
    hebb
  }Relu = oja;
  enum alpha
  {
    free,
    yoked
  }Alpha = free;
  long int steplr = 1500000;
  int nbclesses = 5;
  float gamma = 0.666;
  bool flare = false;
  int nbshots = 3;
  int prestime = 1;
  enum nbf
  {
    nbf64=64,
    nbf128=128
  }Nbf = nbf64;
  int prestimetest = 1;
  int ipd = 0;
  int ImgSize = 46;
  long int nbiter = 6000000;
  long int nbiter_test = 3000;
  float lr = 0.00006;
  int save_every = 10000;
  int rngseed = 0;
  int NBTESTCLASSES = 16;
  bool isFixed = true; //true==fixed or false==random
};

#endif // CONFIG_H
