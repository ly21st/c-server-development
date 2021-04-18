#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SYS_INFO fprintf(stdout,"文件: %s, 函数: %s, 行号: %d\n",__FILE__,__func__,__LINE__)

typedef int key_t;
typedef int data_t;
typedef enum color_t
{
    RED = 0,
    BLACK = 1
}color_t;

typedef struct rb_tree_node
{
    struct rb_tree_node *left;
    struct rb_tree_node *right;
    struct rb_tree_node *parent;
    key_t key;
    data_t data;
    color_t color;
}rb_tree_node;


static rb_tree_node* rb_init(rb_tree_node** root, rb_tree_node** null_tree_node);
static rb_tree_node* rb_new_node();
static rb_tree_node* rb_left_rotate(rb_tree_node* root, rb_tree_node *x,rb_tree_node *null_tree_node);
static rb_tree_node* rb_right_rotate(rb_tree_node* root, rb_tree_node *x,rb_tree_node *null_tree_node);
static rb_tree_node* rb_insert(rb_tree_node* root, key_t key, data_t data, rb_tree_node *null_tree_node );
static rb_tree_node* rb_insert_fixup(rb_tree_node* root, rb_tree_node* z, rb_tree_node *null_tree_node);
static rb_tree_node* rb_insert_fixup(rb_tree_node* root, rb_tree_node* z, rb_tree_node *null_tree_node);
static rb_tree_node* rb_delete(rb_tree_node *root, key_t key,rb_tree_node *null_tree_node );
static rb_tree_node* rb_delete_fixup(rb_tree_node *root, rb_tree_node *x, rb_tree_node *null_tree_node );
static rb_tree_node* rb_search_auxiliary(rb_tree_node* root,key_t key, rb_tree_node *null_tree_node, rb_tree_node** save);
static rb_tree_node* rb_search(rb_tree_node* root, key_t key, rb_tree_node *null_tree_node);
static void rb_clear(rb_tree_node *root, rb_tree_node *null_tree_node );
static void rb_clear_destroy(rb_tree_node *root, rb_tree_node *null_tree_node);


static rb_tree_node* rb_init(rb_tree_node** root, rb_tree_node** null_tree_node)
{
    if (root == NULL || null_tree_node == NULL )
    {
        return (rb_tree_node*)NULL;
    }
    *null_tree_node = rb_new_node();
    *root = *null_tree_node;
    (*null_tree_node)->color = BLACK;
    (*null_tree_node)->left = (*null_tree_node)->right = (*null_tree_node)->parent = *root;
    return *root;
}

static rb_tree_node* rb_new_node()
{
    rb_tree_node* newnode = (rb_tree_node*)malloc(sizeof(rb_tree_node));
    if ( newnode == NULL )
    {
        SYS_INFO;
        perror("rb_new_node");
        return (rb_tree_node*)NULL;
    }
    return newnode;
}

static rb_tree_node* rb_left_rotate(rb_tree_node* root, rb_tree_node *x,rb_tree_node *null_tree_node)
{
    rb_tree_node *right;
    if ( (right = x->right) == null_tree_node)
        return null_tree_node;
    if ( (x->right = right->left) != null_tree_node )
        right->left->parent = x;
    right->parent = x->parent;
    if ( x->parent == null_tree_node )
    {
        root = right;
        null_tree_node->left=null_tree_node->right=null_tree_node->parent= root;
    }else if ( x == x->parent->left )
    {
        x->parent->left = right;
    }else
    {
        x->parent->right = right;
    }
    x->parent = right;
    right->left = x;
    return root;
}

static rb_tree_node* rb_right_rotate(rb_tree_node* root, rb_tree_node *x,rb_tree_node *null_tree_node)
{
    rb_tree_node *left;
    if ( (left = x->left) == null_tree_node )
    {
        return null_tree_node;
    }
    if ( ( x->left = left->right ) != null_tree_node )
    {
        left->right->parent = x;
    }
    left->parent = x->parent;
    if ( x->parent == null_tree_node )
    {
        root = left;
        null_tree_node->left=null_tree_node->right=null_tree_node->parent= root;
    }else if ( x == x->parent->left )
    {
        x->parent->left = left;
    }else
    {
        x->parent->right = left;
    }
    x->parent = left;
    left->right = x;
    return root;
}

static rb_tree_node* rb_insert(rb_tree_node* root, key_t key, data_t data, rb_tree_node *null_tree_node )
{
    rb_tree_node *y;
    rb_tree_node *x;
    rb_tree_node *z;

    if ( (z=rb_search(root,key,null_tree_node)) != null_tree_node )
    {
        return root;
    }

    y = null_tree_node;
    x = root;
    z = rb_new_node();
    z->key = key;
    z->data = data;

    while ( x != null_tree_node )
    {
        y = x;
        if ( z->key < x->key )
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == null_tree_node)
        root = z;
    else{
        if ( z->key < y->key )
            y->left = z;
        else
            y->right = z;
    }
    z->left=z->right=null_tree_node;
    z->color = RED;
    return rb_insert_fixup(root,z,null_tree_node);
}

static rb_tree_node* rb_insert_fixup(rb_tree_node* root, rb_tree_node* z, rb_tree_node *null_tree_node)
{
    while( z->parent->color == RED )
    {
        if ( z->parent == z->parent->parent->left )
        {
            rb_tree_node* y;
            y = z->parent->parent->right;
            //case 1
            if ( y->color == RED )
            {
                y->color = BLACK;
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }else   // case 2 , case 3
            {
                if ( z == z->parent->right )
                {
                    z = z->parent;
                    root = rb_left_rotate(root,z,null_tree_node);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                root = rb_right_rotate(root,z->parent->parent,null_tree_node);
            }
        }
        else
        {
            if ( z->parent == z->parent->parent->right )
            {
                rb_tree_node *y;
                y = z->parent->parent->left;
                if ( y->color == RED )
                {
                    y->color = BLACK;
                    z->parent->color = BLACK;
                    z->parent->parent->color=RED;
                    z = z->parent->parent;
                }else
                {
                    if ( z == z->parent->left )
                    {
                        z=z->parent;
                        root = rb_right_rotate(root,z,null_tree_node);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    root = rb_left_rotate(root,z->parent->parent,null_tree_node);
                }
            }
        }
    }
    root->color = BLACK;
    return root;
}

static rb_tree_node* rb_successor(rb_tree_node *root, rb_tree_node *z, rb_tree_node *null_tree_node)
{
    rb_tree_node *result;
    if ( z == null_tree_node )
        return null_tree_node;
    result = z->right;
    while ( result != null_tree_node )
    {
        if ( result->left != null_tree_node )
            result = result->left;
        else
            break;
    }
    if ( result == null_tree_node )
    {
        rb_tree_node *index;
        index = z->parent;
        result = z;
        while ( index != null_tree_node && result == index->right )
        {
            result = index;
            index = index->parent;
        }
        result = index;
    }
    return result;
}

static rb_tree_node* rb_delete(rb_tree_node *root, key_t key,rb_tree_node *null_tree_node )
{
    rb_tree_node *x;
    rb_tree_node *y;
    rb_tree_node *z;

    if ( (z=rb_search(root,key,null_tree_node)) == null_tree_node )
    {
         printf("key %d is not exist!/n");
         return root;
    }

    if ( z->left == null_tree_node || z->right == null_tree_node )
        y = z;
    else
        y = rb_successor(root,z,null_tree_node);
    if ( y->left != null_tree_node )
        x = y->left;
    else
        x = y->right;
    x->parent = y->parent;
    if ( y->parent == null_tree_node )
    {
        root = x;
        null_tree_node->left=null_tree_node->right=null_tree_node->parent=root;
    }else
    {
        if ( y==y->parent->left )
            y->parent->left = x;
        else
            y->parent->right = x;
    }
    if ( y != z )
    {
        z->key = y->key;
        z->data = y->data;
    }
    if ( y->color == BLACK )
    {
        rb_delete_fixup(root,x,null_tree_node);
    }
    free(y);
    return root;
}

static rb_tree_node* rb_delete_fixup(rb_tree_node *root, rb_tree_node *x, rb_tree_node *null_tree_node )
{
    rb_tree_node *w;

    while ( x!= root && x->color == BLACK )
    {
        if ( x == x->parent->left )
        {
            w = x->parent->right;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                root = rb_left_rotate(root,x->parent,null_tree_node);
                w = x->parent->right;
            }
            if ( w->left->color == BLACK && w->right->color == BLACK )
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == BLACK)
                {
                    w->left->color = BLACK;
                    w->color = RED;
                    root = rb_right_rotate(root,w,null_tree_node);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                root = rb_left_rotate(root,x->parent,null_tree_node);
                x = root;
            }
        }else
        {
            w = x->parent->left;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                root = rb_right_rotate(root,x->parent,null_tree_node);
                w = x->parent->left;
            }
            if ( w->left->color == BLACK && w->right->color == BLACK )
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == BLACK)
                {
                    w->right->color = BLACK;
                    w->color = RED;
                    root = rb_left_rotate(root,w,null_tree_node);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                root = rb_right_rotate(root,x->parent,null_tree_node);
                x = root;
            }
        }
    }
    x->color  = BLACK;
    return root;
}

//红黑树查找结点
//----------------------------------------------------
//rb_search_auxiliary：查找
//rb_node_t* rb_search：返回找到的结点
//----------------------------------------------------
static rb_tree_node* rb_search_auxiliary(rb_tree_node* root,key_t key, rb_tree_node *null_tree_node, rb_tree_node** save)
{
    rb_tree_node *node = root, *parent = NULL;
    int ret;

    while (node != null_tree_node )
    {
        parent = node;
        ret = node->key - key;
        if (0 < ret)
        {
            node = node->left;
        }
        else if (0 > ret)
        {
            node = node->right;
        }
        else
        {
            return node;
        }
    }

    if (save)
    {
        *save = parent;
    }

    return null_tree_node;
}

//返回上述rb_search_auxiliary查找结果
static rb_tree_node* rb_search( rb_tree_node* root,key_t key, rb_tree_node *null_tree_node)
{
    return rb_search_auxiliary(root, key,null_tree_node,NULL);
}

static void rb_clear(rb_tree_node *root, rb_tree_node *null_tree_node )
{
    rb_tree_node *node =root;
    if ( node->left == null_tree_node )
        return;
    else
    {
        rb_clear(node->left,null_tree_node);
        rb_clear(node->right,null_tree_node);
        free(node);
    }
}

static void rb_clear_destroy(rb_tree_node *root, rb_tree_node *null_tree_node)
{
    rb_clear(root,null_tree_node);
    free(null_tree_node);
}


//主函数
int main()
{

    int i, count = 100;
    key_t key;
    rb_tree_node *root = NULL , *null_tree_node=NULL, *node=NULL;

    srand(time(NULL));
    root = rb_init(&root, &null_tree_node);

    for (i = 1; i < count; ++i)
    {
        key = rand() % count;
        if ( (root = rb_insert(root,key,i,null_tree_node)) != null_tree_node )
        {
            printf("[i = %d] insert key %d success!\n", i, key);
        }
        else
        {
            printf("[i = %d] insert key %d error!\n", i, key);
            exit(-1);
        }

        if ( (node = rb_search(root,key,null_tree_node)) != null_tree_node )
        {
            printf("[i = %d] search key %d success!\n", i, key);
        }
        else
        {
            printf("[i = %d] search key %d error!\n", i, key);
            exit(-1);
        }
        if (!(i % 10))
        {
            if ( (root = rb_delete(root,key,null_tree_node)) != null_tree_node )
            {
                printf("[i = %d] erase key %d success\n", i, key);
            }
            else
            {
                printf("[i = %d] erase key %d error\n", i, key);
            }
        }
    }

    return 0;
}
