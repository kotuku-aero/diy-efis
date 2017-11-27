/*
diy-efis
Copyright (C) 2016 Kotuku Aerospace Limited

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

If a file does not contain a copyright header, either because it is incomplete
or a binary file then the above copyright notice will apply.

Portions of this repository may have further copyright notices that may be
identified in the respective files.  In those cases the above copyright notice and
the GPL3 are subservient to that copyright notice.

Portions of this repository contain code fragments from the following
providers.


If any file has a copyright notice or portions of code have been used
and the original copyright notice is not yet transcribed to the repository
then the origional copyright notice is to be respected.

If any material is included in the repository that is not open source
it must be removed as soon as possible after the code fragment is identified.
*/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "neutron.h"

struct _rb_tree_entry_t;

typedef struct _rb_tree_entry_t
  {
  void *key;
  void *value;
  bool red;
  struct _rb_tree_entry_t *left;
  struct _rb_tree_entry_t *right;
  struct _rb_tree_entry_t *parent;
  } rb_tree_entry_t;

typedef struct _rb_tree_t
  {
  uint16_t version;
  dup_fn copy_key;
  dup_fn copy_value;
  compare_key_fn compare;
  destroy_key_fn destroy_key;
  destroy_value_fn destroy_value;
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  rb_tree_entry_t* root;
  rb_tree_entry_t* nil;
  } rb_tree_t;

result_t map_create(dup_fn copy_key, 
                    dup_fn copy_value,
                    compare_key_fn comp_fn,
                    destroy_key_fn destroy_key,
                    destroy_value_fn destroy_value,
                    handle_t *handle)
  {
  rb_tree_t* new_tree;
  rb_tree_entry_t* temp;

  new_tree = (rb_tree_t*) neutron_malloc(sizeof (rb_tree_t));
  new_tree->copy_key = copy_key;
  new_tree->copy_value = copy_value;
  new_tree->compare = comp_fn;
  new_tree->destroy_key = destroy_key;
  new_tree->destroy_value = destroy_value;

  /*  see the comment in the rb_tree_t structure in red_black_tree.h */
  /*  for information on nil and root */
  temp = new_tree->nil = (rb_tree_entry_t*) neutron_malloc(sizeof (rb_tree_entry_t));
  temp->parent = temp->left = temp->right = temp;
  temp->red = false;;
  temp->key = 0;
  temp = new_tree->root = (rb_tree_entry_t*) neutron_malloc(sizeof (rb_tree_entry_t));
  temp->parent = temp->left = temp->right = new_tree->nil;
  temp->key = 0;
  temp->red = false;
  
  return s_ok;
  }

static void copy_str(const void *in, void **out)
  {
  *((char **)out) = neutron_strdup((const char *)in);
  }

result_t map_create_nv(dup_fn copy_fn,
                       destroy_value_fn destroy_value,
                       handle_t *handle)
  {
  return map_create(copy_str,
                    copy_fn,
                    (compare_key_fn)strcmp,
                    neutron_free,
                    destroy_value,
                    handle);
  }
/***********************************************************************/
/*  FUNCTION:  left_rotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input: tree, x */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */

/***********************************************************************/

static void left_rotate(rb_tree_t* tree, rb_tree_entry_t* x)
  {
  rb_tree_entry_t* y;
  rb_tree_entry_t* nil = tree->nil;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls left_rotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when RBDeleteFixUP */
  /*  calls left_rotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  y = x->right;
  x->right = y->left;

  if (y->left != nil)
    y->left->parent = x; /* used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */

  y->parent = x->parent;

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x;
  x->parent = y;
  }

/***********************************************************************/
/*  FUNCTION:  RighttRotate */
/**/
/*  INPUTS:  This takes a tree so that it can access the appropriate */
/*           root and nil pointers, and the node to rotate on. */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input?: tree, y */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly. */

/***********************************************************************/

static void right_rotate(rb_tree_t* tree, rb_tree_entry_t* y)
  {
  rb_tree_entry_t* x;
  rb_tree_entry_t* nil = tree->nil;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls left_rotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when RBDeleteFixUP */
  /*  calls left_rotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  x = y->left;
  y->left = x->right;

  if (nil != x->right)
    x->right->parent = y; /*used to use sentinel here */

  /* and do an unconditional assignment instead of testing for nil */

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  x->parent = y->parent;
  if (y == y->parent->left)
    y->parent->left = x;
  else
    y->parent->right = x;

  x->right = y;
  y->parent = x;
  }

/***********************************************************************/
/*  FUNCTION:  tree_insert_help  */
/**/
/*  INPUTS:  tree is the tree to insert into and z is the node to insert */
/**/
/*  OUTPUT:  none */
/**/
/*  Modifies Input:  tree, z */
/**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the RBTreeInsert function and not by the user */

/***********************************************************************/

static void tree_insert_help(rb_tree_t* tree, rb_tree_entry_t* z)
  {
  /*  This function should only be called by InsertRBTree (see above) */
  rb_tree_entry_t* x;
  rb_tree_entry_t* y;
  rb_tree_entry_t* nil = tree->nil;

  z->left = z->right = nil;
  y = tree->root;
  x = tree->root->left;
  while (x != nil)
    {
    y = x;
    if (1 == tree->compare(x->key, z->key))
      { /* x.key > z.key */
      x = x->left;
      }
    else
      { /* x,key <= z.key */
      x = x->right;
      }
    }

  z->parent = y;
  if ((y == tree->root) ||
      (1 == tree->compare(y->key, z->key)))
    { /* y.key > z.key */
    y->left = z;
    }
  else
    {
    y->right = z;
    }
  }

/***********************************************************************/
/*  FUNCTION:  tree_successor  */
/**/
/*    INPUTS:  tree is the tree in question, and x is the node we want the */
/*             the successor of. */
/**/
/*    OUTPUT:  This function returns the successor of x or NULL if no */
/*             successor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */

/***********************************************************************/

static rb_tree_entry_t* tree_successor(rb_tree_t* tree, rb_tree_entry_t* x)
  {
  rb_tree_entry_t* y;
  rb_tree_entry_t* nil = tree->nil;
  rb_tree_entry_t* root = tree->root;

  if (nil != (y = x->right))
    { /* assignment to y is intentional */
    while (y->left != nil)
      { /* returns the minium of the right subtree of x */
      y = y->left;
      }
    return (y);
    }
  else
    {
    y = x->parent;
    while (x == y->right)
      { /* sentinel used instead of checking for nil */
      x = y;
      y = y->parent;
      }
    if (y == root) return (nil);
    return (y);
    }
  }

/***********************************************************************/
/*  FUNCTION:  exact_query */
/**/
/*    INPUTS:  tree is the tree to print and q is a pointer to the key */
/*             we are searching for */
/**/
/*    OUTPUT:  returns the a node with key equal to q.  If there are */
/*             multiple nodes with key equal to q this function returns */
/*             the one highest in the tree */
/**/
/*    Modifies Input: none */
/**/

/***********************************************************************/

static rb_tree_entry_t* exact_query(rb_tree_t* tree, const void* q)
  {
  rb_tree_entry_t* x = tree->root->left;
  rb_tree_entry_t* nil = tree->nil;
  int compVal;
  if (x == nil) return (0);
  compVal = tree->compare(x->key, (int*) q);
  while (0 != compVal)
    {/*assignemnt*/
    if (1 == compVal)
      { /* x->key > q */
      x = x->left;
      }
    else
      {
      x = x->right;
      }
    if (x == nil) return (0);
    compVal = tree->compare(x->key, (int*) q);
    }
  return (x);
  }

/***********************************************************************/
/*  FUNCTION:  delete_fix_up */
/**/
/*    INPUTS:  tree is the tree to fix and x is the child of the spliced */
/*             out node in RBTreeDelete. */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
/**/
/*    Modifies Input: tree, x */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */

/***********************************************************************/

static void delete_fix_up(rb_tree_t* tree, rb_tree_entry_t* x)
  {
  rb_tree_entry_t* root = tree->root->left;
  rb_tree_entry_t* w;

  while ((!x->red) && (root != x))
    {
    if (x == x->parent->left)
      {
      w = x->parent->right;
      if (w->red)
        {
        w->red = false;;
        x->parent->red = true;;
        left_rotate(tree, x->parent);
        w = x->parent->right;
        }
      if ((!w->right->red) && (!w->left->red))
        {
        w->red = true;;
        x = x->parent;
        }
      else
        {
        if (!w->right->red)
          {
          w->left->red = false;;
          w->red = true;;
          right_rotate(tree, w);
          w = x->parent->right;
          }
        w->red = x->parent->red;
        x->parent->red = false;;
        w->right->red = false;;
        left_rotate(tree, x->parent);
        x = root; /* this is to exit while loop */
        }
      }
    else
      { /* the code below is has left and right switched from above */
      w = x->parent->left;
      if (w->red)
        {
        w->red = false;;
        x->parent->red = true;;
        right_rotate(tree, x->parent);
        w = x->parent->left;
        }
      if ((!w->right->red) && (!w->left->red))
        {
        w->red = true;;
        x = x->parent;
        }
      else
        {
        if (!w->left->red)
          {
          w->right->red = false;;
          w->red = true;;
          left_rotate(tree, w);
          w = x->parent->left;
          }
        w->red = x->parent->red;
        x->parent->red = false;;
        w->left->red = false;;
        right_rotate(tree, x->parent);
        x = root; /* this is to exit while loop */
        }
      }
    }
  x->red = false;;
  }

/***********************************************************************/
/*  FUNCTION:  tree_destroy_helper */
/**/
/*    INPUTS:  tree is the tree to destroy and x is the current node */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively destroys the nodes of the tree */
/*              postorder using the destroy_key and destroy_value functions. */
/**/
/*    Modifies Input: tree, x */
/**/
/*    Note:    This function should only be called by RBTreeDestroy */

/***********************************************************************/

static void tree_destroy_helper(rb_tree_t* tree, rb_tree_entry_t* x)
  {
  rb_tree_entry_t* nil = tree->nil;
  if (x != nil)
    {
    tree_destroy_helper(tree, x->left);
    tree_destroy_helper(tree, x->right);
    tree->destroy_key(x->key);
    tree->destroy_value(x->value);
    neutron_free(x);
    }
  }

result_t map_add(handle_t map, const void *key, const void *value)
  {
  rb_tree_entry_t * y;
  rb_tree_entry_t * x;
  rb_tree_entry_t * new_node;

  if (map == 0 ||
      key == 0 ||
      value == 0)
    return e_bad_parameter;

  rb_tree_t *tree = (rb_tree_t *) map;

  x = (rb_tree_entry_t*) neutron_malloc(sizeof (rb_tree_entry_t));
  
  (*tree->copy_key)(key, &x->key);
  (*tree->copy_value)(value, &x->value);

  tree_insert_help(tree, x);
  new_node = x;
  x->red = true;;
  while (x->parent->red)
    { /* use sentinel instead of checking for root */
    if (x->parent == x->parent->parent->left)
      {
      y = x->parent->parent->right;
      if (y->red)
        {
        x->parent->red = false;;
        y->red = false;;
        x->parent->parent->red = true;;
        x = x->parent->parent;
        }
      else
        {
        if (x == x->parent->right)
          {
          x = x->parent;
          left_rotate(tree, x);
          }
        x->parent->red = false;;
        x->parent->parent->red = true;;
        right_rotate(tree, x->parent->parent);
        }
      }
    else
      { /* case for x->parent == x->parent->parent->right */
      y = x->parent->parent->left;
      if (y->red)
        {
        x->parent->red = false;;
        y->red = false;;
        x->parent->parent->red = true;;
        x = x->parent->parent;
        }
      else
        {
        if (x == x->parent->left)
          {
          x = x->parent;
          right_rotate(tree, x);
          }
        x->parent->red = false;;
        x->parent->parent->red = true;;
        left_rotate(tree, x->parent->parent);
        }
      }
    }
  tree->root->left->red = false;;
  
  return s_ok;
  }

result_t map_remove(handle_t map, const void *key)
  {
  if (map == 0 ||
      key == 0)
    return e_bad_parameter;

  rb_tree_t *tree = (rb_tree_t *) map;
  
  rb_tree_entry_t *z = exact_query(tree, key);
  
  if(z == 0)
    return e_not_found;
  
  rb_tree_entry_t* y;
  rb_tree_entry_t* x;
  rb_tree_entry_t* nil = tree->nil;
  rb_tree_entry_t* root = tree->root;

  y = ((z->left == nil) || (z->right == nil)) ? z : tree_successor(tree, z);
  x = (y->left == nil) ? y->right : y->left;
  
  if (root == (x->parent = y->parent))
    { /* assignment of y->p to x->p is intentional */
    root->left = x;
    }
  else
    {
    if (y == y->parent->left)
      y->parent->left = x;
    else
      y->parent->right = x;
    }
  if (y != z)
    { /* y should not be nil in this case */
    /* y is the node to splice out and x is its child */

    if (!(y->red))
      delete_fix_up(tree, x);

    tree->destroy_key(z->key);
    tree->destroy_value(z->value);
    y->left = z->left;
    y->right = z->right;
    y->parent = z->parent;
    y->red = z->red;
    z->left->parent = z->right->parent = y;
    if (z == z->parent->left)
      {
      z->parent->left = y;
      }
    else
      {
      z->parent->right = y;
      }
    neutron_free(z);
    }
  else
    {
    tree->destroy_key(y->key);
    tree->destroy_value(y->value);
    if (!(y->red))
      delete_fix_up(tree, x);
    neutron_free(y);
    }

  return s_ok;
  }

result_t map_close(handle_t map)
  {
  if (map == 0)
    return e_bad_parameter;

  rb_tree_t *tree = (rb_tree_t *) map;
  tree_destroy_helper(tree, tree->root->left);
  neutron_free(tree->root);
  neutron_free(tree->nil);
  neutron_free(tree);
  
  return s_ok;
  }

result_t map_find(handle_t map, const void *key, void **value)
  {
  if (map == 0 || key == 0 || value == 0)
    return e_bad_parameter;

  rb_tree_t *tree = (rb_tree_t *) map;
  
  rb_tree_entry_t *z = exact_query(tree, key);
  
  if(z == 0)
    return e_not_found;
  
  (*tree->copy_value)(z->value, value);
  
  return s_ok;
  }
