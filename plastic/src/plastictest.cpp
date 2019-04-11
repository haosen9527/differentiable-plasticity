#include "plastic/plastictrain.h"

using namespace tensorflow;

class plasticTest:public PlasticTrain
{
public:
  plasticTest(const tensorflow::Scope &scope, string data_sets):PlasticTrain(scope, data_sets)
  {

  }
  ~plasticTest()
  {

  }

public:
  plasticTest *testSettingParam()
  {
    srand(params.rngseed);
    return this;
  }
  plasticTest *restoreCkpt(std::string filePattern)
  {
    std::vector<std::string> temp_arr_slice(11);
    std::vector<Output> tensor_list;

    /*tensor_list.push_back(net.alpha);
    tensor_list.push_back(net.cv1.bias);
    tensor_list.push_back(net.cv1.weight);
    tensor_list.push_back(net.cv2.bias);
    tensor_list.push_back(net.cv2.weight);
    tensor_list.push_back(net.cv3.bias);
    tensor_list.push_back(net.cv3.weight);
    tensor_list.push_back(net.cv4.bias);
    tensor_list.push_back(net.cv4.weight);
    tensor_list.push_back(net.eta);
    tensor_list.push_back(net.w);
    temp_arr_slice[0] = "alpha";
    temp_arr_slice[1] = "cv1.bias";
    temp_arr_slice[2] = "cv1.weight";
    temp_arr_slice[3] = "cv2.bias";
    temp_arr_slice[4] = "cv2.weight";
    temp_arr_slice[5] = "cv3.bias";
    temp_arr_slice[6] = "cv3.weight";
    temp_arr_slice[7] = "cv4.bias";
    temp_arr_slice[8] = "cv4.weight";
    temp_arr_slice[9] = "eta";
    temp_arr_slice[10] = "w";*/

    tensor_list.push_back(net.cv1.weight);
    tensor_list.push_back(net.cv2.weight);
    tensor_list.push_back(net.cv3.weight);
    tensor_list.push_back(net.cv4.weight);
    tensor_list.push_back(net.cv1.bias);
    tensor_list.push_back(net.cv2.bias);
    tensor_list.push_back(net.cv3.bias);
    tensor_list.push_back(net.cv4.bias);
    tensor_list.push_back(net.w);
    tensor_list.push_back(net.alpha);
    tensor_list.push_back(net.eta);
    temp_arr_slice[0] = "conv1/weight";
    temp_arr_slice[1] = "conv2/weight";
    temp_arr_slice[2] = "conv3/weight";
    temp_arr_slice[3] = "conv4/weight";
    temp_arr_slice[4] = "conv1/bias";
    temp_arr_slice[5] = "conv2/bias";
    temp_arr_slice[6] = "conv3/bias";
    temp_arr_slice[7] = "conv4/bias";
    temp_arr_slice[8] = "hyper/w";
    temp_arr_slice[9] = "hyper/alpha";
    temp_arr_slice[10] = "hyper/eta";

    for(int i=0;i<11;i++)
    {
      auto tensor_data = tensorflow::ops::Restore(scope, filePattern, temp_arr_slice[i], DT_FLOAT);
      TF_CHECK_OK(session.Run({Assign(scope, tensor_list[i], tensor_data)}, NULL));
      /*vector<Tensor> outputs;
      TF_CHECK_OK(session.Run({tensor_data}, &outputs));
      std::cout << outputs[0].shape().DebugString() << std::endl;
      std::cout << std::scientific << std::setprecision(7) << outputs[0].flat<float>()(0) << std::endl;
      std::cout << std::scientific << std::setprecision(7) << outputs[0].flat<float>()(1) << std::endl;
      std::cout << std::scientific << std::setprecision(7) << outputs[0].flat<float>()(2) << std::endl;
      std::cout << std::scientific << std::setprecision(7) << outputs[0].flat<float>()(3) << std::endl;
      std::getchar();*/
    }
    return this;
  }
  plasticTest *testEpisodes()
  {
    std::cout<<"Starting episodes..."<<std::endl;
    int nbsteps = params.nbshots * ((params.prestime + params.ipd) * params.nbclesses) + params.prestimetest;
    Tensor zero_tensor(DT_FLOAT, {(int)params.Nbf, params.nbclesses});
    zero_tensor.flat<float>().setZero();
    std::vector<Tensor> outputs;
    Output inputs = Placeholder(scope, DT_FLOAT, Placeholder::Shape({nbsteps, 1, 1, params.ImgSize, params.ImgSize}));
    Output labels = Placeholder(scope, DT_FLOAT, Placeholder::Shape({nbsteps, 1, params.nbclesses}));
    Output target = Placeholder(scope, DT_FLOAT, Placeholder::Shape({params.nbclesses}));
    std::unordered_map<Output, Input::Initializer, OutputHash> feed_map = {{inputs, Tensor(DT_FLOAT, {nbsteps, 1, 1, params.ImgSize, params.ImgSize})}, {labels, Tensor(DT_FLOAT, {nbsteps, 1, params.nbclesses})}, {target, Tensor(DT_FLOAT, {params.nbclesses})}};
    Output y, hebb = Const(scope, zero_tensor);
    for(int numstep = 0; numstep < nbsteps; numstep++)
    {
      Output inputs_chip = Reshape(scope, StridedSlice(scope, inputs, {numstep, 0, 0, 0, 0}, {numstep + 1, 1, 1, params.ImgSize, params.ImgSize}, {1, 1, 1, 1, 1}), {1, 1, params.ImgSize, params.ImgSize});
      Output labels_chip = Reshape(scope, StridedSlice(scope, labels, {numstep, 0, 0}, {numstep + 1, 1, params.nbclesses}, {1, 1, 1}), {1, params.nbclesses});
      y = net.forward(inputs_chip, labels_chip, params.Nbf, params.nbclesses, hebb, feed_map);
    }
    y = Reshape(scope, StridedSlice(scope, y, {0, 0}, {1, params.nbclesses}, {1, 1}), {params.nbclesses});
    auto ce1 = Negate(scope, Multiply(scope, target, Log(scope, y)));
    auto ce2 = Negate(scope, Multiply(scope, Sub(scope, 1.0f, target), Log(scope, Sub(scope, 1.0f, y))));
    auto loss = Mean(scope, Add(scope, ce1, ce2), {0});
    //this->restoreCkpt("/home/micros/YQ_hu/txtminiImagenet5_3shot.ckpt");
    this->restoreCkpt("/home/micros/catkin_ws/src/differentiable_plasticity/data/snapshot2_1000000.ckpt");
    int mistake=0;
    for(int numiter = 0; numiter < params.nbiter_test; numiter++)
    {
      std::vector<Tensor> tensor_list = generateInputsLabelsAndTarget(params, imgdata, true);
      //TF_CHECK_OK(session.Run({{inputs, tensor_list[0]}, {labels, tensor_list[1]}, {target, tensor_list[2]}, {learning_rate, decaying_rate}}, train_ops, NULL));
      TF_CHECK_OK(session.Run({{inputs, tensor_list[0]}, {labels, tensor_list[1]}, {target, tensor_list[2]}}, {loss}, &outputs));

      auto target_temp = tensor_list[2].vec<float>();
      float max = 0;
      int index_target = 0;
      int index_y=0;
      for (int i=0;i<params.nbclesses;i++)
      {
        if(target_temp(i)>max)
        {
          max = target_temp(i);
          index_target = i;
        }
      }
      float max_y=0;
      std::vector<Tensor> outputs;
      TF_CHECK_OK(session.Run({{inputs, tensor_list[0]}, {labels, tensor_list[1]}, {target, tensor_list[2]}}, {y}, &outputs));

      auto y_temp = outputs[0].vec<float>();
      for(int j=0;j<params.nbclesses;j++)
      {
        std::cout<<"y_temp:"<<y_temp(j)<<std::endl;
        if(y_temp(j)>max_y)
        {
          max_y=y_temp(j);
          index_y = j;
          std::cout<<"max:"<<max_y<<std::endl;
        }
      }
      if(index_target != index_y)
      {
        mistake++;
      }
      std::cout<<"mistake:"<<mistake<<std::endl;
      std::cout<<"index_y:"<<index_y<<std::endl;
      std::cout<<"index_target:"<<index_target<<std::endl;
    }
    double accuracy = (double)mistake/(double)params.nbiter_test;

    //std::cout<<"Mistake:"<<(double)(mistake/params.nbiter_test)<<std::endl;
    std::cout<<"Accuracy:"<<(1-accuracy)<<std::endl;
    return this;
  }
};

int main(int argc, char *argv[])
{
  plasticTest test(Scope::NewRootScope(), "src/differentiable_plasticity/data/miniimagenet/");
  test.testSettingParam();
  test.LoadOmniglotData();
  test.testEpisodes();
  return 0;
}
