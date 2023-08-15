#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// node structure definition for the trie
struct node  {
   struct node *nxt[256];
   char* str;
   int count;
   int max;
};

// trie structure definition
struct ptrie {
   struct node* nodes[256];
   struct node* root;
};
// helper function to allocate memory for a new node in the trie
static struct node *node_allocate(void) {
   struct node *n = malloc(sizeof(struct node));
   if (n == NULL) {
      return NULL;
   }
   memset(n->nxt, 0, sizeof(n->nxt));
   n->str = NULL;
   n->count = 0;
   n->max = 0;
   return n;
}
// helper function to free memory used by a node and its descendants
static void node_free(struct node *n) {
   if (n->str) {
      free(n->str);
   }
   for (int i = 0; i < 256; i++) {
      if (n->nxt[i]) {
         node_free(n->nxt[i]);
      }
   }
   free(n);
}

// converts a character to an array offset for the trie node's nxt array
static int ptrie_char2off(char c) {
   if (c < 32 || c > 126) {
      return -1;
   }
   return (int)c - 32;
}

struct ptrie *ptrie_allocate(void) {
   struct ptrie *pt = malloc(sizeof(struct ptrie));
   if (pt == NULL) {
      return NULL;
   }
   pt->root = node_allocate();
   if (pt->root == NULL) {
      free(pt);
      return NULL;
   }
   return pt;
   
}

void ptrie_free(struct ptrie *pt) {
   node_free(pt->root);
   free(pt);
}

int ptrie_add(struct ptrie *pt, const char *str) {
   struct node *n = pt->root; // start from the root node
   for (int i = 0; str[i] != '\0'; i++) { // iterate over each character in the string
      int off = ptrie_char2off(str[i]); // convert character to offset
      if (off == -1) { // invalid character
         return -1;
      }
      if (!n->nxt[off]) { // if next node does not exist, allocate a new one
         n->nxt[off] = node_allocate();
      }
      n = n->nxt[off]; // move to next node
   }
   if (!n->str) { // if the string is not already present in the trie
      n->str = strdup(str); // duplicate the string and store it in the node
      if (!n->str) { // check for memory allocation failure
         return -1;
      }
   }
   n->count++; // increment the count for the node
   if (n->count > n->max) { // update the maximum count if necessary
      n->max = n->count;
   }
   return 0; // success
}

char *ptrie_autocomplete(struct ptrie *pt, const char *str) {
   if (!pt || !pt->root || !str) {
      return NULL;
   }
   
   struct node *n = pt->root;
   for (int i = 0; str[i] != '\0'; i++) {
      int off = ptrie_char2off(str[i]);
      if (!n->nxt[off]) {
         return strdup(str); // no autocomplete options found, return the original string
      }
      n = n->nxt[off];
   }
   
   if (n->str) {
      // the input string is an exact match for a word in the trie
      return strdup(n->str);
   } else {
      // the input string is a prefix for one or more words in the trie
      int max_count = -1;
      char *result = NULL;
      
      // traverse the subtree rooted at n to find the most frequent word that starts with the given prefix
      struct node *stack[256];
      int top = 0;
      stack[top++] = n;
      while (top > 0) {
         struct node *node = stack[--top];
         if (node->str) {
            if (node->count > max_count || (node->count == max_count && strcmp(node->str, result) < 0)) {
               max_count = node->count;
               if (result) {
                  free(result);
               }
               result = strdup(node->str);
            }
         }
         for (int i = 0; i < 256; i++) {
            if (node->nxt[i]) {
               stack[top++] = node->nxt[i];
            }
         }
      }
      
      if (!result) {
         return strdup(str); // no words starting with the given prefix were found, return the original string
      }
      return result;
   }
}

void ptrie_print_helper(struct node *n) {
   if (!n) {
      return;
   }
   
   if (n->str) {
      printf("%s (%d)\n", n->str, n->count);
   }
   
   for (int i = 0; i < 256; i++) {
      ptrie_print_helper(n->nxt[i]);
   }
}

void ptrie_print(struct ptrie *pt) {
   ptrie_print_helper(pt->root);
}