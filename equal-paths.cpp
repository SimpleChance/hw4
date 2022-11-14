#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height(Node * root)
{
	// Base Case
	if (root == nullptr) return 0;

	// Grab left and right node
	int rh = height(root->right), lh = height(root->left);

	// return depending on lh > rh
	return lh > rh ? lh + 1 : rh + 1;
}

bool equalPaths(Node * root)
{
  // Add your code below

	// Base case
	if (root == nullptr) return true;

	// Check for both left and right nodes
	if (root->left && root->right){

		// Calculate height of left and right trees
		int rh = height(root->right), lh = height(root->left);

		// Check for not balanced
		if (lh!= rh) return false;

		// If balanced
		else {
			// Base case
			if (lh == 1) return true;
			
			// Find height
			return equalPaths(root->left) && equalPaths(root->right);
		}
	}

	// If only a left tree
	else if (root->left){
		return equalPaths(root->left);
	}
	// If only a right tree
	else if (root->right){
		return equalPaths(root->right);
	}
	// If no trees
	else{
		return true;
	}
}

