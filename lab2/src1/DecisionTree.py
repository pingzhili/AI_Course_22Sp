import numpy as np
from math import log2


class DecisionNode():
    def __init__(self, feature, value, true_branch, false_branch, threshold):
        self.feature_idx = feature
        self.value = value
        self.true_branch = true_branch
        self.false_branch = false_branch
        self.threshold = threshold


def calc_info_gain(l, l1, l2):
    def calc_entropy(y):
        unique_labels = np.unique(y)
        entropy = 0
        for label in unique_labels:
            _p = len(y[y == label]) / len(y)
            entropy += - _p * log2(_p)
        return entropy

    p = len(l1) / len(l)
    info_gain = calc_entropy(l) - p * calc_entropy(l1) - (1 - p) * calc_entropy(l2)
    return info_gain


class DecisionTree:
    def __init__(self, max_depth=64, min_sample_split=2):
        self.root = None
        self.min_samples_split = min_sample_split
        self.max_depth = max_depth

    def fit(self, train_features, train_labels):
        """
        TODO: 实现决策树学习算法.
        train_features是维度为(训练样本数,属性数)的numpy数组
        train_labels是维度为(训练样本数, )的numpy数组
        """
        self.root = self._build_tree(train_features, train_labels, depth=0)

    def predict(self, test_features):
        """
        TODO: 实现决策树预测.
        test_features是维度为(测试样本数,属性数)的numpy数组
        该函数需要返回预测标签，返回一个维度为(测试样本数, )的numpy数组
        """
        pred = [self._predict_sample(s) for s in test_features]
        return pred

    def _build_tree(self, features, labels, depth):
        largest_info_gain = 0
        num_samples, num_features = np.shape(features)
        labels = np.expand_dims(labels, axis=1)
        features_labels = np.concatenate((features, labels), axis=1)
        largest_info_gain, best_feature_idx, best_threshold = 0, 0, 0
        best_sets = (None, None)
        if num_samples >= self.min_samples_split and depth <= self.max_depth:
            for feature_idx in range(num_features):
                feature_vals = np.expand_dims(features[:, feature_idx], axis=1)
                unique_vals = np.unique(feature_vals)
                for threshold in unique_vals:
                    fl1 = np.array([s for s in features_labels if s[feature_idx] >= threshold])
                    fl2 = np.array([s for s in features_labels if s[feature_idx] < threshold])
                    if len(fl1) > 0 and len(fl2) > 0:
                        l1 = fl1[:, -1]
                        l2 = fl2[:, -1]
                        info_gain = calc_info_gain(labels, l1, l2)
                        if info_gain > largest_info_gain:
                            largest_info_gain = info_gain
                            best_feature_idx = feature_idx
                            best_threshold = threshold
                            best_sets = (fl1, fl2)
        if largest_info_gain > 0:
            true_branch = self._build_tree(best_sets[0][:, :-1], best_sets[0][:, -1], depth + 1)
            false_branch = self._build_tree(best_sets[1][:, :-1], best_sets[1][:, -1], depth + 1)
            return DecisionNode(feature=best_feature_idx, threshold=best_threshold, true_branch=true_branch,
                                false_branch=false_branch, value=None)
        else:
            max_count, label_pred = 0, 0
            for label in np.unique(labels):
                count = len(labels[labels == label])
                if count > max_count:
                    max_count = count
                    label_pred = label
            return DecisionNode(feature=None, threshold=None, true_branch=None, false_branch=None, value=label_pred)

    def _predict_sample(self, features, tree=None):
        if tree is None:
            tree = self.root
        if tree.value is not None:  # leaf node
            return tree.value
        else:  # non-leaf node
            if features[tree.feature_idx] >= tree.threshold:
                return self._predict_sample(features, tree.true_branch)
            else:
                return self._predict_sample(features, tree.false_branch)

    def print_tree(self, tree=None):
        if tree is None:
            tree = self.root
        if tree.value is not None:
            print(tree.value)
        else:
            print(f"feature {tree.feature_idx}: {tree.threshold} ? ")
            print("T->", end="")
            self.print_tree(tree.true_branch)
            print("F->", end="")
            self.print_tree(tree.false_branch)

# treenode: [attr, feat[attr] == 1, feat[attr] == 2, feat[attr] == 3]
