#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SYS_INFO(msg)                                                          \
  fprintf(stdout, "File:%s, func:%s, Line:%d, errno:%d, errMSg:%s, Msg:%s\n",  \
          __FILE__, __func__, __LINE__, errno, strerror(errno), msg)

typedef int key_t;
typedef int data_t;

typedef enum color_t { RED = 0, BLACK = 1 } color_t;

typedef struct rb_tree_node_t {
  key_t key;
  data_t data;
  color_t color;
  struct rb_tree_node_t *parent;
  struct rb_tree_node_t *left;
  struct rb_tree_node_t *right;

} rb_tree_node_t;

int rb_tree_left_rotate(rb_tree_node_t **root, rb_tree_node_t *z) {
  rb_tree_node_t *w = z->right;
  if (w == NULL) {
    SYS_INFO("right is NULL");
    return -1;
  }
  z->right = w->left;
  if (w->left) {
    w->left->parent = z;
  }
  w->parent = z->parent;
  if (z == *root) {
    *root = w;
  } else {
    if (z->parent->left == z) {
      z->parent->left = w;
    } else {
      z->parent->right = w;
    }
  }
  z->parent = w;
  w->left = z;
  return 0;
}

int rb_tree_right_rotate(rb_tree_node_t **root, rb_tree_node_t *z) {
  rb_tree_node_t *w = z->left;
  if (w == NULL) {
    SYS_INFO("left is NULL");
    return -1;
  }
  z->left = w->right;
  if (w->right) {
    w->right->parent = z;
  }
  w->parent = z->parent;
  if (z == *root) {
    (*root) = w;
  } else {
    if (z->parent->left == z) {
      z->parent->left = w;
    } else {
      z->parent->right = w;
    }
  }
  w->right = z;
  z->parent = w;
  return 0;
}

rb_tree_node_t *new_node(key_t key, data_t data) {
  rb_tree_node_t *node = (rb_tree_node_t *)malloc(sizeof(rb_tree_node_t));
  if (node == NULL) {
    SYS_INFO("create new node error");
    return NULL;
  }
  node->key = key;
  node->data = data;
  node->color = RED;
  node->parent = NULL;
  node->left = NULL;
  node->right = NULL;
  return node;
}

void rb_tree_fixup(rb_tree_node_t **root, rb_tree_node_t *z) {
  while (z != *root && z->parent->color == RED) {
    if (z->parent == z->parent->parent->left) { // 父母是左孩子
      rb_tree_node_t *s = z->parent->parent->right;
      if (s && s->color == RED) { // 伯父是红节点
        z->parent->color = BLACK;
        s->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent;       // 上移红节点
      } else {                       // 伯父是黑节点
        if (z->parent->right == z) { // z节点是右节点
          // rb_tree_left_rotate(root, z->parent);
          // z = z->left;
          z = z->parent;
          rb_tree_left_rotate(root, z);
        }
        //  z节点是左子节点s
        z->parent->parent->color = RED;
        z->parent->color = BLACK;
        rb_tree_right_rotate(root, z->parent->parent);
      }
    } else {
      // 父母是右孩子
      rb_tree_node_t *s = z->parent->parent->left; // 伯父节点
      if (s && s->color == RED) {                  // 伯父是红节点
        z->parent->color = BLACK;
        s->color = BLACK;
        z->parent->parent->color = RED;
        z = z->parent->parent; // 上移红节点
      } else {
        if (z == z->parent->left) {
          // rb_tree_right_rotate(root, z->parent);
          // z = z->right;
          z = z->parent;
          rb_tree_right_rotate(root, z);
        }
        z->parent->parent->color = RED;
        z->parent->color = BLACK;
        rb_tree_left_rotate(root, z->parent->parent);
      }
    }
  }
  (*root)->color = BLACK;
}

rb_tree_node_t *rb_insert(rb_tree_node_t **root, key_t key, data_t data) {
  rb_tree_node_t *node = new_node(key, data);
  if (node == NULL) {
    SYS_INFO("insert error");
    return NULL;
  }
  if (*root == NULL) {
    *root = node;
    (*root)->color = BLACK;
    return node;
  }
  rb_tree_node_t *y = *root;
  rb_tree_node_t *p = NULL;
  while (y != NULL) {
    p = y;
    if (key < y->key) {
      y = y->left;
    } else {
      y = y->right;
    }
  }
  y = p;
  if (key == y->key) {
    free(node);
    return y;
  } else if (key < y->key) {
    y->left = node;
    node->parent = y;
  } else {
    y->right = node;
    node->parent = y;
  }
  if (y->color == RED) {
    rb_tree_fixup(root, node);
  }
  return node;
}

rb_tree_node_t *rb_search(rb_tree_node_t *root, key_t key) {
  if (root == NULL) {
    return NULL;
  }
  rb_tree_node_t *y = root;
  while (y != NULL) {
    if (key == y->key) {
      return y;
    } else if (key < y->key) {
      y = y->left;
    } else {
      y = y->right;
    }
  }
  return NULL;
}

// 找出后继节点
rb_tree_node_t *rb_successor(rb_tree_node_t *root, rb_tree_node_t *z) {
  rb_tree_node_t *y = NULL;
  rb_tree_node_t *x = NULL;

  if (root == NULL) {
    return NULL;
  }
  if (z == NULL) {
    return NULL;
  }
  if (z->right) {
    y = z->right;
    while (y->left) {
      y = y->left;
    }
    return y;
  }
  x = z;
  if (x == root) {
    return NULL;
  }
  y = x->parent;
  while (x == y->right) {
    x = y;
    y = y->parent;
    if (y == NULL) {
      return NULL;
    }
  }
  return y;
}

void rb_tree_fixup_erase(rb_tree_node_t *root, rb_tree_node_t *x,
                         rb_tree_node_t *x_parent) {
  while (x != root && (x == NULL || x->color == BLACK)) {
    if (x == x_parent->left) {             // x是左节点
      rb_tree_node_t *w = x_parent->right; // w 是右孩子
      if (w->color == RED) {               // 兄弟是红色节点,case1
        x_parent->color = RED;
        w->color = BLACK;
        if (rb_tree_left_rotate(&root, x_parent) != 0) {
            SYS_INFO("rb_tree_left_rotate error");
            exit(-1);
        }
        w = x_parent->right;
      } // 兄弟为黑节点，而且兄弟的孩子都黑节点
      if ((w->left == NULL || w->left->color == BLACK) &&
          (w->right == NULL || w->right->color == BLACK)) {
        w->color = RED;
        x = x_parent; // x节点上移一层
        x_parent = x_parent->parent;
      } else {
        // 兄弟为黑节点，兄弟的右孩子为黑节点，兄弟左孩子为红节点
        if (w->right == NULL || w->right->color == BLACK) {
          w->color = RED;
          if (w->left)
            w->left->color = BLACK;
          if (rb_tree_right_rotate(&root, w) != 0) {
                SYS_INFO("rb_tree_right_rotate error");
                exit(-1);
          }
          w = x_parent->right;
        }
        // 兄弟为黑节点，兄弟的右孩子为红，左孩子任意
        w->color = x_parent->color;
        x_parent->color = BLACK;
        if (w->right)
          w->right->color = BLACK;
        if (rb_tree_left_rotate(&root, x_parent) != 0) {
                SYS_INFO("rb_tree_left_rotate error");
                exit(-1);
        }
        break;
      }

    } else {                              // x是右孩子
      rb_tree_node_t *w = x_parent->left; // w是左孩子
      if (w->color == RED) {              // 兄弟节点是红色，case1
        x_parent->color = RED;
        w->color = BLACK;
        if (rb_tree_right_rotate(&root, x_parent) != 0) {
            SYS_INFO("rb_tree_right_rotate error");
            exit(-1);
        }
        w = x_parent->left;
      }
      // 兄弟是黑节点，而且兄弟的孩子也都是黑节点
      if ((w->left == NULL || w->left->color == BLACK) &&
          (w->right == NULL || w->right->color == BLACK)) {
        w->color = RED;
        x = x_parent;
        x_parent = x_parent->parent;
      } else {
        // 兄弟为黑节点，兄弟的左孩子为黑节点，右孩子为红节点
        if (w->left == NULL || w->left->color == BLACK) {
          w->color = RED;
          if (w->right)
            w->right->color = BLACK;
          if (rb_tree_left_rotate(&root, w) != 0) {
                SYS_INFO("rb_tree_left_rotate error");
                exit(-1);
          }
          w = x_parent->left;
        }
        // 兄弟为黑节点，兄弟的左孩子为红，右孩子任意
        w->color = x_parent->color;
        x_parent->color = BLACK;
        if (w->left)
          w->left->color = BLACK;
        if (rb_tree_right_rotate(&root, x_parent) != 0) {
            SYS_INFO("rb_tree_right_rotate error");
            exit(-1);
        }
      }
    }
  }
  if (x) x->color = BLACK;
}

rb_tree_node_t *rb_tree_delete_node(rb_tree_node_t **root, rb_tree_node_t *z) {
  rb_tree_node_t *x;
  rb_tree_node_t *y;
  rb_tree_node_t *x_parent;
  if (z->left == NULL || z->right == NULL) {
    y = z;
  } else {
    y = rb_successor(*root, z);
  }
  x_parent = y->parent;
  if (y->left) {
    x = y->left;
  } else {
    x = y->right;
  }
  if (x) {
    x->parent = y->parent;
  }
  if (y == *root) {
    *root = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }
  if (y != z) {
    z->key = y->key;
    z->data = y->data;
  }
  if (y->color == BLACK) {
    rb_tree_fixup_erase(*root, x, x_parent);
  }
  return y;
}

int rb_tree_delete(rb_tree_node_t **root, key_t key) {
  rb_tree_node_t *y = rb_search(*root, key);
  if (y == NULL) {
    return -1;
  }
  rb_tree_delete_node(root, y);
  return 0;
}

int main() {
  int i, count = 100;
  key_t key;
  rb_tree_node_t *root = NULL;
  srand(time(NULL));

  for (i = 1; i < count; i++) {
    key = rand() % count;
    if (rb_insert(&root, key, i) != NULL) {
      printf("i=%d insert key %d success!\n", i, key);
    } else {
      printf("i=%d insert key %d error!\n", i, key);
      exit(-1);
    }
    if (rb_search(root, key) != NULL) {
      printf("find key:%d\n", key);
    } else {
      printf("can not found key:%d\n", key);
    }

    if (!(i % 10)) {
      if (rb_tree_delete(&root, key) == 0) {
        printf("delete key:%d success\n", key);
      } else {
        printf("delete key:%d failed\n", key);
      }
    }
  }
}
