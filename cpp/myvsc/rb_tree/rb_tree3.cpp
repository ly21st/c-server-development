#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum color_t
{
    RED = 0,
    BLACK=1
}color_t;
typedef int key_t ;
typedef int data_t;
typedef struct rb_tree_node
{
    struct rb_tree_node *left;
    struct rb_tree_node *right;
    struct rb_tree_node *parent;
    key_t key;
    data_t data;
    color_t color;
}rb_tree_node;

static rb_tree_node* grandparent(rb_tree_node *n);
static rb_tree_node* uncle(rb_tree_node* n);
static rb_tree_node* rb_new_node();
static rb_tree_node* rb_init(rb_tree_node **T);
static rb_tree_node* rb_search_aux(rb_tree_node *T, key_t key,rb_tree_node **parent);
static rb_tree_node* rb_search(rb_tree_node *T, key_t key);
static rb_tree_node* rb_successor( rb_tree_node*T ,rb_tree_node *node);
static rb_tree_node* rb_insert(rb_tree_node *T, key_t key, data_t data );
static void rb_insert_fixup(rb_tree_node *T, rb_tree_node *n);
static void rb_left_rotate(rb_tree_node *T, rb_tree_node *x);
static void rb_right_rotate(rb_tree_node *T, rb_tree_node *x);
static void insert_case1(rb_tree_node *T,rb_tree_node *n);
static void insert_case2(rb_tree_node *T,rb_tree_node *n);
static void insert_case3(rb_tree_node *T,rb_tree_node *n);
static void insert_case4(rb_tree_node *T,rb_tree_node *n);
static void insert_case5(rb_tree_node *T,rb_tree_node *n);
static rb_tree_node* sibling(rb_tree_node *n);
static void rb_delete_fixup(rb_tree_node *T,rb_tree_node *parent, rb_tree_node *n);
static void delete_one_child(rb_tree_node *T, rb_tree_node *parent,rb_tree_node *n);
static void delete_case1(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n);
static void delete_case2(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n);
static void delete_case3(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n);
static void delete_case4(rb_tree_node *T,rb_tree_node *parent, rb_tree_node *n);
static void delete_case5(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n);
static void delete_case6(rb_tree_node *T,rb_tree_node *parent, rb_tree_node *n);


static rb_tree_node* grandparent(rb_tree_node *n)
{
    return n->parent->parent;
}

static rb_tree_node* uncle(rb_tree_node* n)
{
    if ( n->parent == grandparent(n)->left )
        return grandparent(n)->right;
    else
        return grandparent(n)->left;
}

static rb_tree_node* rb_init(rb_tree_node **T)
{
    if ( T == NULL )
    {
        return (rb_tree_node*)NULL;
    }
    *T = rb_new_node();
    if ( (*T) == (rb_tree_node*)NULL )
        return (rb_tree_node*)NULL;
    memset(*T,0, sizeof(*T));
    (*T)->left = (*T)->right = NULL;
    (*T)->parent = NULL;
    (*T)->color = RED;
    return *T;
}

static rb_tree_node* rb_new_node()
{
    rb_tree_node *node = NULL;
    if ( node = (rb_tree_node*)malloc(sizeof(rb_tree_node)) )
    {
        return node;
    }
    return (rb_tree_node*)NULL;
}

static rb_tree_node* rb_search_aux(rb_tree_node *T, key_t key,rb_tree_node **parent)
{
    rb_tree_node *root;
    rb_tree_node *result = NULL;
    rb_tree_node *index = NULL;

    if ( T == NULL )
        return (rb_tree_node*)NULL;
    root = T->parent;
    result = root;
    while ( result != NULL )
    {
        index = result;
        if ( key < result->key )
            result = result->left;
        else if ( key > result->key)
            result = result->right;
        else
            break;
    }
    if ( parent )
        *parent = index;
    return result;
}

static rb_tree_node* rb_search(rb_tree_node *T, key_t key)
{
    return rb_search_aux(T,key,NULL);
}

static rb_tree_node* rb_successor( rb_tree_node *T ,rb_tree_node *node)
{
    rb_tree_node *parent = NULL,*result = NULL;

    if ( node == NULL )
        return NULL;
    result = node->right;
    while ( result != NULL )
    {
        if ( result->left != NULL )
            result = result->left;
        else
            break;
    }
    if ( result == NULL )
    {
        parent = node->parent;

        while( parent != T &&  node == parent->right )
        {
            node = parent;
            parent = parent->parent;
        }
        result = parent;
    }
    return result;
}

static rb_tree_node* rb_insert(rb_tree_node *T, key_t key, data_t data )
{
    rb_tree_node *root;
    rb_tree_node *z;
    rb_tree_node *y;
    rb_tree_node *x;

    if ( T == NULL )
        return NULL;
    root = T->parent;
    if ( (z=rb_search(T,key)) )
        return z;
    z = rb_new_node();
    if ( z == NULL )
        return NULL;
    z->key = key;
    z->data = data;

    y=T;
    x = root;
    while ( x != NULL )
    {
        y = x;
        if ( z->key < x->key )
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if ( y == T )
    {
        root = z;
        T->parent = root;
    }else
    {
        if ( z->key < y->key )
            y->left = z;
        else
            y->right = z;
    }
    z->left = z->right = NULL;
    z->color = RED;
    rb_insert_fixup(T,z);
    return z;
}

static void rb_insert_fixup(rb_tree_node *T, rb_tree_node *n)
{
    if ( T== NULL )
        return ;
    insert_case1(T,n);
}

static void insert_case1(rb_tree_node *T,rb_tree_node *n)
{
    if ( T== NULL )
        return ;
    if ( n->parent == T)
        n->color = BLACK;
    else
        insert_case2(T,n);
}

static void insert_case2(rb_tree_node *T,rb_tree_node *n)
{
    if ( T== NULL )
        return;
    if ( n->parent->color == BLACK )
        return;
    else
        insert_case3(T,n);
}

static void insert_case3(rb_tree_node *T, rb_tree_node *n)
{
    if ( T== NULL )
        return ;
    if ( uncle(n) != NULL && uncle(n)->color == RED )
    {
        n->parent->color = BLACK;
        uncle(n)->color = BLACK;
        grandparent(n)->color = RED;
        insert_case1(T,grandparent(n));
    }else
    {
        insert_case4(T,n);
    }
}

static void insert_case4(rb_tree_node *T, rb_tree_node* n)
{
    if ( T== NULL  )
        return ;
    if ( n== n->parent->right && n->parent == grandparent(n)->left )
    {
        rb_left_rotate(T,n->parent);
        n= n->left;
    }else if ( n == n->parent->left && n->parent==grandparent(n)->right )
    {
        rb_right_rotate(T,n->parent);
        n = n->right;
    }
    insert_case5(T,n);
}

static void insert_case5(rb_tree_node *T, rb_tree_node *n)
{
    n->parent->color = BLACK;
    grandparent(n)->color= RED;
    if ( n==n->parent->left && n->parent == grandparent(n)->left )
    {
        rb_right_rotate(T,grandparent(n));
    }else
    {
        rb_left_rotate(T,grandparent(n));
    }
}

static void rb_left_rotate(rb_tree_node *T, rb_tree_node *x)
{
    rb_tree_node *y;
    rb_tree_node *root;

    if ( T== NULL || x == NULL )
        return;
    if ( x->right == NULL )
        return;
    root = T->parent;
    y = x->right;
    x->right = y->left;
    if ( y->left != NULL )
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == T )
    {
        root = y;
        T->parent = root;
    }else
    {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void rb_right_rotate(rb_tree_node *T, rb_tree_node *x)
{
    rb_tree_node *root;
    rb_tree_node *y;

    if ( T == NULL || x == NULL )
    {
        return ;
    }
    if ( x->left == NULL )
        return;
    root = T->parent;
    y = x->left;
    x->left = y->right;
    if ( y->right )
        y->right->parent = x;
    y->parent = x->parent;
    if ( x->parent == T )
    {
        root = y;
        T->parent = root;
    }else
    {
        if ( x == x->parent->left )
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

static rb_tree_node* sibling(rb_tree_node *n)
{
    if ( n== n->parent->left )
        return n->parent->right;
    else
        return n->parent->left;
}

static rb_tree_node* rb_delete(rb_tree_node *T, key_t key)
{
    rb_tree_node *root;
    rb_tree_node *z;
    rb_tree_node *y;
    rb_tree_node *x;

    if ( T == NULL )
        return NULL;
    root = T->parent;
    if ( (z=rb_search(T,key)) == NULL )
    {
        printf("the key is not exist\n");
        return NULL;
    }
    if ( (z->left == NULL) || (z->right == NULL ) )
        y = z;
    else
        y = rb_successor(T,z);
    if ( y->left !=  NULL )
        x = y->left;
    else
        x = y->right;
    if (x)
        x->parent = y->parent;
    if (y->parent == T)
    {
        root = x;
        T->parent = root;
    }else
    {
        if ( y == y->parent->left )
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
        rb_delete_fixup(T,y->parent,x);
    free(y);
    return T;
}

static void rb_delete_fixup(rb_tree_node *T,rb_tree_node* parent, rb_tree_node *n)
{
    if ( T== NULL )
        return ;
    delete_one_child(T,parent,n);
}

static void delete_one_child(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n)
{
    if ( !n )
        delete_case1(T,parent,n);
    else
    {
        if (n->color == RED )
            n->color = BLACK;
        else
            delete_case1(T,parent,n);
    }
}

static void delete_case1(rb_tree_node *T,rb_tree_node *parent,rb_tree_node  *n)
{
    if ( parent != T )
        delete_case2(T,parent,n);
}

static void delete_case2(rb_tree_node *T,rb_tree_node *parent,rb_tree_node *n)
{
    rb_tree_node *s;

    if (T==NULL  )
        return;
    if ( !n )
        delete_case3(T,parent,n);
    else
    {
        s = sibling(n);
        if ( s->color == RED )
        {
            n->parent->color = RED;
            s->color = BLACK;
            if ( n == n->parent->left )
                rb_left_rotate(T,n->parent);
            else
                rb_right_rotate(T,n->parent);
        }
        delete_case3(T,parent,n);
    }
}

static void delete_case3(rb_tree_node *T, rb_tree_node *parent,rb_tree_node *n)
{
    rb_tree_node *s;
    if ( T== NULL )
        return ;
    if ( n == parent->left )
        s = parent->right;
    else
        s = parent->left;
    if ( parent->color == BLACK &&
         s->color == BLACK &&
         ( !s->left || s->left->color == BLACK)  &&
         ( !s->right || s->right->color == BLACK ) )
    {
        s->color = RED;
        delete_case1(T,parent->parent,parent);
    }else
        delete_case4(T,parent,n);
}

static void delete_case4(rb_tree_node *T,rb_tree_node *parent, rb_tree_node *n)
{
    rb_tree_node *s;

    if ( T== NULL )
        return ;
    if ( n== parent->left )
        s = parent->right;
    else
        s = parent->left;
    if ( parent->color == RED &&
         s->color == BLACK &&
         ( !s->left || s->left->color == BLACK) &&
         ( !s->right || s->right->color == BLACK) )
    {
        s->color = RED;
        parent->color = BLACK;
    }else
        delete_case5(T,parent,n);
}

static void delete_case5(rb_tree_node *T,rb_tree_node *parent, rb_tree_node *n )
{
    rb_tree_node *s;
    if ( T== NULL)
        return ;
    if ( n== parent->left )
        s = parent->right;
    else
        s = parent->left;
    if ( s->color == BLACK )
    {
        if ( n==parent->left &&
           (s->left && s->left->color== RED) &&
           ( !s->right || s->right->color == BLACK)  )
        {
            s->left->color = BLACK;
            s->color= RED;
            rb_right_rotate(T,s);
        }else if( n== parent->right &&
                  ( !s->left || s->left->color == BLACK) &&
                  s->right->color == RED )
        {
            s->right->color = BLACK;
            s->color = RED;
            rb_left_rotate(T,s);
        }
    }
    delete_case6(T,parent,n);
}

static void delete_case6(rb_tree_node *T, rb_tree_node *parent,rb_tree_node *n)
{
    rb_tree_node *s;
    if ( T== NULL  )
        return;
    if ( n== parent->left )
        s = parent->right;
    else
        s = parent->left;
    s->color = parent->color;
    parent->color = BLACK;
    if ( n == parent->left )
    {
        s->right->color= BLACK;
        rb_left_rotate(T,parent);
    }else
    {
        s->left->color = BLACK;
        rb_right_rotate(T,parent);
    }
}

//主函数
int main()
{

    int i, count = 1000;
    key_t key;
    rb_tree_node *T, *node=NULL;

    srand(time(NULL));
    rb_init(&T);


    for (i = 1; i < count; ++i)
    {
        key = rand() % count;
        if ( (node = rb_insert(T,key,i)) != NULL )
        {
            printf("[i = %d] insert key %d success!\n", i, key);
        }
        else
        {
            printf("[i = %d] insert key %d error!\n", i, key);
            exit(-1);
        }

        if ( (node = rb_search(T,key)) != NULL )
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
            if ( (node = rb_delete(T,key)) != NULL )
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

