/*                  Binary Tree for sorting things
**                  ==============================
**                      Author: Arthur Secret
*/


#include "HTUtils.h"
#include "HTBTree.h"
#include <stdlib.h>
#include <string.h>

#define MAX(a,b) ((a)>(b)?(a):(b))





PUBLIC HTBTree * HTBTree_new ARGS1(HTComparer, comp)
    /*********************************************************
    ** This function returns an HTBTree with memory allocated 
    ** for it when given a mean to compare things
    */
{
    HTBTree * tree = (HTBTree *)malloc(sizeof(HTBTree));
    if (tree==NULL) outofmem(__FILE__, "HTBTree_new");

    tree->compare = comp;
    tree->top = NULL;

    return tree;
}





PUBLIC void HTBTree_free ARGS1(HTBTree*, tree)
    /**************************************************************
    ** This void will free the memory allocated for the whole tree
    */
{
    HTBTElement * wagazou;

    wagazou = HTBTree_next(tree,NULL);
    while (wagazou != NULL)
    {
        free (wagazou);
        wagazou = HTBTree_next(tree,wagazou);
    }
    free(tree);
}





PUBLIC void HTBTree_add ARGS2(
		    HTBTree*,  tree,
		    void*,     object)
    /**********************************************************************
    ** This void is the core of HTBTree.c . It will
    **       1/ add a new element to the tree at the right place
    **          so that the tree remains sorted
    **       2/ balance the tree to be as fast as possible when reading it
    */
{
    HTBTElement * leopard;
    HTBTElement * leopard_son;
    HTBTElement * leopard_father;
    HTBTElement * leo_brother;
    BOOL bouboule,bouboule2,first_correction;
    int depth,depth2;
        /* leopard is a pointer to the structure that is the father of the new 
        ** object "object".
        ** leopard_son is a pointer to the structure that contains or will contain 
        ** the new object "object".
        ** leopard_father and leo_brother are pointers that are used to modify the
        ** depths of upper elements, when needed.
        **
        ** bouboule indicates by a value NO when the future father of "object" is 
        ** found.
        ** bouboule2 indicates by a value NO when, in case of a difference of depths
        **  < 2, the top of the tree is encountered and forbids any further try to
        ** balance the tree.
        ** first_correction is a boolean used to avoid infinite loops in cases
        ** such as:
        **
        **             3                        3
        **          4                              4
        **           5                            5
        **
        ** 3 is used here to show that it need not be the top of the tree.
        */

    /*
    ** 1/ Adding of the element to the binary tree
    */

    if (tree->top == NULL)
    {
        tree->top = (HTBTElement *)malloc(sizeof(HTBTElement));
        if (tree->top == NULL) outofmem(__FILE__, "HTBTree_add");
        tree->top->up = NULL;
        tree->top->object = object;
        tree->top->left = NULL;
        tree->top->left_depth = 0;
        tree->top->right = NULL;
        tree->top->right_depth = 0;
    }
    else
    {   
        bouboule = YES;
        leopard = tree->top;
        leopard_son = NULL;
        leopard_father = NULL;
        leo_brother = NULL;      
        while (bouboule)
        {
            if (tree->compare(object,leopard->object)<0)
	    {
                if (leopard->left != NULL) leopard = leopard->left;
                else 
	        {
                    bouboule = NO;
                    leopard->left = (HTBTElement *)malloc(sizeof(HTBTElement));
                    if (leopard->left==NULL) outofmem(__FILE__, "HTBTree_add");
                    leopard_son = leopard->left;
                    leopard_son->up = leopard;
                    leopard_son->object = object;
                    leopard_son->left = NULL;
                    leopard_son->left_depth = 0;
                    leopard_son->right = NULL;
                    leopard_son->right_depth = 0;
                }
   	    }
            if (tree->compare(object,leopard->object)>0)
            {
                if (leopard->right != NULL) leopard = leopard->right;
                else 
                {  
                    bouboule = NO;
                    leopard->right = (HTBTElement *)malloc(sizeof(HTBTElement));
                    if (leopard->right==NULL) outofmem(__FILE__, "HTBTree_add");
                    leopard_son = leopard->right;
                    leopard_son->up = leopard;
                    leopard_son->object = object;
                    leopard_son->left = NULL;
                    leopard_son->left_depth = 0;
                    leopard_son->right = NULL;
                    leopard_son->right_depth = 0;       
    	        }
            }
	}
            /*
            ** Changing of all depths that need to be changed
            */
        leopard_father = leopard;
        leo_brother = leopard_son;
        do
        {
            if (leopard_father->left == leo_brother)
            {
                depth = leopard_father->left_depth;
                leopard_father->left_depth = 1 
                            + MAX(leo_brother->right_depth,leo_brother->left_depth);
                depth2 = leopard_father->left_depth;
            }
            else
	    {
                depth = leopard_father->right_depth;
                leopard_father->right_depth = 1
                            + MAX(leo_brother->right_depth,leo_brother->left_depth);
                depth2 = leopard_father->right_depth;
            }
            leo_brother = leopard_father;
            leopard_father = leopard_father->up;
        } while ((depth != depth2) && (leopard_father != NULL));
        

            /*
            ** 2/ Balancing the binary tree, if necessary
            */
        bouboule2 = YES;
        first_correction = YES;
        while ((bouboule2) && (first_correction))
        {
            if ((abs(leopard->left_depth - leopard->right_depth)) < 2)
	    {
                if (leopard->up != NULL) leopard = leopard->up;
                else bouboule2 = NO;
	    }
            else
 	    {                /* We start the process of balancing */

                first_correction = NO;
                    /* 
                    ** first_correction is a boolean used to avoid infinite 
                    ** loops in cases such as:
                    **
                    **             3                        3
                    **          4                              4
                    **           5                            5
                    **
                    ** 3 is used to show that it need not be the top of the tree
                    */


                if (leopard->left_depth > leopard->right_depth)
	        {
                    leopard_son = leopard->left;
                    leopard->left_depth = leopard_son->right_depth;
                    leopard_son->right_depth = 1
                                    + MAX(leopard->right_depth,leopard->left_depth);
                    if (leopard->up != NULL)
		    {
                        leopard_father = leopard->up;
                        leo_brother = leopard_son;
                        do 
                        {
                            if (leopard_father->left == leo_brother->up)
                            {
                                depth = leopard_father->left_depth;
                                leopard_father->left_depth = 1
                                                    + MAX(leo_brother->left_depth,
                                                          leo_brother->right_depth);
                                depth2 = leopard_father->left_depth;
			    }
                            else
			    {
                                depth = leopard_father->right_depth;
                                leopard_father->right_depth = 1
                                                    + MAX(leo_brother->left_depth,
                                                          leo_brother->right_depth);
                                depth2 = leopard_father->right_depth;
			    }
                            leo_brother = leopard_father;
                            leopard_father = leopard_father->up;
			} while ((depth != depth2) && (leopard_father != NULL));
                        leopard_father = leopard->up;
                        if (leopard_father->left == leopard)
	                {
                            /*
                            **                   3                       3
                            **               4                       5
                            ** When tree   5   6        becomes    7    4
                            **            7 8                          8 6
                            **
                            ** 3 is used to show that it may not be the top of the
                            ** tree.
                            */ 
                            leopard_father->left = leopard_son;
                            leopard->left = leopard_son->right;
                            leopard_son->right = leopard;
                        }
                        if (leopard_father->right == leopard)
		        {
                            /*
                            **          3                       3
                            **               4                       5
                            ** When tree   5   6        becomes    7    4
                            **            7 8                          8 6
                            **
                            ** 3 is used to show that it may not be the top of the
                            ** tree
                            */
                            leopard_father->right = leopard_son;
                            leopard->left = leopard_son->right;
                            leopard_son->right = leopard;
                        }
                        leopard_son->up = leopard_father;
		    }
                    else
		    {
                        /*
                        **
                        **               1                       2
                        ** When tree   2   3        becomes    4    1
                        **            4 5                          5 3
                        **
                        ** 1 is used to show that it is the top of the tree    
                        */
                        leopard_son->up = NULL;
                        leopard->left = leopard_son->right;
                        leopard_son->right = leopard;
		    }
                    leopard->up = leopard_son;
                    if (leopard->left != NULL)
                        leopard->left->up = leopard;
	        }
                else
	        {
                    leopard_son = leopard->right;
                    leopard->right_depth = leopard_son->left_depth;
                    leopard_son->left_depth = 1 + 
                                      MAX(leopard->right_depth,leopard->left_depth);
                    if (leopard->up != NULL)
		    {
                        leopard_father = leopard->up;
                        leo_brother = leopard_son;
                        do 
                        {
                            if (leopard_father->left == leopard)
                            {
                                depth = leopard_father->left_depth;
                                leopard_father->left_depth = 1
                                                    + MAX(leopard_son->left_depth,
                                                          leopard_son->right_depth);
                                depth2 = leopard_father->left_depth;
			    }
                            else
			    {
                                depth = leopard_father->right_depth;
                                leopard_father->right_depth = 1
                                                   + MAX(leopard_son->left_depth,
                                                         leopard_son->right_depth); 
                                depth2 = leopard_father->right_depth;
			    }
                            leopard_father = leopard_father->up;
                            leo_brother = leopard_father;
			} while ((depth != depth2) && (leopard_father != NULL));;
                        leopard_father = leopard->up;
                        if (leopard_father->left == leopard)
		        {
                            /*
                            **                    3                       3
                            **               4                       6
                            ** When tree   5   6        becomes    4    8
                            **                7 8                 5 7
                            **
                            ** 3 is used to show that it may not be the top of the
                            ** tree.
                            */
                            leopard_father->left = leopard_son;
                            leopard->right = leopard_son->left;
                            leopard_son->left = leopard;
                        }
                        if (leopard_father->right == leopard)
		        {
                            /*
                            **           3                      3
                            **               4                       6
                            ** When tree   5   6        becomes    4    8
                            **                7 8                 5 7
                            **
                            ** 3 is used to show that it may not be the top of the
                            ** tree
                            */
                            leopard_father->right = leopard_son;
                            leopard->right = leopard_son->left;
                            leopard_son->left = leopard;
                        }
                        leopard_son->up = leopard_father;
		    }
                    else
                    {
                        /*
                        **
                        **               1                       3
                        ** When tree   2   3        becomes    1    5
                        **                4 5                 2 4
                        **
                        ** 1 is used to show that it is the top of the tree.
                        */
                        leopard_son->up = NULL;
                        leopard->right = leopard_son->left;
                        leopard_son->left = leopard;
		    }
                    leopard->up = leopard_son;
                    leopard->right->up = leopard;
		}
	    }
        }
        while (leopard->up != NULL)
	{
            leopard = leopard->up;
        }
        tree->top = leopard;
    }
}



PUBLIC HTBTElement * HTBTree_next ARGS2(
                               HTBTree*,       tree,
                               HTBTElement*,   ele)
    /**************************************************************************
    ** this function returns a pointer to the leftmost element if ele is NULL,
    ** and to the next object to the right otherways.
    ** If no elements left, returns a pointer to NULL.
    */
{
    HTBTElement * leopard;
    HTBTElement * leopard_father;

    if (ele == NULL)
    {
        leopard = tree->top;
        if (leopard != NULL)
            while (leopard->left != NULL)
                leopard = leopard->left;
    }
    else
    {
        leopard = ele;
        if (leopard->right != NULL)
	{
            leopard = leopard->right;
            while (leopard->left != NULL)
                leopard = leopard->left;
	}
        else
	{
            leopard_father = leopard->up;
            while (leopard_father->right == leopard)
	    {
                leopard = leopard_father;
                leopard_father = leopard->up;
	    }
            leopard = leopard_father;
	}
    }
    return leopard;
}


#ifdef TEST
main ()
    /******************************************************
    ** This is just a test to show how to handle HTBTree.c
    */
{
    HTBTree * tree;
    HTBTElement * wagazou;
    
    tree = HTBTree_new(strcasecmp);
    HTBTree_add(tree,"zoo");
    HTBTree_add(tree,"carot");
    HTBTree_add(tree,"elephant\t");
    HTBTree_add(tree,"coat");
    HTBTree_add(tree,"snake");
    HTBTree_add(tree,"Peter");
    HTBTree_add(tree,"Harrison");
    HTBTree_add(tree,"WWW");
    wagazou = HTBTree_next(tree,NULL);
    while (wagazou != NULL)
    {
        printf("The next element is %s\n",wagazou->object);
        wagazou = HTBTree_next(tree,wagazou);
    }
    HTBTree_free (tree);
}


#endif
