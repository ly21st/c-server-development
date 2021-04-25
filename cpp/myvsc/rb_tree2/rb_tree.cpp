#include <stdio.h>
#include <stdlib.h>

typedef int key_t;
typedef int data_t;
typedef int color_t;

#define RED 1
#define BLACK 0

#define PRINT_INFO(msg) fprintf(stdout, "file:%s,func:%s,line:%d, msg:%s\n", __FILE__， __func__, __LINE__, msg)

typedef struct rb_tree_node_t {
    key_t key;
    data_t data;
    rb_tree_node_t *parent;
    rb_tree_node_t *left;
    rb_tree_node_t *right;
    color_t color;
} rb_tree_node;

typedef struct rb_tree_t {
    rb_tree_node  *root;
    rb_tree_node *sentinel;
} rb_tree;

int rb_tree_init(rb_tree *tree) {
    rb_tree_node *node = (rb_tree_node*)malloc(sizeof(rb_tree_node));
    if (node == nullptr) {
        return -1;
    }

    node->left = node->right = node->parent = nullptr;
    node->color = BLACK;
    tree->root = tree->sentinel = node;
    return 0;
}

int rb_tree_left_rotate(rb_tree_node **root, rb_tree_node* node, rb_tree_node *sentinel) {
    rb_tree_node *w = node->right;
    node->right = w->left;
    if (w->left != sentinel) {
        w->left->parent = node;
    }
    w->parent = node->parent;
    if (node == *root) {
        *root = w;
    } else {
        if (node == node->parent->left) {
            node->parent->left = w;
        } else {
            node->parent->right = w;
        }
    }
    w->left = node;
    node->parent = w;
}

int rb_tree_right_rotate(rb_tree_node **root, rb_tree_node *node, rb_tree_node *sentinel) {
    rb_tree_node *w = node->left;
    node->left = w->right;
    if (w->right != sentinel ) {
        w->right->parent = node;
    }
    w->parent = node->parent;
    if (node  == *root) {
        *root = w;
    } else {
        if (node == node->parent->left) {
            node->parent->left = w;
        } else {
            node->parent->right = w;
        }
    }
    w->right = node;
    node->parent = w;
}


int main() {

}


