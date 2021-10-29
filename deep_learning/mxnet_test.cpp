#include "mxnet-cpp/MxNetCpp.h"

using namespace mxnet::cpp;

int main()
{
    Symbol data = Symbol::Variable("data");
	Symbol data_label = Symbol::Variable("data_label");
    Symbol conv1_w("conv1_w"), conv1_b("conv1_b");
	Symbol fc1_w("fc1_w"), fc1_b("fc1_b");
	Symbol fc2_w("fc2_w"), fc2_b("fc2_b");


    Symbol conv1 = Convolution("conv1", data, conv1_w, conv1_b, Shape(5, 5), 20);
    Symbol tanh1 = Activation("tanh1", conv1, ActivationActType::kTanh);
    Symbol pool1 = Pooling("pool1", tanh1, Shape(2, 2), PoolingPoolType::kMax, false, false,
                           PoolingPoolingConvention::kValid, Shape(2, 2));


    Symbol flatten = Flatten("flatten", pool1);
    Symbol fc1 = FullyConnected("fc1", flatten, fc1_w, fc1_b, 500);
    Symbol tanh2 = Activation("tanh4", fc1, ActivationActType::kTanh);
    Symbol fc2 = FullyConnected("fc2", tanh2, fc2_w, fc2_b, 10);

    Symbol lenet = SoftmaxOutput("softmax", fc2, data_label);

    for (auto s : lenet.ListArguments()) LG << s; 

    MXNotifyShutdown();
}