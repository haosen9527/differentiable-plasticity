#ifndef PLASTICTRAIN_H
#define PLASTICTRAIN_H

#include <iostream>
#include <iomanip>

#include <tensorflow/core/kernels/training_ops.h>
#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/ops/standard_ops.h>

#include "data/imgdata.h"
#include "plastic/config.h"
#include "network/omniglot.h"

using namespace tensorflow::ops;

std::vector<Tensor> generateInputsLabelsAndTarget(const defaultParams &params, ImgData &imagedata, bool is_test);

class PlasticTrain
{
public:
  PlasticTrain(const tensorflow::Scope &scope, std::string data_sets);
  ~PlasticTrain();
public:
  //Setting ParamSetting...
  PlasticTrain *ParamSetting();
  //Loading Omniglot data...
  PlasticTrain *LoadOmniglotData();

  PlasticTrain *load(std::string filePattern);

  PlasticTrain *snapshot(int iter);
  //main loop
  PlasticTrain *Episodes(int start);

public:
  const defaultParams params;
  tensorflow::Scope scope;
  tensorflow::ClientSession session;
  ImgData imgdata;
  network net;
};

#endif // PLASTICTRAIN_H
