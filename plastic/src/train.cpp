#include "plastic/plastictrain.h"

int main(int argc, char *argv[])
{
  PlasticTrain train(tensorflow::Scope::NewRootScope(), "src/differentiable_plasticity/data/miniimagenet/");
  train.ParamSetting();
  train.LoadOmniglotData();
  train.Episodes(0);
  return 0;
}
