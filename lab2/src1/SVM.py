import numpy as np
# import cvxpy as cp
import cvxopt


class SupportVectorMachine:
    def __init__(self, C=1, kernel='Linear', epsilon=1e-4):
        self.C = C
        self.epsilon = epsilon
        self.kernel = kernel
        self.SV = None
        self.SV_alpha = None
        self.SV_label = None
        self.b = 0
        # Hint: 你可以在训练后保存这些参数用于预测
        # SV即Support Vector，表示支持向量，SV_alpha为优化问题解出的alpha值，
        # SV_label表示支持向量样本的标签。

    def KERNEL(self, x1, x2, d=2, sigma=1):
        # d for Poly, sigma for Gauss
        if self.kernel == 'Gauss':
            K = np.exp(-(np.sum((x1 - x2) ** 2)) / (2 * sigma ** 2))
        elif self.kernel == 'Linear':
            K = np.dot(x1, x2)
        elif self.kernel == 'Poly':
            K = (np.dot(x1, x2) + 1) ** d
        else:
            raise NotImplementedError()
        return K

    def fit(self, train_data, train_label):
        '''
        TODO：实现软间隔SVM训练算法
        train_data：训练数据，是(N, 7)的numpy二维数组，每一行为一个样本
        train_label：训练数据标签，是(N,)的numpy数组，和train_data按行对应
        '''
        num_samples, num_features = train_data.shape
        kernel_matrix = np.zeros((num_samples, num_samples))
        for i in range(num_samples):
            for j in range(num_samples):
                kernel_matrix[i, j] = self.KERNEL(train_data[i], train_data[j])

        # P = np.outer(train_label, train_label) * kernel_matrix
        # q = -1 * np.ones(num_samples)
        # G = np.vstack((np.identity(num_samples) * -1, np.identity(num_samples)))
        # h = np.vstack((np.zeros(num_samples).reshape(-1, 1), self.C * np.ones(num_samples).reshape(-1, 1))).reshape(-1)
        # A = train_label.reshape(1, num_samples)
        # b = np.array(0).reshape(1)
        # x = cp.Variable(num_samples)
        # obj = cp.Minimize(0.5*cp.quad_form(x, P) + q.T @ x)
        # prob = cp.Problem(obj, [G @ x <= h, A @ x == b])
        # prob.solve(verbose=True)
        # x = np.ravel(x.value)

        P = cvxopt.matrix(np.outer(train_label, train_label) * kernel_matrix, tc='d')
        q = cvxopt.matrix(np.ones(num_samples) * -1)
        A = cvxopt.matrix(train_label, (1, num_samples), tc='d')
        b = cvxopt.matrix(0, tc='d')
        G = cvxopt.matrix(np.vstack((np.identity(num_samples) * -1, np.identity(num_samples))))
        h = cvxopt.matrix(
            np.vstack((cvxopt.matrix(np.zeros(num_samples)), cvxopt.matrix(np.ones(num_samples) * self.C))))
        minimization = cvxopt.solvers.qp(P, q, G, h, A, b)
        x = np.ravel(minimization['x'])

        idx = x > self.epsilon
        self.SV = train_data[idx]
        self.SV_alpha = x[idx]
        self.SV_label = train_label[idx]

        self.b = self.SV_label[0]
        for i in range(len(self.SV_alpha)):
            self.b -= self.SV_alpha[i] * self.SV_label[i] * self.KERNEL(self.SV[i], self.SV[0])

    def predict(self, test_data):
        '''
        TODO：实现软间隔SVM预测算法
        train_data：测试数据，是(M, 7)的numpy二维数组，每一行为一个样本
        必须返回一个(M,)的numpy数组，对应每个输入预测的标签，取值为1或-1表示正负例
        '''
        preds = []
        for d in test_data:
            prediction = 0
            for i in range(len(self.SV_alpha)):
                prediction += self.SV_alpha[i] * self.SV_label[i] * self.KERNEL(self.SV[i], d)
            prediction += self.b
            preds.append(np.sign(prediction))
        return np.array(preds)
