#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <cassert>
#include "rbtree.hpp" // RBTree와 RBNode 정의가 포함된 헤더 파일

#include <set> // 중복 없는 집합 사용

template <typename T>
bool check_black_balance(const std::unique_ptr<RBNode<T>>& node, int black_count = 0, std::set<int>* path_black_counts = nullptr) {
    if (!node) {
        // 리프 노드(NULL)에 도달하면 현재 경로의 Black Node 개수를 기록
        if (path_black_counts)
            path_black_counts->insert(black_count);
        return true;
    }

    // 현재 노드가 Black이면 Black Node 개수 증가
    if (node->color == BLK)
        black_count++;

    // 좌우 서브트리를 재귀적으로 검사
    std::set<int> left_black_counts, right_black_counts;
    bool left_balanced = check_black_balance(node->left, black_count, &left_black_counts);
    bool right_balanced = check_black_balance(node->right, black_count, &right_black_counts);

    // 좌우 서브트리에서 Black Node 개수가 동일해야 함
    if (left_black_counts != right_black_counts)
        return false;

    // 현재 노드가 리프 노드가 아니면 path_black_counts에 결과 병합
    if (path_black_counts) {
        path_black_counts->insert(left_black_counts.begin(), left_black_counts.end());
        path_black_counts->insert(right_black_counts.begin(), right_black_counts.end());
    }

    return left_balanced && right_balanced;
}

int main() {
    RBTree<int> rbtree; // RBTree 객체 생성
    int keys[300] = {
        82, 116, 185, 111, 89, 254, 132, 218, 284, 203, 20, 209, 291, 149, 156, 143, 25, 78, 107, 273, 
        86, 40, 27, 192, 177, 277, 32, 208, 24, 131, 29, 68, 98, 97, 120, 52, 128, 226, 166, 49, 
        238, 41, 295, 90, 219, 57, 83, 138, 62, 182, 85, 36, 84, 270, 265, 256, 171, 127, 133, 69, 
        190, 38, 54, 45, 193, 43, 263, 207, 217, 274, 272, 5, 262, 58, 260, 198, 87, 162, 64, 96, 
        102, 126, 42, 23, 297, 34, 63, 59, 37, 222, 112, 146, 234, 103, 250, 215, 110, 246, 48, 157, 
        33, 267, 124, 296, 93, 4, 70, 47, 135, 74, 22, 21, 204, 170, 91, 294, 227, 240, 178, 167, 
        118, 231, 278, 9, 197, 202, 276, 285, 145, 191, 95, 201, 251, 121, 140, 79, 298, 186, 77, 279, 
        163, 99, 31, 6, 10, 101, 137, 53, 176, 153, 184, 105, 72, 290, 206, 282, 51, 35, 173, 183, 
        224, 280, 245, 179, 66, 281, 255, 60, 269, 154, 194, 65, 239, 241, 252, 172, 196, 80, 257, 139, 
        214, 76, 55, 232, 221, 164, 100, 88, 175, 258, 299, 61, 11, 228, 115, 92, 13, 19, 160, 174, 
        12, 286, 134, 50, 169, 81, 1, 271, 212, 136, 125, 152, 67, 3, 114, 94, 223, 288, 161, 237, 
        7, 28, 144, 8, 243, 168, 283, 199, 39, 180, 244, 275, 248, 200, 2, 147, 247, 225, 71, 30, 
        123, 268, 158, 15, 235, 189, 155, 16, 119, 216, 104, 75, 122, 220, 130, 18, 17, 253, 56, 233, 
        159, 187, 165, 113, 213, 106, 195, 289, 148, 188, 129, 117, 205, 151, 266, 230, 210, 108, 264, 261, 
        249, 293, 44, 300, 150, 236, 14, 229, 73, 287, 181, 46, 211, 242, 292, 26, 141, 142, 259, 109
    };

    int delkeys[300] = {
        198, 194, 83, 62, 242, 164, 241, 234, 8, 108, 156, 26, 159, 15, 110, 152, 13, 256, 60, 262, 
        179, 236, 209, 88, 222, 54, 35, 292, 239, 203, 115, 126, 205, 197, 70, 299, 51, 93, 269, 282, 
        166, 263, 56, 206, 286, 218, 167, 5, 183, 136, 4, 52, 274, 172, 258, 133, 20, 113, 74, 79, 
        280, 125, 177, 266, 272, 97, 155, 192, 31, 2, 285, 124, 142, 169, 45, 252, 102, 129, 154, 120, 
        92, 119, 84, 78, 243, 180, 295, 251, 253, 254, 178, 283, 50, 38, 46, 238, 250, 73, 89, 33, 
        261, 278, 9, 63, 111, 187, 11, 232, 147, 228, 37, 80, 257, 201, 10, 53, 182, 208, 42, 85, 
        221, 210, 69, 44, 153, 162, 18, 260, 121, 219, 290, 1, 173, 233, 189, 270, 229, 181, 118, 86, 
        94, 248, 225, 90, 213, 176, 22, 281, 237, 288, 101, 23, 217, 294, 138, 175, 186, 72, 29, 16, 
        224, 171, 21, 230, 114, 227, 271, 151, 75, 193, 212, 117, 25, 264, 207, 287, 64, 71, 112, 6, 
        134, 65, 170, 273, 216, 76, 67, 139, 204, 143, 249, 34, 77, 158, 55, 14, 223, 255, 202, 43, 
        47, 196, 100, 246, 240, 96, 157, 127, 284, 27, 57, 122, 137, 148, 298, 199, 188, 289, 200, 130, 
        141, 105, 40, 168, 12, 184, 49, 135, 185, 140, 150, 39, 109, 275, 144, 297, 190, 99, 123, 103, 
        107, 174, 277, 48, 91, 59, 161, 24, 128, 146, 163, 131, 235, 244, 104, 32, 265, 81, 215, 68, 
        66, 132, 267, 36, 291, 245, 191, 214, 296, 87, 300, 28, 3, 116, 231, 211, 220, 259, 276, 145, 
        293, 165, 106, 98, 7, 247, 195, 19, 279, 61, 58, 30, 268, 226, 41, 82, 17, 149, 160, 95
    };


    // 삽입 테스트
    for (int key : keys) {
        rbtree.insert(key);

        // std::cout << "RBTree structure after deletion:" << std::endl;

        // if (rbtree.root) {
            // size_t max_depth = rbtree.root->get_max_depth(); // 트리 최대 깊이
            // for (size_t level = 0; level < max_depth; ++level) {
            //     std::cout << rbtree.root->format_level(level) << std::endl;
            // }
        // } else {
        //     std::cout << "Tree is now empty." << std::endl;
        // }
        // std::cout << "\n\n" << std::endl;

        // Black Balance 검사
        if (!check_black_balance(rbtree.root)) {
            std::cerr << "Black balance violation detected" << std::endl;
        }
    }
    std::cout << "RBTree structure" << std::endl;

    size_t max_depth = rbtree.root->get_max_depth(); // 트리 최대 깊이
    for (size_t level = 0; level < max_depth; ++level) {
        std::cout << rbtree.root->format_level(level) << std::endl;
    }
    std::cout << "\n\n" << std::endl;
    std::cout << "All insertions maintained black balance." << std::endl;

    // 삭제 테스트
    for (int key : delkeys) {
        rbtree.remove(key); // 트리에서 삭제å

        // 트리 구조 출력
        std::cout << "RBTree structure after deletion:" << std::endl;

        if (rbtree.root) {
            size_t max_depth = rbtree.root->get_max_depth(); // 트리 최대 깊이
            for (size_t level = 0; level < max_depth; ++level) {
                std::cout << rbtree.root->format_level(level) << std::endl;
            }
        } else {
            std::cout << "Tree is now empty." << std::endl;
        }
        std::cout << std::endl;

        // Black Balance 검사
        if (rbtree.root && !check_black_balance(rbtree.root)) {

            std::cerr << "Black balance violation detected after deleting key: " << key << std::endl;

            return 1; // 프로그램 종료
        }
    }

    std::cout << "All deletions maintained black balance." << std::endl;

    return 0;
}
