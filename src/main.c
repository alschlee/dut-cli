#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "args.h"
#include "utils.h"
# include <unistd.h>

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
    printf("Valid: %s\n", is_valid_path(options.path) ? "Yes" : "No");
    printf("Max depth: %d\n", options.max_depth);
    printf("Min size: %lld\n", options.min_size);
    printf("Top: %d\n", options.top);
    printf("Reverse: %s\n", options.reverse ? "true":"false");
    printf("Percent: %s\n", options.show_percent ? "true" : "false");
    printf("Summary: %s\n", options.show_summary ? "true" : "false");

    // 예시 트리 생성
    TreeNode *root = create_node("root", options.path, 100, true);
    add_child(root, create_node("a.txt", "/a.txt", 50, false));
    add_child(root, create_node("b.txt", "/b.txt", 20, false));
    add_child(root, create_node("dir", "/dir", 200, true));



    printf("--트리 확인 테스트--\n");
    printf("Root: %s (size=%lld, is_dir=%d, children=%d)\n", root->name, root->size, root->is_dir, root->child_count);
   
   for (int i = 0; i < root->child_count; i++) {
       TreeNode *child = root->children[i];
       printf("Child %d: %s (size=%lld, is_dir=%d)\n", i, child->name, child->size, child->is_dir);
   }
   
  printf("--정렬 전 순서--\n");
  for (int i = 0; i < root->child_count; i++) {
      printf("%d. %s - %lld bbytes\n", i+1, root->children[i]->name, root->children[i]->size);
  }

  sort_tree(root, options.reverse);

  printf("\n--정렬 후 순서--\n");
  for (int i = 0; i < root->child_count; i++) {
      printf("%d. %s - %lld bytes\n", i+1, root->children[i]->name, root->children[i]->size);
  }
  
  free_tree(root);

  return 0;
}

