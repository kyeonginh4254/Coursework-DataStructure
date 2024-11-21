#ifndef __RBTREE_H_
#define __RBTREE_H_

#include <algorithm>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <string>
#include <sstream>
#include <valarray>
#include <utility>
#include <type_traits>
#include <iostream>
#include <optional>
#include <fstream>

static size_t null_count = 0;

template <typename T>
struct RBNode;

using color_t = bool;
constexpr static color_t RED = false;
constexpr static color_t BLK = true;

/* This is an abstraction for search-path. For debugging purpose */
struct Path;

template<typename T>
struct RBTree {
    std::unique_ptr<RBNode<T>> root = nullptr;

    ~RBTree() = default;

    bool insert(const T&);
    void remove_max();
    void remove_min();
    void remove(const T&);

    const std::optional<T> leftmost_key();
    const std::optional<T> rightmost_key();

    void traverse_inorder(std::function<void(RBNode<T>*)>);

    bool contains(const T& t);

    std::unordered_map<Path, const RBNode<T>&> collect_all_leaves() const;

    std::string format_graphviz();
};

template<typename T>
struct RBNode {
    T key;
    color_t color = RED;
    std::unique_ptr<RBNode<T>> left = nullptr;
    std::unique_ptr<RBNode<T>> right = nullptr;

    RBNode(const T& t);
    ~RBNode();

    bool is_leaf();

    void flip_color();
    static RBNode* rotate_right(std::unique_ptr<RBNode>&);
    static RBNode* rotate_left(std::unique_ptr<RBNode>&);
    static bool is_red(const std::unique_ptr<RBNode>&);

    static RBNode* move_red_right(std::unique_ptr<RBNode>&);
    static RBNode* move_red_left(std::unique_ptr<RBNode>&);

    static RBNode* remove_max(std::unique_ptr<RBNode>&);
    static RBNode* remove_min(std::unique_ptr<RBNode>&);
    static RBNode* remove(std::unique_ptr<RBNode>&, const T&);

    static RBNode* insert(std::unique_ptr<RBNode>&, const T&);
    std::pair<RBNode<T>*, Path> search(const T&, Path);

    void traverse_inorder(std::function<void(RBNode*)>);
    size_t get_max_depth();

    std::unordered_map<size_t, const RBNode&> get_nodes_at_level(size_t);
    void _get_nodes_at_level(size_t,
                             std::unordered_map<size_t, const RBNode&>&,
                             Path);
    std::string format_level(size_t);

    std::unordered_map<Path, const RBNode<T>&> collect_all_leaves(void);
    void _collect_all_leaves(std::unordered_map<Path, const RBNode<T>&>&, Path);

    static RBNode* fix_up(std::unique_ptr<RBNode>&);

    std::string format_graphviz();

    bool contains(const T& t);

    const T& leftmost_key();
    const T& rightmost_key();
};

template<typename T>
RBNode<T>::~RBNode() {
    std::cout << "Node " << key << " destroyed." << std::endl;
}


template<typename T>
bool RBTree<T>::insert(const T& t) {
    if (!root) {
        root = std::make_unique<RBNode<T>>(t);
        root->color = BLK;
        return true;
    }

    root.reset(RBNode<T>::insert(root, t));

    std::cout << "root exist?" << std::endl;
    std::cout << root->color << std::endl;
    std::cout << "root do exist!" << std::endl;

    /* Change root to black. Won't affect the balance */
    if (root->color == RED) {
        root->color = BLK;
    }
    return true;
}

template<typename T>
void RBTree<T>::remove_max() {
    root.reset(RBNode<T>::remove_max(root));

    if (root)
        root->color = BLK;
}

template<typename T>
void RBTree<T>::remove_min() {
    root.reset(RBNode<T>::remove_min(root));

    if (root)
        root->color = BLK;
}

template<typename T>
void RBTree<T>::remove(const T& t) {
    root.reset(RBNode<T>::remove(root, t));

    if (root)
        root->color = BLK;
}

template <typename T>
const std::optional<T> RBTree<T>::leftmost_key() {
    if (!root)
        return std::nullopt;

    return root->leftmost_key();
}

template <typename T>
const std::optional<T> RBTree<T>::rightmost_key() {
    if (!root)
        return std::nullopt;

    return root->rightmost_key();
}

template <typename T>
bool RBTree<T>::contains(const T& t) {
    if (!root)
        return false;

    return root->contains(t);
}

/* This is an abstraction for search-path. For debugging purpose */
struct Path {
    static constexpr size_t LEFT = 0;
    static constexpr size_t RIGHT = 1;
    size_t p_;
    size_t len_;
    size_t num_black_;

    Path() : p_(0), len_(0), num_black_(0) {}
    Path(size_t p, size_t len, size_t num_black)
        : p_(p), len_(len), num_black_(num_black) {}

    static Path down_left(Path sp, color_t color) {
        auto num_black = color == BLK ? sp.num_black_ + 1 : sp.num_black_;
        return Path { (sp.p_ << 1) + LEFT, sp.len_ + 1, num_black };
    }

    static Path down_right(Path sp, color_t color) {
        auto num_black = color == BLK ? sp.num_black_ + 1 : sp.num_black_;
        return Path { (sp.p_ << 1) + RIGHT, sp.len_ + 1, num_black };
    }

    /* For hashtable */
    bool operator==(const Path& other) const {
        return p_ == other.p_;
    }
};

namespace std {
    template<>
    struct hash<Path> {
        std::size_t operator()(const Path& p) const {
            return std::hash<size_t>()(p.p_);
        }
    };
}

template<typename T>
bool RBNode<T>::is_leaf() {
    return !left && !right;
}

template<typename T>
bool RBNode<T>::is_red(const std::unique_ptr<RBNode<T>>& n) {
    return n && n->color == RED;
}

template<typename T>
void RBNode<T>::flip_color() {
    color = !this->color;

    if (left)
        left->color = !left->color;

    if (right)
        right->color = !right->color;
}

template<typename T>
RBNode<T>* RBNode<T>::rotate_right(std::unique_ptr<RBNode<T>>& n) {
    auto x = std::move(n->left);
    n->left = std::move(x->right);
    x->right = std::move(n);
    x->color = x->right->color;
    x->right->color = RED;
    n = std::move(x);
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::rotate_left(std::unique_ptr<RBNode<T>>& n) {
    auto x = std::move(n->right);
    n->right = std::move(x->left);
    x->left = std::move(n);
    x->color = x->left->color;
    x->left->color = RED;
    n = std::move(x);
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::insert(std::unique_ptr<RBNode<T>>& n, const T& t) {
    
    if (!n) {
        std::cout << "\nn->key is null" << std::endl;
        n = std::make_unique<RBNode<T>>(t);
        std::cout << "release " << n->key << "\n" << std::endl;
        return n.release();
    } else {
        std::cout << "\nn->key " << n->key << std::endl;
    }

    if (is_red(n->left) && is_red(n->right)) {
        std::cout << "flip color" << std::endl;
        n->flip_color();
    }

    if (t < n->key) {
        std::cout << "left" << std::endl;
        insert(n->left, t);
    }
    
    if (t > n->key) {
        std::cout << "right" << std::endl;
        insert(n->right, t);
    }

    if (is_red(n->right) && !is_red(n->left)) {
        std::cout << "rotate left" << std::endl;
        rotate_left(n);
    }
    if (is_red(n->left) && is_red(n->left->left)) {
        std::cout << "rotate right" << std::endl;
        rotate_right(n);
    }
    std::cout << "release " << n->key << "\n" << std::endl;
    return n.release();
}

template<typename T>
std::pair<RBNode<T>*, Path> RBNode<T>::search(const T& t, Path sp) {
    if (t > key) {
        return right ?
            right->search(t, Path::down_right(sp, color)) :
            std::pair<RBNode<T>*, Path>{ nullptr, {} };
    } else if (t < key) {
        return left ?
            left->search(t, Path::down_left(sp, color)) :
            std::pair<RBNode<T>*, Path>{ nullptr, {} };
    } else {
        return { this, sp };
    }
}

template<typename T>
RBNode<T>* RBNode<T>::fix_up(std::unique_ptr<RBNode<T>>& n) {

    if (!n) return nullptr;

    if (is_red(n->right) && !is_red(n->left)) {
        rotate_left(n);
    }
    if (is_red(n->left) && is_red(n->left->left)) {
        rotate_right(n);
    }
    if (is_red(n->left) && is_red(n->right)) {
        n->flip_color();
    }

    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::move_red_left(std::unique_ptr<RBNode<T>>& n) {
    n->flip_color();
    
    // If the right child exists and its left child is red, perform rotations
    if (n->right && is_red(n->right->left)) {
        n->right = std::unique_ptr<RBNode<T>>(rotate_right(n->right));
        n = std::unique_ptr<RBNode<T>>(rotate_left(n));
        n->flip_color();
    }
    
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::remove_max(std::unique_ptr<RBNode<T>>& n) {
}


template<typename T>
RBNode<T>* RBNode<T>::move_red_right(std::unique_ptr<RBNode<T>>& n) {
    // Flip colors to prepare for moving right
    n->flip_color();
    
    // If the left child exists and its left child is red, perform rotation
    if (is_red(n->left->left)) {
        n = std::unique_ptr<RBNode<T>>(rotate_right(n));
        n->flip_color();
    }
    
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::remove_min(std::unique_ptr<RBNode<T>>& n) {
    if (!n->left) {
        // If there's no left child, this node is the minimum
        return nullptr;
    }
    
    if (!is_red(n->left) && !is_red(n->left->left)) {
        // Ensure that the left child is not a 2-node
        n = std::unique_ptr<RBNode<T>>(move_red_left(n));
    }
    
    // Recursively delete the minimum in the left subtree
    n->left.reset(remove_min(n->left));
    
    // Fix up any right-leaning links or color inconsistencies
    return fix_up(n);
}


template<typename T>
RBNode<T>* RBNode<T>::remove(std::unique_ptr<RBNode<T>>& n, const T& key) {
    
    std::cout << "Removing key: " << key << " from node with key: " << n->key << std::endl;
    if (key < n->key) {
        std::cout << "case 1" << std::endl;
        if (n->left) {
            if (!is_red(n->left) && (!n->left->left || !is_red(n->left->left))) {
                std::cout << "move red left" << std::endl;
                move_red_left(n);
                std::cout << "here with " << n->key << std::endl;

                std::cout << "flag2 in, with key "<< n->key << std::endl;
                n->left.reset(remove(n->left, key)); // strange
                std::cout << "\nflag2 out, with key "<< n->key << std::endl;
            } else {
                std::cout << "flag2 in, with key "<< n->key << std::endl;
                n->left.reset(remove(n->left, key)); // strange
                std::cout << "\nflag2 out, with key "<< n->key << std::endl;
            }
        }
    } else {
        std::cout << "case 2" << std::endl;
        if (is_red(n->left)){
            std::cout << "rotate right" << std::endl;
            rotate_right(n);
            std::cout << "here with " << n->key << std::endl;
        }
        if (key == n->key && !n->right) {
            std::cout << ">> we found " << key << std::endl;
            return nullptr;
        }
        if (n->right) {
            if (!is_red(n->right) && (!n->right->left || !is_red(n->right->left))) {
                std::cout << "move red right" << std::endl;
                move_red_right(n);
                std::cout << "\nhere with " << n->key << std::endl;
            }
            if (key == n->key) {
                n->key = n->right->leftmost_key();
                n->right.reset(remove_min(n->right));
            } else {
                std::cout << "flag1 in, with key "<< n->key << std::endl;
                n->right.reset(remove(n->right, key));
                std::cout << "flag1 out, with key "<< n->key << std::endl;
            }
        } else {
            if (key == n->key) {
                return nullptr;
            }
        }
    }
    fix_up(n);
    std::cout << n->key << std::endl;
    return n.release();
}

template<typename T>
const T& RBNode<T>::leftmost_key() {
    if (!left)
        return key;

    return left->leftmost_key();
}

template<typename T>
const T& RBNode<T>::rightmost_key() {
    if (!right)
        return key;

    return right->rightmost_key();
}

template<typename T>
bool RBNode<T>::contains(const T& t) {
    if (t == key)
        return true;
    if (t < key)
        return left && left->contains(t);
    else /* t > key */
        return right && right->contains(t);
}

template<typename T>
void RBTree<T>::traverse_inorder(std::function<void(RBNode<T>*)> f) {
    if (root)
        root->traverse_inorder(f);
}

template<typename T>
void RBNode<T>::traverse_inorder(std::function<void(RBNode*)> f) {
    if (left)
        left->traverse_inorder(f);

    f(this);

    if (right)
        right->traverse_inorder(f);
}

template<typename T>
size_t RBNode<T>::get_max_depth() {
    if (is_leaf())
        return 1;

    if (left && right) {
        return 1 + std::max(left->get_max_depth(), right->get_max_depth());
    } else if (left && !right) {
        return 1 + left->get_max_depth();
    } else {
        return 1 + right->get_max_depth();
    }
}

template<typename T>
std::unordered_map<size_t, const RBNode<T>&>
RBNode<T>::get_nodes_at_level(size_t lvl) {
    std::unordered_map<size_t, const RBNode<T>&> ns;

    _get_nodes_at_level(lvl, ns, Path{});

    return ns;
}

template<typename T>
void RBNode<T>::_get_nodes_at_level(
    size_t lvl, std::unordered_map<size_t, const RBNode<T>&>& ns, Path sp) {
    if (lvl == 0) {
        ns.insert({ sp.p_, std::as_const(*this) });
        return;
    }

    if (left)
        left->_get_nodes_at_level(lvl - 1, ns, Path::down_left(sp, color));

    if (right)
        right->_get_nodes_at_level(lvl - 1, ns, Path::down_right(sp, color));
}

template<typename T>
std::string RBNode<T>::format_level(size_t lvl) {
    static const std::string red = "\033[1;31m";
    static const std::string reset = "\033[0m";

    std::ostringstream os;

    auto ns = get_nodes_at_level(lvl);
    for (auto i = 0; i < (1 << lvl); i++) {
        const auto& pn = ns.find(i);
        if (pn != ns.end()) {
            const auto& n = pn->second;
            if (n.color == RED) os << red << n.key << reset << ' ';
            else os << n.key << ' ';
        } else {
            os << "- ";
        }
    }

    return os.str();
}

template<typename T>
std::unordered_map<Path, const RBNode<T>&>
RBTree<T>::collect_all_leaves() const {
    std::unordered_map<Path, const RBNode<T>&> all_leaves;

    if (!root)
        return all_leaves;

    return root->collect_all_leaves();
}

template<typename T>
std::unordered_map<Path, const RBNode<T>&>
RBNode<T>::collect_all_leaves() {
    std::unordered_map<Path, const RBNode<T>&> all_leaves;

    _collect_all_leaves(all_leaves, Path {});

    return all_leaves;
}

template<typename T>
void
RBNode<T>::_collect_all_leaves(std::unordered_map<Path,const RBNode<T>&>& ls,
                               Path p) {
    if (is_leaf())
        ls.insert({ p, std::as_const(*this) });

    if (left)
        left->_collect_all_leaves(ls, Path::down_left(p, left->color));

    if (right)
        right->_collect_all_leaves(ls, Path::down_right(p, right->color));
}

template<typename T>
RBNode<T>::RBNode(const T& t) 
    : key(t), color(RED), left(nullptr), right(nullptr) {
    std::cout << "Node " << key << " created." << std::endl;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const RBTree<T>& rbtree) {
    if (!rbtree.root)
        return os;

    auto max_depth = rbtree.root->get_max_depth();
    for (auto i = 0; i < max_depth; i++)
        os << rbtree.root->format_level(i) << '\n';

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const RBNode<T>& rbnode) {
    auto max_depth = rbnode.get_max_depth();
    for (auto i = 0; i < max_depth; i++)
        os << rbnode.format_level(i) << '\n';

    return os;
}

template <typename T>
std::string RBTree<T>::format_graphviz() {
    std::ostringstream os;
    if(!root) {
        os << "None\n";
        return os.str();
    }
    os << "graph RBTree {\n"
       << "\tnode [fontname=\"Arial\"];\n"
       << root->format_graphviz()
       << "}\n";

    return os.str();
}

template <typename T>
std::string RBNode<T>::format_graphviz() {
    std::ostringstream os;

    if(left) {
        os << '\t' << key << " -- " << left->key;
        if(is_red(left))
            os << "[color=red,penwidth=3.0]";
        else
            os << "[penwidth=3.0]";
        os << ";\n" << left->format_graphviz();
    } else {
        os << "\tnull" << null_count << "[shape=point];\n"
           << "\t" << key << " -- " << "null" << null_count++ << ";\n";
    }

    if(right) {
        os << '\t' << key << " -- " << right->key;
        if(is_red(right))
            os << "[color=red,penwidth=3.0]";
        else
            os << "[penwidth=3.0]";
        os << ";\n" << right->format_graphviz();
    } else {
        os << "\tnull" << null_count << "[shape=point];\n"
           << "\t" << key << " -- " << "null" << null_count++ << ";\n";
    }

    return os.str();
}

#endif // __RBTREE_H_
