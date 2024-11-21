#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void shuffle_and_print(int n) {
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // 1부터 n까지의 숫자를 배열에 초기화
    for (int i = 0; i < n; i++) {
        arr[i] = i + 1;
    }

    // 배열을 섞기 (Fisher-Yates Shuffle 알고리즘)
    srand(time(NULL)); // 랜덤 시드 설정
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // arr[i]와 arr[j] 교환
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    // 배열 출력 (쉼표로 구분)
    printf("{\n");
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) {
            printf(", ");
        }
        if (i % 20 == 19) {
            printf("\n");
        }
    }
    printf("}\n");

    // 동적 메모리 해제
    free(arr);
}

int main() {
    int N;
    printf("Enter the value of N: ");
    scanf("%d", &N);

    if (N <= 0) {
        printf("N must be greater than 0.\n");
        return 1;
    }

    shuffle_and_print(N);

    return 0;
}