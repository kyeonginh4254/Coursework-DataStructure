#include <iostream>
#include "btree.hpp" // BTree와 BTreeNode 정의가 포함된 헤더 파일

int main(void) {
    BTree<int, 3> btree;

    int keys[100] = {18, 26, 15, 16, 42, 70, 57, 93, 22, 73, 5, 39, 9, 43, 51, 28, 35, 10, 88, 78, 1, 65, 29, 98, 100, 81, 31, 40, 8, 92, 44, 90, 55, 47, 19, 12, 54, 45, 32, 68, 38, 46, 2, 62, 82, 6, 64, 34, 4, 76, 87, 11, 37, 23, 56, 24, 27, 75, 95, 7, 63, 17, 97, 14, 13, 99, 41, 67, 20, 21, 91, 3, 80, 61, 77, 85, 84, 83, 79, 86, 58, 71, 66, 30, 48, 94, 72, 60, 89, 69, 50, 49, 33, 59, 36, 25, 52, 53, 96, 74};

    int delkeys[100] = {97, 69, 67, 50, 96, 15, 60, 11, 32, 2, 86, 48, 72, 56, 95, 49, 22, 83, 13, 94, 100, 5, 33, 12, 89, 73, 52, 64, 41, 85, 59, 25, 6, 54, 36, 44, 39, 61, 18, 46, 63, 42, 35, 77, 58, 19, 8, 31, 84, 3, 29, 30, 40, 91, 47, 26, 66, 78, 76, 53, 27, 92, 74, 57, 75, 24, 62, 10, 17, 51, 88, 98, 68, 79, 82, 99, 20, 28, 70, 55, 80, 71, 23, 34, 16, 65, 93, 81, 21, 4, 87, 90, 43, 45, 7, 38, 37, 9, 1, 14};

    for (int key : keys) {
        std::cout << "Insert " << key << std::endl;
        btree.insert(key);
        std::cout << btree.format() << std::endl;
    }

    std::cout << "---------------------" << std::endl;

    for (int key : delkeys) {
        std::cout << "Delete " << key << std::endl;
        btree.remove(key);
        std::cout << btree.format() << std::endl;
    }

    // 트리의 깊이 출력
    auto depth = btree.depth();
    if (depth.has_value()) {
        std::cout << "트리 깊이: " << depth.value() << std::endl;
    } else {
        std::cout << "트리가 비어 있습니다." << std::endl;
    }

    // 최우측 키 출력
    auto rightmost = btree.find_rightmost_key();
    if (rightmost.has_value()) {
        std::cout << "가장 큰 키: " << rightmost.value() << std::endl;
    } else {
        std::cout << "트리가 비어 있습니다." << std::endl;
    }

    return 0;
}

