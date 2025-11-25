#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "args.h"
#include "utils.h"
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

// Tree 구조 정의
typedef struct TreeNode {
    char *name;
    char *path;
    long long size;
    bool is_dir;
    struct TreeNode **children;
    int child_count;
    int capacity;
} TreeNode;

typedef struct {
    char *path;
    int max_depth;
    long long min_size;
    int top;
    bool reverse;
    bool show_percent;
    bool show_summary;
} Options;

// 통계용
// int total_dirs = 0;
// int toal_files = 0;


// 노드 생성
TreeNode* create_node(const char *name, const char *path, long long size, bool is_dir) {
    TreeNode *node = malloc(sizeof(TreeNode));
    if (!node) return NULL;

    node->name = strdup(name);
    node->path = strdup(path);
    node->size = size;
    node->is_dir = is_dir;
    node->children = NULL;
    node->child_count = 0;
    node->capacity = 0;
    
    // if (is_dir) {
       // total_dirs++;
    // } else {
       // total_files+;
    // }

    return node;

}

// 자식 추가
void add_child(TreeNode *parent, TreeNode *child) {
    if (parent->child_count == parent->capacity) {
       int new_cap = (parent->capacity == 0) ? 4 : parent->capacity *2;

       TreeNode **tmp = realloc(parent->children, new_cap * sizeof(TreeNode*));
       if (!tmp) return;

       parent->children = tmp;
       parent->capacity = new_cap;
   }

   parent->children[parent->child_count] = child;
   parent->child_count++;
}

// 정렬용 비교
static int cmp_nodes(const void *a, const void *b) {
   TreeNode *A = *(TreeNode**)a;
   TreeNode *B = *(TreeNode**)b;

   if (A->size > B->size) return -1;
   if (A->size < B->size) return 1;
   
   return strcmp(A->name, B->name);
}

// reverse 정렬
static int cmp_nodes_rev(const void *a, const void *b) {
   return cmp_nodes(b, a);
}

// 트리 정렬
void sort_tree(TreeNode *node, bool reverse) {
   if (node->child_count > 0) {
       qsort(node->children, node->child_count, sizeof(TreeNode*), reverse ? cmp_nodes_rev : cmp_nodes);
   }
   
   for (int i = 0; i < node->child_count; i++) {
       sort_tree(node->children[i], reverse);
   }
}

void free_tree(TreeNode *node) {
    if (!node) return;

    for (int i = 0; i < node->child_count; i++) {
         free_tree(node->children[i]);
    }
    
    free(node->children);
    free(node->name);
    free(node->path);
    free(node);
}



// 디렉터리 크기 계산
long long calculate_dir_size(TreeNode *node) {
    if (!node->is_dir) {
       return node->size;
    }
    long long total = 0;
    for (int i = 0; i < node->child_count; i++) {
        total = total + calculate_dir_size(node->children[i]);
    }
    node->size = total;
    return total;
}

// 디렉터리 스캔
void scan_directory(const char *dir_path, TreeNode *parent, int current_depth, int max_depth) {
    printf("dir_path: %s\n", dir_path);

    if (max_depth != -1 && current_depth >= max_depth) {
        return;
    }

    DIR *dir = opendir(dir_path);
    if (!dir) {
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == -1) {
            continue;
        }

        bool is_directory = S_ISDIR(st.st_mode);
        long long size = is_directory ? 0 : st.st_size;

        TreeNode *child = create_node(entry->d_name, full_path, size, is_directory);
        add_child(parent, child);

        if (is_directory) {
            scan_directory(full_path, child, current_depth + 1, max_depth);    
        }
    }
    closedir(dir);
}

// 크기 포맷팅
void format_size(long long bytes, char *buf, size_t bufsize)
{
    double v = bytes;
    const char *unit = "B";

    if (bytes >= 1024) {
        v = bytes / 1024.0;
        unit = "KB";
    }
    if (bytes >= 1024 * 1024) {
        v = bytes / (1024.0 * 1024);
        unit = "MB";
    }
    if (bytes >= 1024LL * 1024 * 1024) {
        v = bytes / (1024.0 * 1024 * 1024);
        unit = "GB";
    }

    snprintf(buf, bufsize, "%.1f %s", v, unit);
}

// 트리 출력
void print_tree(TreeNode *node, int depth, const char *prefix, bool is_last, long long min_size)
{
    if (!node) return;
    if (node->size < min_size) return;

    char s[64];
    format_size(node->size, s, sizeof(s));

    if (depth == 0) {
        printf("[%s] %s%s\n", s, node->name, node->is_dir ? "/" : "");
    } else {
        printf("%s%s[%s] %s%s\n",
               prefix,
               is_last ? "└── " : "├── ",
               s,
               node->name,
               node->is_dir ? "/" : "");
    }

    if (node->child_count == 0) return;

    char next[1024];
    strcpy(next, prefix);
    strcat(next, is_last ? "    " : "│   ");

    for (int i = 0; i < node->child_count; i++) {
        int last = (i == node->child_count - 1);
        print_tree(node->children[i], depth + 1, next, last, min_size);
    }
}


// valid path
bool is_valid_path(const char *path) {
    if (path == NULL) return false;
    return access(path, F_OK) == 0;
}

// min size str to number
long long str_to_num(const char *str) {
    long long result = 0;
    int i = 0;
    while (str[i] != '\0') {
      if (str[i] >= '0' && str[i] <= '9') {
      result = result * 10 + (str[i] - '0');
      }
      i++;
    }
    return result;
}

void parse_args(int argc, char *argv[], Options *options) {
    options->path = ".";
    options->max_depth = -1;
    options->min_size = 0;
    options->top = 0;
    options->reverse = false;
    options->show_percent = false;
    options->show_summary = false;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--max-depth=", 12) == 0) {
            options->max_depth = atoi(argv[i] + 12);
        } else if (strncmp(argv[i], "--min-size=", 11) == 0) {
            options->min_size = str_to_num(argv[i] + 11);
        } else if (strncmp(argv[i], "--top=", 6) == 0) {
            options->top = atoi(argv[i] + 6);
        } else if (strcmp(argv[i], "--reverse") == 0) {
            options->reverse = true;
        } else if (strcmp(argv[i], "--percent") == 0) {
            options->show_percent = true;
        } else if (strcmp(argv[i], "--summary") == 0) {
            options->show_summary = true;
        } else if (argv[i][0] != '-') {
            options->path = argv[i];
        }
    }
}

int main(int argc, char *argv[]) {

    printf("안녕하세요. dut-DiskUsageTree입니다.\n");
    printf("Arguments count: %d개\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    Options options;
    parse_args(argc, argv, &options);

    printf("Path: %s\n", options.path);
    
    if (!is_valid_path(options.path)) {
        printf("오류: 경로를 찾을 수 없습니다: %s\n", options.path);
        return 1;
    }

    //printf("Max depth: %d\n", options.max_depth);
    if (options.max_depth != -1) {
        printf("최대 깊이: %d\n", options.max_depth);
    }

    //printf("Min size: %lld\n", options.min_size);
    if (options.min_size > 0) {
        char size_str[64];
        format_size(options.min_size, size_str, sizeof(size_str));
        printf("최소 크기: %s\n", size_str);
    }
    printf("\n");

    printf("Top: %d\n", options.top);
    printf("Reverse: %s\n", options.reverse ? "true":"false");
    printf("Percent: %s\n", options.show_percent ? "true" : "false");
    printf("Summary: %s\n", options.show_summary ? "true" : "false");

  
  char *dir_name = strrchr(options.path, '/');
  if (dir_name) {
      dir_name++; 
  } else {
      dir_name = options.path;
  }

  TreeNode *root = create_node(dir_name, options.path, 0, true);
  scan_directory(options.path, root, 0, options.max_depth);
  calculate_dir_size(root);
  sort_tree(root, options.reverse);
  print_tree(root, 0, "", true, options.min_size);
  free_tree(root);

  return 0;
}

