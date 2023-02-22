/***********************************************************************
 * Header:
 *    BST
 * Summary:
 *    Our custom implementation of a BST for set and for map
 *      __       ____       ____         __
 *     /  |    .'    '.   .'    '.   _  / /
 *     `| |   |  .--.  | |  .--.  | (_)/ /
 *      | |   | |    | | | |    | |   / / _
 *     _| |_  |  `--'  | |  `--'  |  / / (_)
 *    |_____|  '.____.'   '.____.'  /_/
 *
 *    This will contain the class definition of:
 *        BST                 : A class that represents a binary search tree
 *        BST::iterator       : An iterator through BST
 * Author
 *    Joel Jossie and Gergo Medveczky
 ************************************************************************/

#pragma once

#ifdef DEBUG
#define debug(x) x
#else // !DEBUG
#define debug(x)
#endif // !DEBUG

#include <cassert>
#include <utility>
#include <memory>     // for std::allocator
#include <functional> // for std::less
#include <utility>    // for std::pair
#include <iostream>
class TestBST; // forward declaration for unit tests
class TestMap;
class TestSet;

namespace custom
{

   template <class TT>
   class set;
   template <class KK, class VV>
   class map;

/*****************************************************************
 * BINARY SEARCH TREE
 * Create a Binary Search Tree
 *****************************************************************/
template <typename T>
class BST
{
   friend class ::TestBST; // give unit tests access to the privates
   friend class ::TestMap;
   friend class ::TestSet;

   template <class KK, class VV>
   friend class map;

   template <class TT>
   friend class set;

   template <class KK, class VV>
   friend void swap(map<KK, VV>& lhs, map<KK, VV>& rhs);
public:
   //
   // Construct
   //

   BST();
   BST(const BST &  rhs);
   BST(      BST && rhs);
   BST(const std::initializer_list<T>& il);
   ~BST();

   //
   // Assign
   //

   BST & operator = (const BST &  rhs);
   BST & operator = (      BST && rhs);
   BST & operator = (const std::initializer_list<T>& il);
   void swap(BST & rhs);

   //
   // Iterator
   //

   class iterator;
   iterator   begin() const noexcept;
   iterator   end()   const noexcept { return iterator(nullptr); }

   //
   // Access
   //

   iterator find(const T& t);

   //
   // Insert
   //

   std::pair<iterator, bool> insert(const T&  t, bool keepUnique = false);
   std::pair<iterator, bool> insert(      T&& t, bool keepUnique = false);

   //
   // Remove
   //

   iterator erase(iterator& it);
   void   clear() noexcept;

   //
   // Status
   //

   bool   empty() const noexcept { return (numElements == 0); }
   size_t size()  const noexcept { return numElements;        }
   

private:

   class BNode;
   BNode * root;              // root node of the binary search tree
   size_t numElements;        // number of elements currently in the tree

   void _assign(BNode * & pDest, const BNode* pSrc)
   {
      // If source is empty.
      if (pSrc == nullptr)
      {
         _clear(pDest);
         return;
      }
      
      // Check if pDest exists.
      else if (pDest == nullptr)
      {
         pDest = new BNode(pSrc->data);        // V
         _assign(pDest->pRight, pSrc->pRight); // R
         _assign(pDest->pLeft, pSrc->pLeft);   // L
      }
      else
      {
         pDest->data = pSrc->data;             // V
         _assign(pDest->pRight, pSrc->pRight); // R
         _assign(pDest->pLeft, pSrc->pLeft);   // L
      }

      // Connect any newly assigned children to this one as a parent
      if (pDest->pRight)
         pDest->pRight->pParent = pDest;
      if (pDest->pLeft)
         pDest->pLeft->pParent = pDest;
   }

   // Destroy the whole tree
   void _clear(BNode * & pThis)
   {
      if (pThis == nullptr)
         return;
      _clear(pThis->pLeft);   // L
      _clear(pThis->pRight);  // R
      delete pThis;           // V
      pThis = nullptr;
   }
   
   std::pair<BNode *, bool> _insert(BNode * pNode, const T & t, bool keepUnique)
   {
      // Equal case, but only check if we're keeping unique
      if (keepUnique && t == pNode->data)
         return {pNode, false};
      // Less than - Left
      else if (t < pNode->data)
      {
         if (pNode->pLeft)
            return _insert(pNode->pLeft, t, keepUnique);
         else
         {
            pNode->addLeft(t);
            return {pNode->pLeft, true};
         }
      }
      // Greater than or equal to - right
      else
      {
         if (pNode->pRight)
            return _insert(pNode->pRight, t, keepUnique);
         else
         {
            pNode->addRight(t);
            return {pNode->pRight, true};
         }
      }
   }

   std::pair<BNode *, bool> _insertMove(BNode * pNode, T && t, bool keepUnique)
   {
      // Equal case, but only check if we're keeping unique
      if (keepUnique && t == pNode->data)
         return {pNode, false};
         // Less than - Left
      else if (t < pNode->data)
      {
         if (pNode->pLeft)
            return _insertMove(pNode->pLeft, std::move(t), keepUnique);
         else
         {
            pNode->addLeft(std::move(t));
            return {pNode->pLeft, true};
         }
      }
         // Greater than or equal to - right
      else
      {
         if (pNode->pRight)
            return _insertMove(pNode->pRight, std::move(t), keepUnique);
         else
         {
            pNode->addRight(std::move(t));
            return {pNode->pRight, true};
         }
      }
   }

};


/*****************************************************************
 * BINARY NODE
 * A single node in a binary tree. Note that the node does not know
 * anything about the properties of the tree so no validation can be done.
 *****************************************************************/
template <typename T>
class BST <T> :: BNode
{
public:
   //
   // Construct
   //
   BNode()              : pLeft(nullptr), pRight(nullptr), pParent(nullptr), data(0)            { }
   BNode(const T &  t)  : pLeft(nullptr), pRight(nullptr), pParent(nullptr), data(t)            { }
   BNode(T && t)        : pLeft(nullptr), pRight(nullptr), pParent(nullptr), data(std::move(t)) { }

   //
   // Insert
   //
   void addLeft (BNode * pNode);
   void addRight(BNode * pNode);
   void addLeft (const T &  t);
   void addRight(const T &  t);
   void addLeft(       T && t);
   void addRight(      T && t);

   //
   // Status
   //
   bool isRightChild() const { return (pParent && pParent->pRight == this); }
   bool isLeftChild()  const { return (pParent && pParent->pLeft == this);  }

   //
   // Data
   //
   T data;                  // Actual data stored in the BNode
   BNode* pLeft;            // Left child - smaller
   BNode* pRight;           // Right child - larger
   BNode* pParent;          // Parent
   bool isRed;              // Red-black balancing stuff

};




/**********************************************************
 * BINARY SEARCH TREE ITERATOR
 * Forward and reverse iterator through a BST
 *********************************************************/
template <typename T>
class BST <T> :: iterator
{
   friend class ::TestBST; // give unit tests access to the privates
   friend class ::TestMap;
   friend class ::TestSet;

   template <class KK, class VV>
   friend class map;

   template <class TT>
   friend class set;
public:
   // constructors and assignment
   iterator(BNode * p = nullptr): pNode(p)       { }
   iterator(const iterator & rhs)                { this->pNode = rhs.pNode; }
   iterator & operator = (const iterator & rhs)
   {
      this->pNode = rhs.pNode;
      return *this;
   }

   // compare
   bool operator == (const iterator & rhs) const { return (pNode == rhs.pNode || this->pNode == nullptr); }
   bool operator != (const iterator & rhs) const { return (pNode != rhs.pNode);                           }

   // de-reference. Cannot change because it will invalidate the BST
   const T & operator * () const                 { return pNode->data;                                    }

   // increment and decrement
   iterator & operator ++ ();
   iterator   operator ++ (int postfix)          { return *this;                                          }
   iterator & operator -- ();
   iterator   operator -- (int postfix)          { return *this;;                                         }

   // must give friend status to remove so it can call getNode() from it
   friend BST <T> :: iterator BST <T> :: erase(iterator & it);

private:

   // the node
   BNode * pNode;
};

/*********************************************
 *********************************************
 *********************************************
 ******************** BST ********************
 *********************************************
 *********************************************
 *********************************************/


/*********************************************
 * BST :: DEFAULT CONSTRUCTOR
 ********************************************/
template <typename T>
BST <T> ::BST(): root(nullptr), numElements(0) { }

/*********************************************
 * BST :: COPY CONSTRUCTOR
 * Copy one tree to another
 ********************************************/
template <typename T>
BST <T> :: BST ( const BST<T>& rhs): root(nullptr), numElements(rhs.numElements) { *this = rhs; }

/*********************************************
 * BST :: MOVE CONSTRUCTOR
 * Move one tree to another
 ********************************************/
template <typename T>
BST <T> :: BST(BST <T> && rhs) : root(nullptr), numElements(0)                   { *this = std::move(rhs); }

/*********************************************
 * BST :: INITIALIZER LIST CONSTRUCTOR
 * Create a BST from an initializer list
 ********************************************/
template <typename T>
BST <T> ::BST(const std::initializer_list<T>& il) : root(nullptr), numElements(0)
{
   for (auto & element : il)
      insert(element);
}

/*********************************************
 * BST :: DESTRUCTOR
 ********************************************/
template <typename T>
BST <T> :: ~BST() { clear(); }


/*********************************************
 * BST :: ASSIGNMENT OPERATOR
 * Copy one tree to another
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (const BST <T> & rhs)
{
   _assign(this->root, rhs.root);
   numElements = rhs.numElements;
   return *this;
}

/*********************************************
 * BST :: ASSIGNMENT OPERATOR with INITIALIZATION LIST
 * Copy nodes onto a BTree
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (const std::initializer_list<T>& il)
{
   clear();
   for (auto & item : il)
      insert(item);
   return *this;
}

/*********************************************
 * BST :: ASSIGN-MOVE OPERATOR
 * Move one tree to another
 ********************************************/
template <typename T>
BST <T> & BST <T> :: operator = (BST <T> && rhs)
{
   clear();
   swap(rhs);
   return *this;
}

/*********************************************
 * BST :: SWAP
 * Swap two trees
 ********************************************/
template <typename T>
void BST <T> :: swap (BST <T>& rhs)
{
    std::swap(root, rhs.root);
    std::swap(numElements, rhs.numElements);
}

/*****************************************************
 * BST :: INSERT
 * Insert a node at a given location in the tree
 ****************************************************/
template <typename T>
std::pair<typename BST <T> :: iterator, bool> BST <T> :: insert(const T & t, bool keepUnique)
{
   if (root)
   {
      auto result = _insert(root, t, keepUnique);
      if (result.second) // If we inserted a new node, increment numElements
         numElements++;
      return result;

   }
   else // In case the tree is empty
   {
      root = new BNode(t);
      numElements = 1;
      return { iterator(root), true };
   }
}

template <typename T>
std::pair<typename BST <T> ::iterator, bool> BST <T> ::insert(T && t, bool keepUnique)
{
   auto result = _insertMove(root, std::move(t), keepUnique);
   if (result.second) // If we inserted a new node, increment numElements
      numElements++;
   
   return result;
}

/*************************************************
 * BST :: ERASE
 * Remove a given node as specified by the iterator
 ************************************************/
template <typename T>
typename BST <T> ::iterator BST <T> :: erase(iterator & it)
{
   // If we're erasing nullptr, do nothing
   if (!it.pNode)
      return it;

   // Case 1: No Children
   else if(!it.pNode->pRight && !it.pNode->pLeft)
   {
      if (it.pNode->isRightChild())
         it.pNode->pParent->pRight = nullptr;
      else if (it.pNode->isLeftChild())
         it.pNode->pParent->pLeft = nullptr;
   }

   // Case 2a: One Left Child
   else if(!it.pNode->pRight && it.pNode->pLeft)
   {
      it.pNode->pLeft->pParent = it.pNode->pParent;
      if(it.pNode->pParent->pRight->data == it.pNode->data)
      {
         it.pNode->pRight = it.pNode->pLeft;
      }
      if(it.pNode->pParent->pLeft->data == it.pNode->data)
      {
         it.pNode->pParent->pLeft = it.pNode->pLeft;
      }
   }

   // Case 2b: One Right Child
   else if(!it.pNode->pLeft && it.pNode->pRight)
   {

      it.pNode->pRight->pParent = it.pNode->pParent;
      if(it.pNode->pParent->pRight->data == it.pNode->data)
      {
         it.pNode->pParent->pRight = it.pNode->pRight;
      }
      if(it.pNode->pParent->pLeft->data == it.pNode->data)
      {
         it.pNode->pParent->pLeft = it.pNode->pRight;
      }
   }

   // Case 3: Two Children
   else if(it.pNode->pLeft && it.pNode->pRight)
   {
      auto itInOrderSuccessor = it.pNode->pRight;
      while(itInOrderSuccessor->pLeft != nullptr)
      {
         itInOrderSuccessor = itInOrderSuccessor->pLeft;
      }
      // hold inOrderSuccessor's parent
      auto inOrderRChild = itInOrderSuccessor->pRight;

      // hold inOrderSuccessor's right child (it has no left)
      it.pNode->pParent->pLeft = itInOrderSuccessor;
      // set it->parent->child to itInOrderSuccessor
      itInOrderSuccessor->pRight = it.pNode->pRight;
      itInOrderSuccessor->pLeft = it.pNode->pLeft;
      itInOrderSuccessor->pRight->pLeft = inOrderRChild;

      // set inOrderSuccessor's children to it's children
      itInOrderSuccessor->pParent = it.pNode->pParent;
      itInOrderSuccessor->pRight->pParent = itInOrderSuccessor;
      itInOrderSuccessor->pLeft->pParent = itInOrderSuccessor;
      inOrderRChild->pParent = itInOrderSuccessor->pRight;
      // adopt the orphan
   }
   delete it.pNode;
   it.pNode = nullptr;
   numElements--;
   return it;
}

/*****************************************************
 * BST :: CLEAR
 * Removes all the BNodes from a tree
 ****************************************************/
template <typename T>
void BST <T> ::clear() noexcept
{
   numElements = 0;
   _clear(root);
}

/*****************************************************
 * BST :: BEGIN
 * Return the first node (left-most) in a binary search tree
 ****************************************************/
template <typename T>
typename BST <T> :: iterator custom :: BST <T> :: begin() const noexcept
{
   
   if(empty())
      return end();
   auto p = root;
   while(p->pLeft)
      p = p->pLeft;
   return iterator(p);
}


/****************************************************
 * BST :: FIND
 * Return the node corresponding to a given value
 ****************************************************/
template <typename T>
typename BST <T> :: iterator BST<T> :: find(const T & t)
{
   auto p = root;
   while(p != nullptr)
   {
      if(p->data == t)
         return iterator(p);
      else if(t < p->data)
         p = p->pLeft;
      else
         p = p->pRight;
   }
   return end();
}

/******************************************************
 ******************************************************
 ******************************************************
 *********************** B NODE ***********************
 ******************************************************
 ******************************************************
 ******************************************************/

 
/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addLeft (BNode * pNode)
{
   if (pNode != nullptr)
      pNode->pParent = this;

   this->pLeft = pNode;
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addRight (BNode * pNode)
{
   if (pNode != nullptr)
      pNode->pParent = this;

   this->pRight = pNode;
}

/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST<T> :: BNode :: addLeft (const T & t)
{
   auto pAdd = new BNode(t);
   pAdd->pParent = this;
   this->pLeft = pAdd;
}

/******************************************************
 * BINARY NODE :: ADD LEFT
 * Add a node to the left of the current node
 ******************************************************/
template <typename T>
void BST<T> ::BNode::addLeft(T && t)
{
   auto pAdd = new BNode(std::move(t));
   pAdd->pParent = this;
   this->pLeft = pAdd;
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> :: BNode :: addRight (const T & t)
{
   auto pAdd = new BNode(t);
   pAdd->pParent = this;
   this->pRight = pAdd;
}

/******************************************************
 * BINARY NODE :: ADD RIGHT
 * Add a node to the right of the current node
 ******************************************************/
template <typename T>
void BST <T> ::BNode::addRight(T && t)
{
   auto pAdd = new BNode(std::move(t));
   pAdd->pParent = this;
   this->pRight = pAdd;
}



/*************************************************
 *************************************************
 *************************************************
 ****************** ITERATOR *********************
 *************************************************
 *************************************************
 *************************************************/

/**************************************************
 * BST ITERATOR :: INCREMENT PREFIX
 * advance by one
 *************************************************/
template <typename T>
typename BST <T> :: iterator & BST <T> :: iterator :: operator ++ ()
{
   // Check for null node
   if(pNode == nullptr)
      return *this;
   
   // Case when you have to go right one and dig all the way down left.
   else if(pNode->pRight)
   {
      pNode = pNode->pRight;
      while(pNode->pLeft)
         pNode = pNode->pLeft;
   }
   // Case when the next consecutive number is parent.
   else if(pNode->pRight == nullptr && pNode->pParent->pLeft == pNode)
   {
      pNode = pNode->pParent;
   }
   // Case when you have to go all the way up to grannys!
   else if(pNode->pRight == nullptr && pNode->pParent->pRight == pNode)
   {
      while(pNode->pParent && pNode->pParent->pRight == pNode)
         pNode = pNode->pParent;
      pNode = pNode->pParent;
   }
   return *this;
}

/**************************************************
 * BST ITERATOR :: DECREMENT PREFIX
 * advance by one
 *************************************************/
template <typename T>
typename BST <T> :: iterator & BST <T> :: iterator :: operator -- ()
{
   if(pNode == nullptr)
      return *this;
   else if(pNode->pLeft)
   {
      pNode = pNode->pLeft;
      while(pNode->pLeft)
         pNode = pNode->pRight;
   }
   else if(pNode->pLeft == nullptr && pNode->pParent->pRight == pNode)
   {
      pNode = pNode->pParent;
   }
   else if(pNode->pLeft == nullptr && pNode->pParent->pLeft == pNode)
   {
      while(pNode->pParent && pNode->pParent->pLeft == pNode)
         pNode = pNode->pParent;
      pNode = pNode->pParent;
   }
   return *this;
}


} // namespace custom


