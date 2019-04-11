#ifndef NETWORK_OMNIGLOT_H_
#define NETWORK_OMNIGLOT_H_

#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/ops/standard_ops.h>

using tensorflow::Input;
using tensorflow::Output;
using tensorflow::OutputHash;
using tensorflow::Tensor;
using tensorflow::DT_FLOAT;
using namespace tensorflow::ops;

class convolution
{
private:
    int c_in, c_out, k_size, stride;
    bool initialized;
public:
    Output weight, bias;
    convolution(int c_in = 1, int c_out = 1, int k_size = 3, int stride = 2);
    Output operator()(const tensorflow::Scope &scope, tensorflow::ClientSession &session, Input inputx, const std::unordered_map<Output, Input::Initializer, OutputHash> &feed_map);
};

class network
{
private:
    bool is_oja, is_free, is_tanh, is_relu;
    tensorflow::Scope scope;
    tensorflow::ClientSession &session;
public:
    convolution cv1, cv2, cv3, cv4;
    Output w, alpha, eta;
    //Output hebb_assign, hebb_assign_zero, hebb;
    network(bool is_oja, bool flare, bool is_free, bool is_tanh, bool is_relu, int nbf, int nbclasses, const tensorflow::Scope &scope, tensorflow::ClientSession &session);
    ~network();
    Output forward(Input inputx, Input input_label, int nbf, int nbclasses, Output &hebb, const std::unordered_map<Output, Input::Initializer, OutputHash> &feed_map);
};

#endif
