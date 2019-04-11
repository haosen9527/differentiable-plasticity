#include "plastic/plastictrain.h"

PlasticTrain::PlasticTrain(const tensorflow::Scope &scope, std::string data_sets): params(), scope(scope), session(scope), imgdata(data_sets),
    net(params.Relu == defaultParams::oja, params.flare, params.Alpha == defaultParams::free, params.Activ == defaultParams::tanh, params.Activ != defaultParams::selu, params.Nbf, params.nbclesses, this->scope, session)
{

}
PlasticTrain::~PlasticTrain()
{

}
PlasticTrain *PlasticTrain::ParamSetting()
{
  srand(params.rngseed);
  return this;
}
PlasticTrain *PlasticTrain::LoadOmniglotData()
{
  //img Random
  //ImgData imgdata("src/differentiable_plasticity/data/data_sets/");
  if(params.isFixed==false)
  {
    std::multimap<std::string, std::vector<std::string> > full_img_info = imgdata.GetClassinfo();
  }
  else
  {
    imgdata.fixedDataset("/home/micros/catkin_ws/src/differentiable_plasticity/data/gazeboDataSet/train/");
  }

  return this;
}
PlasticTrain *PlasticTrain::load(std::string filePattern)
{
  std::vector<std::string> temp_arr_slice(11);
  std::vector<Output> tensor_list;
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
  for(int i=0; i<11; i++)
  {
    TF_CHECK_OK(session.Run({Assign(scope, tensor_list[i], tensorflow::ops::Restore(scope, filePattern, temp_arr_slice[i], DT_FLOAT))}, NULL));
  }
  return this;
}
PlasticTrain *PlasticTrain::snapshot(int iter)
{
  const std::string filename = std::string("src/differentiable_plasticity/data/snapshot_") + std::to_string(iter + 1) + std::string(".ckpt");
  Tensor arr_slice_tensor(tensorflow::DT_STRING, {11});
  auto temp_arr_slice = arr_slice_tensor.vec<std::string>();
  std::vector<Output> tensor_list;
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
  temp_arr_slice(0) = "conv1/weight";
  temp_arr_slice(1) = "conv2/weight";
  temp_arr_slice(2) = "conv3/weight";
  temp_arr_slice(3) = "conv4/weight";
  temp_arr_slice(4) = "conv1/bias";
  temp_arr_slice(5) = "conv2/bias";
  temp_arr_slice(6) = "conv3/bias";
  temp_arr_slice(7) = "conv4/bias";
  temp_arr_slice(8) = "hyper/w";
  temp_arr_slice(9) = "hyper/alpha";
  temp_arr_slice(10) = "hyper/eta";
  TF_CHECK_OK(session.Run(std::unordered_map<Output, Input::Initializer, OutputHash>(), std::vector<Output>(), {Save(scope, filename, arr_slice_tensor, tensor_list)}, NULL));
  std::cout << "Wrote snapshot to: " << filename << std::endl;
  return this;
}
PlasticTrain *PlasticTrain::Episodes(int start)
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
  load(std::string("src/differentiable_plasticity/data/snapshot_") + std::to_string(start) + std::string(".ckpt"));
  std::vector<Output> variables = {net.cv1.weight, net.cv2.weight, net.cv3.weight, net.cv4.weight, net.cv1.bias, net.cv2.bias, net.cv3.bias, net.cv4.bias, net.w, net.alpha, net.eta};
  std::vector<Output> grads;
  std::vector<Output> train_ops;
  Output learning_rate = Placeholder(scope, DT_FLOAT, Placeholder::Shape({}));
  TF_CHECK_OK(AddSymbolicGradients(scope, {loss}, variables, &grads));
  for(int i = 0; i < variables.size(); i++)
  {
    train_ops.push_back(ApplyGradientDescent(scope, variables[i], learning_rate, grads[i]));
  }
  for(int numiter = start; numiter < params.nbiter; numiter++)
  {
    float decaying_rate = params.lr * exp((numiter / params.steplr) * log(params.gamma));
    std::vector<Tensor> tensor_list = generateInputsLabelsAndTarget(params, imgdata, false);
    TF_CHECK_OK(session.Run({{inputs, tensor_list[0]}, {labels, tensor_list[1]}, {target, tensor_list[2]}, {learning_rate, decaying_rate}}, train_ops, NULL));
    TF_CHECK_OK(session.Run({{inputs, tensor_list[0]}, {labels, tensor_list[1]}, {target, tensor_list[2]}, {learning_rate, decaying_rate}}, {loss}, &outputs));
    FILE *file = fopen("loss_list.txt", "a");
    fprintf(file, "%d: %0.6f\n", numiter + 1, outputs[0].scalar<float>()(0));
    fclose(file);
    if((numiter + 1) % params.save_every == 0)
    {
      snapshot(numiter);
    }
  }
  return this;
}
