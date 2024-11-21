#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>


template <typename T>
class TreeNode
{
    public:
        T element;
        std::unique_ptr<TreeNode<T>> left;
        std::unique_ptr<TreeNode<T>> right;

        TreeNode<T>(const T& e)
            :element{e}, left{nullptr}, right{nullptr} {}

        ~TreeNode() {}

};


template <typename T>
class BST
{
    public:
        std::unique_ptr<TreeNode<T>> root = nullptr;

        ~BST() {}

        bool insert(const T& key);
        bool search(const T& key);
        bool remove(const T& key);

    private:
        bool insert(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool search(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool remove(std::unique_ptr<TreeNode<T>>& t, const T& key);

        T find_rightmost_key(std::unique_ptr<TreeNode<T>>& t);
};

template <typename T>
bool BST<T>::insert(const T& key) {
    return insert(root, key);
}

template <typename T>
bool BST<T>::search(const T& key) {
    return search(root, key);
}

template <typename T>
bool BST<T>::remove(const T& key) {
    return remove(root, key);
}

template <typename T>
bool BST<T>::insert(std::unique_ptr<TreeNode<T>>& t, const T& key) {
    if (!t) {
        t = std::make_unique<TreeNode<T>>(key);
        return true;
    }

    if (key == t->element) return false;
    
    return (key < t->element) ? insert(t->left, key) : insert(t->right, key);

}

template <typename T>
bool BST<T>::search(std::unique_ptr<TreeNode<T>>& t, const T& key) {
    if (!t) return false;

    if (key == t->element) return true;

    return (key < t->element) ? search(t->left, key) : search(t->right, key);

}

template <typename T>
T BST<T>::find_rightmost_key(std::unique_ptr<TreeNode<T>>& t) {
    TreeNode<T>* tmp = t.get();
    while (tmp->right) {
        tmp = tmp->right.get();
    }
    return tmp->element;
}

template <typename T>
bool BST<T>::remove(std::unique_ptr<TreeNode<T>>& t, const T& key) {

    if (!t) return false;

    if (key != t->element) return (key < t->element) ? remove(t->left, key) : remove(t->right, key);

    if (!(t->left) && !(t->right)) {
        t.reset();
    } else if (!(t->left)) {
        t = std::move(t->right);
    } else if (!(t->right)) {
        t = std::move(t->left);
    } else {
        T next_element = find_rightmost_key(t->left);
        t->element = next_element;
        remove(t->left, next_element);
    }

    return true;

}
