#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

// 트리 노드
typedef struct Node {
    char name[256];
    long long size;
    int is_dir;
    struct Node **children;
    int child_count;
} Node;

// 옵션
typedef struct Option {
    char path[256];
    int max_depth;
    long long min_size;
    int reverse;
} Option;

// 노드 만들기
Node* new_node(char *name, long long size, int is_dir) {
    Node *n = malloc(sizeof(Node));
    strcpy(n->name, name);
    n->size = size;
    n->is_dir = is_dir;
    n->children = malloc(sizeof(Node*) * 100);
    n->child_count = 0;
    return n;
}

// 자식 추가
void add(Node *parent, Node *child) {
    int index = parent->child_count;
    parent->children[index] = child;
    parent->child_count = index+1;
}

// 정렬
int cmp(const void *a, const void *b) {
    Node *x = *(Node**)a;
    Node *y = *(Node**)b;
    if (x->size > y->size) return -1;
    if (x->size < y->size) return 1;
    return 0;
}

// 정렬 실행
void do_sort(Node *n, int rev) {

    if (n->child_count > 0) {
        qsort(n->children, n->child_count, sizeof(Node*), cmp);

        if (rev == 1) {
            int i, j;
            for (i = 0, j = n->child_count - 1; i < j; i++, j--) {
                Node *tmp = n->children[i];
                n->children[i] = n->children[j];
                n->children[j] = tmp;
            }
        }
    }

    int i;
    for (i = 0; i < n->child_count; i++) {
        do_sort(n->children[i], rev);
    }
}

// 메모리 해제
void delete(Node *n) {
    printf("%s, %d", n->name, n->child_count);
    int i;
    for (i = 0; i < n->child_count; i++) {
        delete(n->children[i]);
    }
    free(n->children);
    free(n);
}

// 크기 계산
long long get_size(Node *n) {
    if (n->is_dir == 0) return n->size;
   
    long long total = 0;
    int i;
    for (i = 0; i < n->child_count; i++) {
        total = total + get_size(n->children[i]);
    }
    n->size = total;
    return total;
}

// 디렉토리 읽기
void scan(char *path, Node *parent, int depth, int max) {

    if (max != -1 && depth >= max) return;

    DIR *d = opendir(path);
    if (d==NULL) return;

    struct dirent *item;  // dirent -> d_name(파일 이름), d_type(파일 타입)
    while ((item = readdir(d)) != NULL) {
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)  // readdir
            continue;

        // 읽고있는 파일 경로
        char p[1024];
        sprintf(p, "%s/%s", path, item->d_name);

       
        // 파일 정보
        struct stat s; // st_mode(파일 타입),st_size(파일 길이 바이트),  
        if (stat(p, &s) == -1) continue;

        int dir = S_ISDIR(s.st_mode); //디렉터리면 1
        long long sz = dir ? 0 : s.st_size;

        Node *child = new_node(item->d_name, sz, dir);
        add(parent, child);


        if (dir) {
           scan(p, child, depth + 1, max);
        }
    }
    closedir(d);
}

// 크기 표시
void print_size(long long bytes, char *buf) {

    if (bytes < 1024) {
        sprintf(buf, "%lld B", bytes);

    } else if (bytes < 1024 * 1024) {
        sprintf(buf, "%.1f KB", bytes / 1024.0);

    } else if (bytes < 1024LL * 1024 * 1024) {
        sprintf(buf, "%.1f MB", bytes / (1024.0 * 1024));

    } else {
        sprintf(buf, "%.1f GB", bytes / (1024.0 * 1024 * 1024));
    }
}

// 출력
void print(Node *n, int depth, char *pre, int last, long long min) {
    if (n->size < min) return;

    char s[64];
    print_size(n->size, s);

    if (depth == 0) { // 루트
        printf("[%s] %s%s\n", s, n->name, n->is_dir ? "/" : "");
    } else {
        printf("%s%s[%s] %s%s\n",
               pre, last ? "└── " : "├── ", s, n->name, n->is_dir ? "/" : "");
    }

    if (n->child_count == 0) return;

    char new_pre[1024];
    sprintf(new_pre, "%s%s", pre, last ? "    " : "│   ");

    int i;
    for (i = 0; i < n->child_count; i++) {
        print(n->children[i],
              depth + 1,
              new_pre,
              i == n->child_count - 1, // 아니면 0, 같으면 1(last)
              min);
    }
}

int main(int argc, char *argv[]) {
    Option opt;
    strcpy(opt.path, ".");
    opt.max_depth = -1;
    opt.min_size = 0;
    opt.reverse = 0;

    int i;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            opt.max_depth = atoi(argv[i] + 12);
        } else if (strncmp(argv[i], "--min-size=", 11) == 0) {
            opt.min_size = atoll(argv[i] + 11); // long long
        } else if (strcmp(argv[i], "--reverse") == 0) {
            opt.reverse = 1;
        } else if (argv[i][0] != '-') {
            strcpy(opt.path, argv[i]);
        }
    }

    if (access(opt.path, F_OK) != 0) {
        printf("경로를 찾을 수 없습니다: %s\n", opt.path);
        return 1;
    }

    char *dname = strrchr(opt.path, '/');
    dname = dname ? dname + 1 : opt.path;

    Node *root = new_node(dname, 0, 1);
    scan(opt.path, root, 0, opt.max_depth);
    get_size(root);
    do_sort(root, opt.reverse);
    print(root, 0, "", 1, opt.min_size); //0 false

    delete(root);

    return 0;
}
