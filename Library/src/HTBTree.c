/*                  Binary Tree for sorting things
**                  ==============================
**                      Author: Arthur Secret
*/


#include "HTUtils.h"
#include "HTBTree.h"
#include <stdlib.h>
#include <string.h>

#define MAXIMUM(a,b) ((a)>(b)?(a):(b))





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




PRIVATE void HTBTElement_free ARGS1(HTBTElement*, element)
    /**********************************************************
    ** This void will free the memory allocated for one element
    */
{
    if (element->left != NULL)    HTBTElement_free(element->left);
    if (element->right != NULL)    HTBTElement_free(element->right);
    free(element);
}

PUBLIC void HTBTree_free ARGS1(HTBTree*, tree)
    /**************************************************************
    ** This void will free the memory allocated for the whole tree
    */
{
    HTBTElement_free(tree->top);
    free(tree);
}




PRIVATE void HTBTElementAndObject_free ARGS1(HTBTElement*, element)
    /**********************************************************
    ** This void will free the memory allocated for one element
    */
{
    if (element->left != NULL)    HTBTElementAndObject_free(element->left);
    if (element->right != NULL)    HTBTElementAndObject_free(element->right);
    free(element->object);
    free(element);
}

PUBLIC void HTBTreeAndObject_free ARGS1(HTBTree*, tree)
    /**************************************************************
    ** This void will free the memory allocated for the whole tree
    */
{
    HTBTElementAndObject_free(tree->top);
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
    HTBTElement * father_of_element;
    HTBTElement * added_element;
    HTBTElement * forefather_of_element;
    HTBTElement * father_of_forefather;
    BOOL father_found,top_found,first_correction;
    int depth,depth2;
        /* father_of_element is a pointer to the structure that is the father of the
        ** new object "object".
        ** added_element is a pointer to the structure that contains or will contain 
        ** the new object "object".
        ** father_of_forefather and forefather_of_element are pointers that are used
        ** to modify the depths of upper elements, when needed.
        **
        ** father_found indicates by a value NO when the future father of "object" 
        ** is found.
        ** top_found indicates by a value NO when, in case of a difference of depths
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
        father_found = YES;
        father_of_element = tree->top;
        added_element = NULL;
        father_of_forefather = NULL;
        forefather_of_element = NULL;      
        while (father_found)
        {
            if (tree->compare(object,father_of_element->object)<0)
	    {
                if (father_of_element->left != NULL)
                    father_of_element = father_of_element->left;
                else 
	        {
                    father_found = NO;
                    father_of_element->left = 
                        (HTBTElement *)malloc(sizeof(HTBTElement));
                    if (father_of_element->left==NULL) 
                        outofmem(__FILE__, "HTBTree_add");
                    added_element = father_of_element->left;
                    added_element->up = father_of_element;
                    added_element->object = object;
                    added_element->left = NULL;
                    added_element->left_depth = 0;
                    added_element->right = NULL;
                    added_element->right_depth = 0;
                }
   	    }
            if (tree->compare(object,father_of_element->object)>0)
            {
                if (father_of_element->right != NULL) 
                    father_of_element = father_of_element->right;
                else 
                {  
                    father_found = NO;
                    father_of_element->right = 
                        (HTBTElement *)malloc(sizeof(HTBTElement));
                    if (father_of_element->right==NULL) 
                        outofmem(__FILE__, "HTBTree_add");
                    added_element = father_of_element->right;
                    added_element->up = father_of_element;
                    added_element->object = object;
                    added_element->left = NULL;
                    added_element->left_depth = 0;
                    added_element->right = NULL;
                    added_element->right_depth = 0;       
    	        }
            }
	}
            /*
            ** Changing of all depths that need to be changed
            */
        father_of_forefather = father_of_element;
        forefather_of_element = added_element;
        do
        {
            if (father_of_forefather->left == forefather_of_element)
            {
                depth = father_of_forefather->left_depth;
                father_of_forefather->left_depth = 1 
                            + MAXIMUM(forefather_of_element->right_depth,
                                  forefather_of_element->left_depth);
                depth2 = father_of_forefather->left_depth;
            }
            else
	    {
                depth = father_of_forefather->right_depth;
                father_of_forefather->right_depth = 1
                            + MAXIMUM(forefather_of_element->right_depth,
                                  forefather_of_element->left_depth);
                depth2 = father_of_forefather->right_depth;
            }
            forefather_of_element = father_of_forefather;
            father_of_forefather = father_of_forefather->up;
        } while ((depth != depth2) && (father_of_forefather != NULL));
        

            /*
            ** 2/ Balancing the binary tree, if necessary
            */
        top_found = YES;
        first_correction = YES;
        while ((top_found) && (first_correction))
        {
            if ((abs(father_of_element->left_depth
                      - father_of_element->right_depth)) < 2)
	    {
                if (father_of_element->up != NULL) 
                    father_of_element = father_of_element->up;
                else top_found = NO;
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


                if (father_of_element->left_depth > father_of_element->right_depth)
	        {
                    added_element = father_of_element->left;
                    father_of_element->left_depth = added_element->right_depth;
                    added_element->right_depth = 1
                                    + MAXIMUM(father_of_element->right_depth,
                                          father_of_element->left_depth);
                    if (father_of_element->up != NULL)
		    {
                        father_of_forefather = father_of_element->up;
                        forefather_of_element = added_element;
                        do 
                        {
                            if (father_of_forefather->left
                                 == forefather_of_element->up)
                            {
                                depth = father_of_forefather->left_depth;
                                father_of_forefather->left_depth = 1
                                    + MAXIMUM(forefather_of_element->left_depth,
                                          forefather_of_element->right_depth);
                                depth2 = father_of_forefather->left_depth;
			    }
                            else
			    {
                                depth = father_of_forefather->right_depth;
                                father_of_forefather->right_depth = 1
                                    + MAXIMUM(forefather_of_element->left_depth,
                                          forefather_of_element->right_depth);
                                depth2 = father_of_forefather->right_depth;
			    }
                            forefather_of_element = father_of_forefather;
                            father_of_forefather = father_of_forefather->up;
			} while ((depth != depth2) && 
				 (father_of_forefather != NULL));
                        father_of_forefather = father_of_element->up;
                        if (father_of_forefather->left == father_of_element)
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
                            father_of_forefather->left = added_element;
                            father_of_element->left = added_element->right;
                            added_element->right = father_of_element;
                        }
                        if (father_of_forefather->right == father_of_element)
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
                            father_of_forefather->right = added_element;
                            father_of_element->left = added_element->right;
                            added_element->right = father_of_element;
                        }
                        added_element->up = father_of_forefather;
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
                        added_element->up = NULL;
                        father_of_element->left = added_element->right;
                        added_element->right = father_of_element;
		    }
                    father_of_element->up = added_element;
                    if (father_of_element->left != NULL)
                        father_of_element->left->up = father_of_element;
	        }
                else
	        {
                    added_element = father_of_element->right;
                    father_of_element->right_depth = added_element->left_depth;
                    added_element->left_depth = 1 + 
                            MAXIMUM(father_of_element->right_depth,
                                father_of_element->left_depth);
                    if (father_of_element->up != NULL)
		    {
                        father_of_forefather = father_of_element->up;
                        forefather_of_element = added_element;
                        do 
                        {
                            if (father_of_forefather->left == father_of_element)
                            {
                                depth = father_of_forefather->left_depth;
                                father_of_forefather->left_depth = 1
                                                 + MAXIMUM(added_element->left_depth,
                                                         added_element->right_depth);
                                depth2 = father_of_forefather->left_depth;
			    }
                            else
			    {
                                depth = father_of_forefather->right_depth;
                                father_of_forefather->right_depth = 1
                                                 + MAXIMUM(added_element->left_depth,
                                                         added_element->right_depth);
                                depth2 = father_of_forefather->right_depth;
			    }
                            father_of_forefather = father_of_forefather->up;
                            forefather_of_element = father_of_forefather;
			} while ((depth != depth2) && 
				 (father_of_forefather != NULL));
                        father_of_forefather = father_of_element->up;
                        if (father_of_forefather->left == father_of_element)
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
                            father_of_forefather->left = added_element;
                            father_of_element->right = added_element->left;
                            added_element->left = father_of_element;
                        }
                        if (father_of_forefather->right == father_of_element)
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
                            father_of_forefather->right = added_element;
                            father_of_element->right = added_element->left;
                            added_element->left = father_of_element;
                        }
                        added_element->up = father_of_forefather;
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
                        added_element->up = NULL;
                        father_of_element->right = added_element->left;
                        added_element->left = father_of_element;
		    }
                    father_of_element->up = added_element;
                    father_of_element->right->up = father_of_element;
		}
	    }
        }
        while (father_of_element->up != NULL)
	{
            father_of_element = father_of_element->up;
        }
        tree->top = father_of_element;
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
    HTBTElement * father_of_element;
    HTBTElement * father_of_forefather;

    if (ele == NULL)
    {
        father_of_element = tree->top;
        if (father_of_element != NULL)
            while (father_of_element->left != NULL)
                father_of_element = father_of_element->left;
    }
    else
    {
        father_of_element = ele;
        if (father_of_element->right != NULL)
	{
            father_of_element = father_of_element->right;
            while (father_of_element->left != NULL)
                father_of_element = father_of_element->left;
	}
        else
	{
            father_of_forefather = father_of_element->up;
	        while (father_of_forefather && 
		       (father_of_forefather->right == father_of_element))
      	        {
                    father_of_element = father_of_forefather;
		    father_of_forefather = father_of_element->up;
		}
            father_of_element = father_of_forefather;
	}
    }
#ifdef BTREE_TRACE
    /* The option -DBTREE_TRACE will give much more information
    ** about the way the process is running, for debugging matters
    */
    if (father_of_element != NULL)
    {
        printf("\nObject = %s\t",(char *)father_of_element->object);
        if (father_of_element->up != NULL)
            printf("Objet du pere = %s\n",
		   (char *)father_of_element->up->object);
        else printf("Pas de Pere\n");
        if (father_of_element->left != NULL)
            printf("Objet du fils gauche = %s\t",
		   (char *)father_of_element->left->object); 
        else printf("Pas de fils gauche\t");
        if (father_of_element->right != NULL)
            printf("Objet du fils droit = %s\n",
		   (char *)father_of_element->right->object);
        else printf("Pas de fils droit\n");
        printf("Profondeur gauche = %i\t",father_of_element->left_depth);
        printf("Profondeur droite = %i\n",father_of_element->right_depth);
        printf("      **************\n");
    }
#endif
    return father_of_element;
}


#ifdef TEST
main ()
    /******************************************************
    ** This is just a test to show how to handle HTBTree.c
    */
{
    HTBTree * tree;
    HTBTElement * next_element;
    
    tree = HTBTree_new(strcasecmp);
    HTBTree_add(tree,"zoo");
    HTBTree_add(tree,"carot");
    HTBTree_add(tree,"elephant\t");
    HTBTree_add(tree,"coat");
    HTBTree_add(tree,"snake");
    HTBTree_add(tree,"Peter");
    HTBTree_add(tree,"Harrison");
    HTBTree_add(tree,"WWW");
#ifdef BTREE_TRACE
    printf("\nTreeTopObject=%s\n\n",tree->top->object);
#endif
    next_element = HTBTree_next(tree,NULL);
    while (next_element != NULL)
    {
#ifndef BTREE_TRACE
        printf("The next element is %s\n",next_element->object);
#endif
        next_element = HTBTree_next(tree,next_element);
    }
    HTBTree_free (tree);
}


#endif
