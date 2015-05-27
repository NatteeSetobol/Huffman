#ifndef tree_h
#define tree_h
#include "node.h"
#include <vector>
#include <math.h>
#include "lettercount.h"
#include <stdlib.h>

using namespace std;

enum { NONE, RIGHT, LEFT };


class tree
{
	private:
		
		int FactorTree(node* current, int countL, int countR);
		int height;
		int heightCount;
		void PushToBottom(node* n);
		node* FindEmptyNode(node* n);
		node* FindPrevNode(node* n, node* seek);
		node* FindUnBalanceNode(node* n);
	public:
		tree();
		~tree();
		node* root;

		bool isBalanced(node* n);
		void BuildTree(vector<lettercount> *v);
		void BalanceTree();
		void DelTree(node* n);
		void Swap(node* snode,node* dnode);
		void CheckBottom(node* n);

		node* FindLetter(node* n);
		node* RotateLeft(node* n);
		node* RotateRight(node* n);

		int FindHeight(node* n);
		int MAX(int a, int b);
		int GetMaxHeight();
};



tree::tree()
{
	root=NULL;
	height=0;
	heightCount=0;
}

tree::~tree()
{
	if (root != NULL)
	{
		DelTree(root->left);
		DelTree(root->right);

		delete root->left;
		root->left = NULL;
		delete root->right;
		root->right = NULL;	
	}	
}


int tree::MAX(int a, int b)
{
	return (a >= b)? a:b;
}

int tree::FindHeight(node* Node)
{
	/*
		===========================================================
		We know the total number of bits when we encoded it.

		Problems:
		Recursion
		Might need to rewrite this function to speed it up	
		
		===========================================================
	*/
	int leftHeight=0;
	int rightHeight=0;

	if (Node == NULL)	
	{
		return 0;
	}   
	
	if (Node->left != NULL) leftHeight = FindHeight(Node->left);
	if (Node->right != NULL) rightHeight = FindHeight(Node->right);

	return 1 + MAX(leftHeight, rightHeight);

}

void tree::BuildTree(vector<lettercount> *vectorOfUsedChar)
{	
	/* 
	================================================
	We Build a nice balance Tree so and push
	all the characters to the bottom to make the
	bits nice and even
	===============================================
	*/

	bool turnright=false;
	double total=0.00;
	node* newNode=NULL;
	

    	root = new node(0,vectorOfUsedChar->at(0).count+vectorOfUsedChar->at(1).count, vectorOfUsedChar->at(0).prob + vectorOfUsedChar->at(1).prob);
	root->isletter = false;
    	root->left = new node(vectorOfUsedChar->at(0).letter,vectorOfUsedChar->at(0).count, vectorOfUsedChar->at(0).prob);
	root->left->isletter = true;
    	root->right = new node(vectorOfUsedChar->at(1).letter,vectorOfUsedChar->at(1).count, vectorOfUsedChar->at(1).prob);
	root->right->isletter = true;
    	root->left->left = NULL;
    	root->left->right = NULL;
    	root->right->left = NULL;
    	root->right->right = NULL;
	vector <lettercount>::iterator i = vectorOfUsedChar->begin()+2;
	
	FactorTree(root,0,0);

	while (i != vectorOfUsedChar->end())
	{
		newNode = new node(0, root->GetWeight() + i->count,root->GetProb() + i->prob);
		newNode->isletter = false;
  	     	newNode->left = root;
		newNode->left->isletter = false;
        	newNode->right = new node(i->letter, i->count,i->prob);
		newNode->right->isletter = true;
		total +=newNode->GetProb();
		root = newNode;
		FactorTree(root,0,0);

		if (isBalanced(root) == false)
		{
			/*
			1. find a +2 or -2 node
			2. if left is smaller rotate right, if right is smaller rotate left
			3. Check balanc
			4. if root is not -1, 0, or 1 then goto 1
			*/	
			node* UnBalancedNode=NULL;
			do
			{
				UnBalancedNode = FindUnBalanceNode(root);
				
				if (UnBalancedNode != NULL)
				{		
					
					if (UnBalancedNode->GetBalance()  > 1)
					{
						RotateRight(UnBalancedNode);
					} else
					if (UnBalancedNode->GetBalance() < -1)
					{
						RotateLeft(UnBalancedNode);
					}
					FactorTree(root, 0, 0);
				}
				
			} while (isBalanced(root) == 0);
		}
		/*Push the letters to the very bottom (naddr) */
		PushToBottom(root);
		++i;
	}

	FactorTree(root, 0,0);
	PushToBottom(root);
}	


node* tree::RotateRight(node* nodeToRotate)
{
	/*
	=================================================
	It is necessary to rotate to the right is when
	the tree is unbalanced which means the left side
	of the tree has less nodes then the right side
	===============================================
	*/
	int direction = NONE;
        bool isRoot=false;

        node* rootNode=nodeToRotate;
        node* leftNode=nodeToRotate->left;
        node* leftToRightNode=nodeToRotate->left->right;
	node* prevNode = NULL;
	

        if (nodeToRotate == root) 
	{
		isRoot = true;
	} else {

		prevNode = FindPrevNode(root, nodeToRotate);

		if (prevNode->left == nodeToRotate)
		{
			direction = LEFT;
		}  else 
		if (prevNode->right == nodeToRotate)
		{
			direction = RIGHT;
		}
	}

        nodeToRotate = leftNode;

	leftNode->right = rootNode;
	rootNode->left = leftToRightNode;
	

        if (isRoot) 
	{
		root = leftNode;
	} else {
		if (direction == LEFT)
		{
			prevNode->left = leftNode;
		}else 
		if (direction == RIGHT)
		{
			prevNode->right = leftNode;
		}
	}
	return nodeToRotate;
}

node* tree::RotateLeft(node* nodeToRotate)
{
	/*
	=================================================
	It is necessary to rotate to the left is when
	the tree is unbalanced which means the right side
	of the tree has less nodes then the left side
	===============================================
	*/

	bool isRoot=false;
        int direction=NONE;
	node* prevNode=NULL;
	node* rootNode = NULL;
	node* rightNode = NULL;
	node* rightToLeft = NULL;

        if (nodeToRotate == root)
        {
                isRoot = true;
        } else {
                prevNode = FindPrevNode(root, nodeToRotate);
                if (prevNode->left == nodeToRotate)
                {
                        direction = LEFT;
                }  else
                if (prevNode->right == nodeToRotate)
                {
                        direction = RIGHT;
                }
        }

	rootNode = nodeToRotate;
	rightNode = rootNode->right;
	rightToLeft = rootNode->right->left;

	nodeToRotate = rightNode;
	rightNode->left = rootNode;
	rootNode->right = rightToLeft;
	
        if (isRoot)
        {
                root = rightNode;
        } else {
                if (direction == LEFT)
                {
                        prevNode->left = rightNode;
                }else
                if (direction == RIGHT)
                {
                        prevNode->right = rightNode;
                }
        }

	return nodeToRotate;
}

int tree::FactorTree(node* current, int countL, int countR)
{
	/*
	=================================================
	It order to know which side is better, we have
	to factor the tree.
	=================================================
	*/

	int leftHeight=0;
	int rightHeight=0;

	if (current == NULL)
	{
		return 0;
	}

	if (current->left != NULL) FactorTree(current->left,0,0);
	if (current->right != NULL) FactorTree(current->right,0,0);

	if (current->left != NULL) leftHeight = FindHeight(current->left);
	if (current->right != NULL) rightHeight = FindHeight(current->right);

	current->SetBalance(leftHeight-rightHeight);
		
	return 0;
}

void tree::BalanceTree()
{
	/*
	=================================================
	We need to calculate where an unbalanced tree so
	we can balance it.
	=================================================
	*/
	FactorTree(root,0,0);
	printf("root factor = %i\nLeft: %i\nRight: %i\n", root->GetBalance(), root->left->GetBalance(), root->right->GetBalance());
}

void tree::DelTree(node* nodeToDelete)
{
	/*no rotation deletion*/
	if (nodeToDelete->left != NULL)
	{
		if (nodeToDelete->left->left == NULL)
		{
       			delete nodeToDelete->left;
        		delete nodeToDelete->right;
       			nodeToDelete->left = NULL;
        		nodeToDelete->right = NULL;
			return ;
		}
		return ;
	} 
	
	DelTree(nodeToDelete->left);

	delete nodeToDelete->left;
	delete nodeToDelete->right;
	nodeToDelete->left = NULL;
	nodeToDelete->right = NULL;
}


void tree::PushToBottom(node* nodeToPush)
{
	/* 
		========================================================
		This will push a node with an character in it down to 
		the bottom of the Tree
		========================================================		
	*/
	node* foundLetter = NULL;
	node* emptyNode=NULL;

	foundLetter = FindLetter(nodeToPush);
	
	/*

	 If letter is found
	 check left and right nodes 
	 if both is empty 
	 find an empty node at the bottom

	*/

	
	emptyNode=NULL;
	if (emptyNode == NULL && foundLetter !=NULL)
	{
		/* if there's no node from the bottom of letter than start looking from the root*/
		emptyNode = FindEmptyNode(nodeToPush);
		/*swap letter to the empty node*/
		if (emptyNode != NULL)
		{
			Swap(emptyNode,foundLetter);
		}
	}
}


node* tree::FindLetter(node* nodeToSearch)
{
	/*
	=================================================
	We need to find all the letters that is in random
	spots and push them down to make the bits nice
	and even.
	=================================================
	*/
	node* foundNode=NULL;
	if (nodeToSearch->isletter == true && (nodeToSearch->right != NULL && nodeToSearch->left != NULL))
	{
		return nodeToSearch;
	}

	
	if (nodeToSearch->left != NULL)
	{
		foundNode = FindLetter(nodeToSearch->left);
		if (foundNode != NULL) 
		{
			return foundNode;
		}
	}

	if (nodeToSearch->right != NULL)
	{
		foundNode = FindLetter(nodeToSearch->right);
	}
	return foundNode;
}

node* tree::FindEmptyNode(node* nodeToSearch)
{
	/*
	=================================================
	In order to push a character to the bottom, we
	need to find a empty node at the bottom of the 
	tree.
	=================================================
	*/
	node* foundEmptyNode = NULL;
	
	if (nodeToSearch->isletter == false && (nodeToSearch->right == NULL && nodeToSearch->left == NULL))
	{
		return nodeToSearch;
	}

	if (nodeToSearch->left != NULL)
	{
		foundEmptyNode = FindEmptyNode(nodeToSearch->left);
		if (foundEmptyNode != NULL) return foundEmptyNode;
	}
 
	if (nodeToSearch->right != NULL)
	{
		foundEmptyNode = FindEmptyNode(nodeToSearch->right);
	}
        
	if (nodeToSearch->right == NULL && nodeToSearch->left == NULL)
        {
                heightCount++;
                if (heightCount > height)
                {
                        height = heightCount;
                        heightCount = 0;
                }
        }

	return foundEmptyNode;
}

void tree::CheckBottom(node* nodeToCheck)
{
	/*
	=================================================
	This function is for debugging purpose only to
	see if all of the character used is at the bottom
	of the tree.
	=================================================
	*/
	if (nodeToCheck != NULL && nodeToCheck->isletter && nodeToCheck->left == NULL && nodeToCheck->right==NULL)
	{
		printf("%x\n", nodeToCheck->GetLetter());
		
	}
	if (nodeToCheck->left != NULL)
	{
		CheckBottom(nodeToCheck->left);
	}

	if (nodeToCheck->right != NULL)
	{
		
		CheckBottom(nodeToCheck->right);

	}
	return ;
}

void tree::Swap(node* nodeToSwap,node* nodeToSwapWith)
{
	/*
	=================================================
	We need to swap an empty node in a random spot in
	the tree that has a character and push swap it 
	with an empty node at the bottom.
	=================================================
	*/
	node* temp = nodeToSwap;
	nodeToSwap = nodeToSwapWith;
	nodeToSwapWith = temp;
}


node* tree::FindUnBalanceNode(node* Node)
{
	/*
	=================================================
	In order to balance a node we need to see which
	node in the tree is unbalanced.

	This might slow down the process since it's recursive

	=================================================
	*/
	node* foundNode = NULL;

	if (Node->left != NULL)
	{
		foundNode = FindUnBalanceNode(Node->left);
		if (foundNode!= NULL) return foundNode;
	}
	if (Node->right != NULL && foundNode==NULL)
	{
		foundNode = FindUnBalanceNode(Node->right);
		if (foundNode!=NULL) return foundNode;
	}
	
	
	if ( isBalanced(Node) == false)
        {
                	return Node;
        }
	return foundNode;
}


bool tree::isBalanced(node* nodeToCheck)
{
	/*
	=================================================
	In order to find an unbalance node we need to 
	check if the node is balanced or not

	This might slow down the process since it's recursive

	=================================================
	*/
	int leftHeight=0;
	int rightHeight=0;

	if (nodeToCheck == NULL) return true;


	if (nodeToCheck->left != NULL) leftHeight = FindHeight(nodeToCheck->left);
	if (nodeToCheck->right != NULL) rightHeight = FindHeight(nodeToCheck->right);

	if (abs(leftHeight-rightHeight) <= 1 && isBalanced(nodeToCheck->left) && isBalanced(nodeToCheck->right) ) return 1;

	return 0;
}

node* tree::FindPrevNode(node* nodeToFind, node* seekedNode)
{
	/*
	=================================================
	We need the previous node to reconnect the nodes
	when we rotate the tree.

	This might slow down the process since it's recursive

	=================================================
	*/
	node* prevNode=NULL;
	
	if (nodeToFind->left != NULL) 
	{
		prevNode=FindPrevNode(nodeToFind->left,seekedNode);
	}
	if (nodeToFind->right != NULL && prevNode==NULL) 
	{
		prevNode=FindPrevNode(nodeToFind->right,seekedNode);
	}

        if (nodeToFind->left == seekedNode)
        {
                return nodeToFind;
        }
        if (nodeToFind->right == seekedNode)
        {
                return nodeToFind;
        }

	return prevNode;
}


int tree::GetMaxHeight()
{
	return FindHeight(root)-1;
}
#endif
