from cProfile import label
from random import shuffle
import numpy as np
from matplotlib import pyplot as plt
from torch import argmax


class LinearLayer:
    def __init__(self, input_size, output_size):
        self.input_size = input_size
        self.output_size = output_size
        self.last_input = None
        self.weights = np.random.randn(input_size, output_size) * np.sqrt(2/input_size)
        self.bias = np.zeros((1, self.output_size))
        self.training = False

    def forward(self, x):
        if self.training:
            self.last_input = x
        return np.dot(x, self.weights) + self.bias

    def backward(self, grad_y):
        if self.last_input is None:
            raise RuntimeError("No input to back-propagate through")
        grad_bias = np.sum(grad_y, axis=0, keepdims=True)
        grad_weights = np.matmul(self.last_input.T, grad_y)
        grad_x = np.matmul(grad_y, self.weights.T)
        return grad_x, self.weights, grad_weights, self.bias, grad_bias

    def train(self):
        self.training = True

    def eval(self):
        self.training = False


class Tanh:
    def __init__(self):
        self.last_input = None
        self.is_training = False

    def forward(self, x):
        if self.is_training:
            self.last_input = x
        return np.tanh(x)

    def backward(self, grad_y):
        return (1 - np.power(np.tanh(self.last_input), 2)) * grad_y, None, None, None, None

    def train(self):
        self.is_training = True

    def eval(self):
        self.is_training = False


class SoftmaxCrossEntropy:
    def __init__(self):
        pass

    def __call__(self, scores, labels):
        scores = np.exp(scores - np.max(scores, axis=1, keepdims=True))
        scores /= np.sum(scores, axis=1, keepdims=True)
        positive_scores = scores[np.arange(batch_size), labels]
        loss = np.mean(-np.log(positive_scores))

        one_hot = np.zeros_like(scores)
        one_hot[np.arange(batch_size), labels] = 1.0
        grad = (scores - one_hot) / batch_size
        return loss, grad


class MLP:
    def __init__(self):
        # layer size = [10, 8, 8, 4]
        # 初始化所需参数   
        self.layer_size = [10, 10, 8, 8, 4]
        self.layers = []
        self.loss = SoftmaxCrossEntropy()
        for i in range(len(self.layer_size) - 1):
            self.layers.append(LinearLayer(self.layer_size[i], self.layer_size[i + 1]))
            if i < len(self.layer_size) - 2:
                self.layers.append(Tanh())

    def forward(self, x):
        # 前向传播
        for layer in self.layers:
            x = layer.forward(x)
        return x

    def backward(self, input, label, lr):  # 自行确定参数表
        # 反向传播
        weights_list = []
        grad_weights_list = []
        bias_list = []
        grad_bias_list = []
        pred = self.forward(input)
        loss, grad = self.loss(pred, label)
        for l in reversed(self.layers):
            grad, weights, grad_weights, bias, grad_bias = l.backward(grad)
            if weights is not None:
                weights_list.append(weights)
                grad_weights_list.append(grad_weights)
                bias_list.append(bias)
                grad_bias_list.append(grad_bias)
        for w, gw, b, gb in zip(weights_list, grad_weights_list, bias_list, grad_bias_list):
            w -= gw * lr
            b -= gb * lr
        return loss

    def train(self):
        for i in range(len(self.layers)):
            self.layers[i].train()

    def eval(self):
        for i in range(len(self.layers)):
            self.layers[i].eval()


def train(mlp: MLP, epochs, lr, inputs, input_labels, batch_size=10):
    """
        mlp: 传入实例化的MLP模型
        epochs: 训练轮数
        lr: 学习率
        inputs: 生成的随机数据
        labels: 生成的one-hot标签
    """
    mlp.train()
    global_step = 0
    num_input = inputs.shape[0]
    input_labels = np.argmax(input_labels, axis=1)
    loss_list = []
    for epoch in range(epochs):
        start_idx = 0
        shuffle_idx = np.random.permutation(num_input)
        while start_idx < num_input:
            end_idx = min(start_idx + batch_size, num_input)
            batch_idx = shuffle_idx[start_idx:end_idx]
            batch_input = inputs[batch_idx]
            batch_label = input_labels[batch_idx]
            loss = mlp.backward(batch_input, batch_label, lr)
            global_step += 1
            loss_list.append(loss)
            start_idx += batch_size
            if global_step % 1 == 0:
                print(f"Global step: {global_step}, epoch: {epoch}, loss: {loss}")
    return loss_list


if __name__ == '__main__':
    # 设置随机种子,保证结果的可复现性
    np.random.seed(1)
    mlp = MLP()
    # 生成数据
    inputs = np.random.randn(100, 10)

    # 生成one-hot标签
    labels = np.eye(4)[np.random.randint(0, 4, size=(1, 100))].reshape(100, 4)

    # 训练
    epochs = 1000
    lr = 1e-2
    batch_size = 10
    loss_list = train(mlp, epochs, lr, inputs, labels.astype(int), batch_size)
