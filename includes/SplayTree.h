#pragma once

#include <stdexcept>
#include <iostream>

template <typename T>
class TreeNode {
public:
	T key;

	TreeNode<T>* parent;
	TreeNode<T>* left;
	TreeNode<T>* right;

	TreeNode(T key);
};

template <typename T>
class SplayTree
{
private:
	TreeNode<T>* root;

	void rotateLeft(TreeNode<T>* x);
	void rotateRight(TreeNode<T>* y);
	void splay(TreeNode<T>* x);
	//! Combine trees tree1 and tree2 into a single tree.
	TreeNode<T>* join(TreeNode<T>* tree1, TreeNode<T>* tree2);

	//! Prints the tree according to the inorder traversal.
	void print(const TreeNode<T>* tree, std::ostream& out);
	void deleteTreeNode(TreeNode<T>* node);

public:
	SplayTree();
	~SplayTree();

	void insert(T key);
	//! Deletes the node with the key 'key'.
	void erase(T key);
	//! Returns the pointer to the node with the key 'key'.
	TreeNode<T>* access(T key);
	//! Returns the pointer to the root.
	TreeNode<T>* getRoot();
	
	//! Prints the tree according to the inorder traversal.
	void print(std::ostream& out = std::cout);
	//! Deletes all nodes.
	void clear();
	
};

template<typename T>
inline TreeNode<T>::TreeNode(T key) : key(key), parent(nullptr), left(nullptr), right(nullptr)
{
}

template<typename T>
inline void SplayTree<T>::rotateLeft(TreeNode<T>* x)
{
	if (!x || !x->right) {
		return;
	}

	TreeNode<T>* y = x->right;
	x->right = y->left;
	if (y->left) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (!x->parent) {
		this->root = y;
	}
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	else {
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

template<typename T>
inline void SplayTree<T>::rotateRight(TreeNode<T>* y)
{
	if (!y || !y->left) {
		return;
	}

	TreeNode<T>* x = y->left;
	y->left = x->right;
	if (x->right) {
		x->right->parent = y;
	}
	x->parent = y->parent;
	if (!y->parent) {
		this->root = x;
	}
	else if (y == y->parent->right) {
		y->parent->right = x;
	}
	else {
		y->parent->left = x;
	}
	x->right = y;
	y->parent = x;
}

template<typename T>
inline void SplayTree<T>::splay(TreeNode<T>* x)
{
	if (!x) {
		throw std::invalid_argument("The argument is nullptr.");
	}
	while (x->parent) {
		if (!x->parent->parent) {
			if (x->parent->left == x) {
				rotateRight(x->parent);
			}
			else {
				rotateLeft(x->parent);
			}
		}
		else if (x->parent->left == x && x->parent->parent->left == x->parent) {
			rotateRight(x->parent->parent);
			rotateRight(x->parent);
		}
		else if (x->parent->right == x && x->parent->parent->right == x->parent) {
			rotateLeft(x->parent->parent);
			rotateLeft(x->parent);
		}
		else if (x->parent->left == x && x->parent->parent->right == x->parent) {
			rotateRight(x->parent);
			rotateLeft(x->parent);
		}
		else {
			rotateLeft(x->parent);
			rotateRight(x->parent);
		}
	}
}

/*!
 * Combine trees tree1 and tree2 into a single tree containing all items from both trees and return 
 * the resulting tree. This operation assumes that all items in tree1 are less than all those in tree2
 * and destroys both tree1 and tree2.
 * 
 * As rotateLeft and rotateRight, used in the splay member function, change this->root
 * the join member function changes this->root and as a result can make the tree nodes of this object unaccesable.
 */
template<typename T>
inline TreeNode<T>* SplayTree<T>::join(TreeNode<T>* tree1, TreeNode<T>* tree2)
{
	if (!tree1) {
		return tree2;
	}
	if (!tree2) {
		return tree1;
	}
	while (tree1->right) {
		tree1 = tree1->right;
	}
	splay(tree1);
	tree1->right = tree2;
	tree2->parent = tree1;
	return tree1;
}

template<typename T>
inline void SplayTree<T>::print(const TreeNode<T>* tree, std::ostream& out)
{
	if (!tree) {
		return;
	}
	print(tree->left, out);
	out << tree->key << " ";
	print(tree->right, out);
}

template<typename T>
inline void SplayTree<T>::deleteTreeNode(TreeNode<T>* node)
{
	if (!node) {
		return;
	}
	deleteTreeNode(node->left);
	deleteTreeNode(node->right);
	delete node;
}

template<typename T>
inline SplayTree<T>::SplayTree() : root(nullptr)
{
}

template<typename T>
inline SplayTree<T>::~SplayTree()
{
	deleteTreeNode(this->root);
}

template<typename T>
inline void SplayTree<T>::insert(T key)
{
	// The splay tree takes ownership of z.
	TreeNode<T>* z = new TreeNode<T>(key);
	TreeNode<T>* x = this->root;
	TreeNode<T>* y = nullptr;
	while (x) {
		y = x;
		if (z->key < x->key) {
			x = x->left;
		}
		else {
			x = x->right;
		}
	}
	z->parent = y;
	if (!y) {
		this->root = z;
	}
	else if (z->key < y->key) {
		y->left = z;
	}
	else {
		y->right = z;
	}
	splay(z);
}

template<typename T>
inline void SplayTree<T>::erase(T key)
{
	TreeNode<T>* node = access(key);
	if (!node) {
		return;
	}
	if (node->left) {
		node->left->parent = nullptr;
	}
	if (node->right) {
		node->right->parent = nullptr;
	}
	this->root = join(node->left, node->right);
	delete node;
}

template<typename T>
inline TreeNode<T>* SplayTree<T>::access(T key)
{
	TreeNode<T>* x = this->root;
	TreeNode<T>* y = nullptr;
	while (x) {
		y = x;
		if (key == x->key) {
			splay(x);
			return x;
		}
		else if (key < x->key) {
			x = x->left;
		}
		else {
			x = x->right;
		}
	}
	if (y) {
		splay(y);
	}
	return nullptr;
}

template<typename T>
inline void SplayTree<T>::print(std::ostream& out)
{
	print(this->root, out);
	out << std::endl;
}

template<typename T>
inline void SplayTree<T>::clear()
{
	deleteTreeNode(this->root);
	this->root = nullptr;
}

template<typename T>
inline TreeNode<T>* SplayTree<T>::getRoot()
{
	return root;
}
