#include <stdio.h>
#include <stdlib.h>
#include <string.h >

int compare(const char* str1, const char* str2, int n);



void getNext(const char* pattern, int *next) {
    next[0] = 0;
    int i = 0;
    int j = 0;
    int n = strlen(pattern);
    for (i = 1; i < n; i++) {
        for (j = 1; j <= i; j++) {
            if (compare(pattern, pattern+j, i+1-j) == 0) break;
        }
        next[i] = i + 1 - j;
    }
}

void getNext2(const char* pattern, int *next) {
    next[0] = 0;
    int i = 0;
    int j = 0;
    int n = strlen(pattern);
    for (i = 1; i < n; i++) {
        while (j > 0 && pattern[i] != pattern[j]) {
            j = next[j - 1];
        }
        if (pattern[j] == pattern[i]) {
            j++;
        }
        next[i] = j;
    }
}

void make_next(const char *pattern, int *next) {

	int q, k;
	int m = strlen(pattern);

	next[0] = 0;
	for (q = 1,k = 0;q < m; q ++) {

		while (k > 0 && pattern[q] != pattern[k])
			k = next[k-1];

		if (pattern[q] == pattern[k]) {
			k ++;
		}

		next[q] = k;
	}
	// next[0] = 0;
	// q=1, k=0, pattern[q]:pattern[k] = b:a, next[1] = 0;
	// q=2, k=0, pattern[q]:pattern[k] = c:a, next[2] = 0;
	// q=3, k=0, pattern[q]:pattern[k] = a:a, k++, next[3] = 1;
	// q=4, k=1, pattern[q]:pattern[k] = b:b, k++, next[4] = 2;
	// q=5, k=2, pattern[q]:pattern[k] = c:c, k++, next[5] = 3;
	// q=6, k=3, pattern[q]:pattern[k] = d:a, k=next[k-1] -> k=0; next[6] = 0;
}

int compare(const char* str1, const char* str2, int n) {
    const char *p1 = NULL;
    const char *p2 = NULL;
    int i = 0;
    for (p1 = str1, p2 = str2, i = 0; i < n; p1++, p2++, i++) {
        if (*p1 < *p2) return -1;
        else if (*p1 > *p2) return 1;
    }
    return 0;
}

// int kmp(const char* text, const char* pattern, int* next) {
//     int n = strlen(text);
//     int m = strlen(pattern);
//     int i = 0; 
//     int j = 0;
//     for (i = 0; i <= n - m;) {
//         j = 0;
//         if (text[i] != pattern[j]) {
//             i++;
//             continue;
//         }
//         while (i <= n-m && j < m && text[i] == pattern[j]) {
//             i++; 
//             j++;
//         }
//         if (j == m) return i - m;
//         i = i - next[j-1];
//     }
//     return -1;
// }



int kmp_my1(const char* text, const char* pattern, int* next) {
    int n = strlen(text);
    int m = strlen(pattern);
    int i = 0; 
    int j = 0;
    for (i = 0; i <= n - m;) {
        if (text[i] != pattern[j]) {
            i++;
            j = 0;
        }
        while (i <= n-m && j < m && text[i] == pattern[j]) {
            i++; 
            j++;
        }
        if (j == m) return i - m;
        j = next[j-1];
    }
    return -1;
}

int kmp_my2(const char* text, const char* pattern, int* next) {
    int n = strlen(text);
    int m = strlen(pattern);
    int i = 0; 
    int j = 0;
    for (i = 0; i <= n - m; i++) {
        while (j > 0 && text[i] != pattern[j]) {
            j = next[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == m) {
            return i + 1 - m;
        }
    }
    return -1;
}


int kmp(const char *text, const char *pattern, int *next) {

	int n = strlen(text);
	int m = strlen(pattern);

	make_next(pattern, next);
	
	int i, q;
	for (i = 0, q = 0;i < n;i ++) {

		while (q > 0 && pattern[q] != text[i]) {
			q = next[q-1];
		}

		if (pattern[q] == text[i]) {
			q ++;
		}

		if (q == m) {
			//printf("Pattern occurs with shift: %d\n", (i-m+1));
			break;
		}
	}

	return i-q+1;
}

int main() {

	int i;
	int next[20] = {0};

	char *text = "ababxbababababcdababcabddcadfdsss";
	char *pattern = "abcabd"; 


	//  int idx = kmp(text, pattern, next);
    int idx = kmp_my2(text, pattern, next);
	 printf("match pattern : %d\n", idx);


 //   getNext(pattern, next);
    getNext2(pattern, next);
 //   make_next(pattern, next);

	for (i = 0;i < strlen(pattern);i ++) {
		printf("%4d", next[i]);
	}
	printf("\n");

	return 0;

}

