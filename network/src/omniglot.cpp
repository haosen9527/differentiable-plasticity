#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/ops/standard_ops.h>

#include "network/omniglot.h"

convolution::convolution(int c_in, int c_out, int k_size, int stride): c_in(c_in), c_out(c_out), k_size(k_size), stride(stride), initialized(false) {}

Output convolution::operator()(const tensorflow::Scope &scope, tensorflow::ClientSession &session, Input inputx, const std::unordered_map<Output, Input::Initializer, OutputHash> &feed_map)
{
    if(!initialized)
    {
        weight = Variable(scope, {c_out, c_in, k_size, k_size}, DT_FLOAT);
        bias = Variable(scope, {c_out}, DT_FLOAT);
        TF_CHECK_OK(session.Run(feed_map, {Assign(scope, weight, Multiply(scope, 0.01f, RandomUniform(scope, {c_out, c_in, k_size, k_size}, DT_FLOAT))), Assign(scope, bias, Multiply(scope, 0.01f, RandomUniform(scope, {c_out}, DT_FLOAT)))}, NULL));
        initialized = true;
    }
    return Transpose(scope, MaxPool(scope, BiasAdd(scope, Conv2D(scope, Transpose(scope, inputx, {0,2,3,1}), Transpose(scope, weight, {3,2,1,0}), {1,1,1,1}, "VALID"), bias), {1, stride, stride, 1}, {1, stride, stride, 1}, "VALID"), {0,3,1,2});
}

network::network(bool is_oja, bool flare, bool is_free, bool is_tanh, bool is_relu, int nbf, int nbclasses, const tensorflow::Scope &scope, tensorflow::ClientSession &session):
  is_oja(is_oja), is_free(is_free), is_tanh(is_tanh), is_relu(is_relu), scope(scope), session(session)
{
    this->is_oja = is_oja;
    if (flare)
    {
        this->cv1 = convolution(1, nbf / 4, 3, 2);
        this->cv2 = convolution(nbf / 4, nbf / 4, 3, 2);
        this->cv3 = convolution(nbf / 4, nbf / 2, 3, 2);
        this->cv4 = convolution(nbf / 2, nbf, 3, 2);
    }
    else
    {
        this->cv1 = convolution(1, nbf, 3, 2);
        this->cv2 = convolution(nbf, nbf, 3, 2);
        this->cv3 = convolution(nbf, nbf, 3, 2);
        this->cv4 = convolution(nbf, nbf, 3, 2);
    }
    w = Variable(this->scope, {nbf, nbclasses}, DT_FLOAT);
    this->session.Run({Assign(this->scope, w, Multiply(this->scope, 0.01f, RandomNormal(this->scope, {nbf, nbclasses}, DT_FLOAT)))}, NULL);
    if (is_free)
    {
        alpha = Variable(this->scope, {nbf, nbclasses}, DT_FLOAT);
        this->session.Run({Assign(this->scope, alpha, Multiply(this->scope, 0.01f, RandomUniform(this->scope, {nbf, nbclasses}, DT_FLOAT)))}, NULL);
    }
    else
    {
        alpha = Variable(this->scope, tensorflow::PartialTensorShape({1}), DT_FLOAT);
        this->session.Run({Assign(this->scope, alpha, {0.01f})}, NULL);
    }
    eta = Variable(this->scope, tensorflow::PartialTensorShape({1}), DT_FLOAT);
    this->session.Run({Assign(this->scope, eta, {0.01f})}, NULL);
    //this->hebb = Variable(scope, {nbf, nbclasses}, DT_FLOAT);
    //this->hebb_assign_zero = Assign(scope, hebb, Fill(scope, {nbf, nbclasses}, 0.0f));
}

network::~network() {}

Output network::forward(Input inputx, Input input_label, int nbf, int nbclasses, Output &hebb, const std::unordered_map<Output, Input::Initializer, OutputHash> &feed_map)
{
    Output active;
    if(is_tanh)
    {
        active = Tanh(scope, this->cv1(scope, session, inputx, feed_map));
        active = Tanh(scope, this->cv2(scope, session, active, feed_map));
        active = Tanh(scope, this->cv3(scope, session, active, feed_map));
        active = Tanh(scope, this->cv4(scope, session, active, feed_map));
    }
    else if(is_relu)
    {
        active = Relu(scope, this->cv1(scope, session, inputx, feed_map));
        active = Relu(scope, this->cv2(scope, session, active, feed_map));
        active = Relu(scope, this->cv3(scope, session, active, feed_map));
        active = Relu(scope, this->cv4(scope, session, active, feed_map));
    }
    else
    {
        active = Selu(scope, this->cv1(scope, session, inputx, feed_map));
        active = Selu(scope, this->cv2(scope, session, active, feed_map));
        active = Selu(scope, this->cv3(scope, session, active, feed_map));
        active = Selu(scope, this->cv4(scope, session, active, feed_map));
    }
    Output active_in = Reshape(scope, active, {-1, nbf});
    active = Add(scope, MatMul(scope, active_in, Add(scope, this->w, Multiply(scope, this->alpha, hebb))), Multiply(scope, 1000.0f, input_label)); // The expectation is that a nonzero inputlabel will overwhelm the inputs and clamp the outputs
    Output active_out = Softmax(scope, active);
    if(is_oja)
    {
        Output active_in_r0 = Reshape(scope, StridedSlice(scope, active_in, {0, 0}, {1, nbf}, {1, 1}), {nbf});
        Output active_out_r0 = Reshape(scope, StridedSlice(scope, active_out, {0, 0}, {1, nbclasses}, {1, 1}), {nbclasses});
        hebb = Add(scope, hebb, Multiply(scope, this->eta, Multiply(scope, Sub(scope, ExpandDims(scope, active_in_r0, 1), Multiply(scope, hebb, ExpandDims(scope, active_out_r0, 0))), ExpandDims(scope, active_out_r0, 0)))); // Oja's rule. Remember that yin, yout are row vectors (dim (1,N)). Also, broadcasting!
    }
    else
    {
        Output product = BatchMatMul(scope, ExpandDims(scope, active_in, 2), ExpandDims(scope, active_out, 1));
        Output product_r0 = Reshape(scope, StridedSlice(scope, product, {0, 0, 0}, {1, nbf, nbclasses}, {1, 1, 1}), {nbf, nbclasses});
        hebb = Add(scope, Multiply(scope, Sub(scope, 1.0f, this->eta), hebb), Multiply(scope, this->eta, product_r0)); // bmm used to implement outer product; remember activs have a leading singleton dimension
    }
    return active_out;
}
