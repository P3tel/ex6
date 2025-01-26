// Name: Ori Bahat-Petel
// ID: 331753830
// Assignment: ex6
#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define INT_BUFFER 128
#define MAX_POKEDEX 151

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------
Queue* createQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}
int isQueueEmpty(Queue* q)
{
    if (q->front == NULL && q->rear == NULL) {
        return 1;
    }
    return 0;
}
void enqueue(Queue* q, PokemonNode* newNode)
{
    QueueNode *newQueueNode = (QueueNode*)malloc(sizeof(QueueNode));
    newQueueNode->pokemonNode = newNode;
    newQueueNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newQueueNode;
        return;
    }
    q->rear->next = newQueueNode;
    q->rear = newQueueNode;
}
PokemonNode* dequeue(Queue* q)
{
    if (isQueueEmpty(q)) 
    {
        return NULL;
    }
    QueueNode *temp = q->front;
    PokemonNode *node = temp->pokemonNode;
    q->front = q->front->next;
    if (q->front == NULL)
     {
        q->rear = NULL;
    }
    free(temp);
    return node;
}
void clearInputReader()
{
    char c = getchar();
    while (c != '\n' && c != ' ')
        c = getchar();
}
void printPokemonNode(PokemonNode *node)
{
    printf("ID: %d, Name: %s, Type: %s, HP: %d,",node->data->id,node->data->name,getTypeName(node->data->TYPE),node->data->hp);
    printf(" Attack: %d, Can Evolve: %s\n",node->data->attack,EvoStatus(node->data->CAN_EVOLVE));
}
const char *EvoStatus(EvolutionStatus evo)
{
  switch(evo)
  {
  case CANNOT_EVOLVE:
    return "no";
  default:
    return "yes";
  }
}
void displayAlphabetical(PokemonNode *root)
{
    if (root == NULL) 
    {
        printf("Tree is empty.\n");
        return;
    }
    NodeArray na;
    initNodeArray(&na, 10);
    collectAll(root, &na);
    qsort(na.nodes, na.size, sizeof(PokemonNode *), compareByNameNode);
    printf("Pokemon in alphabetical order:\n");
    for (int i = 0; i < na.size; i++) 
    {
       printPokemonNode(na.nodes[i]);
    }
    free(na.nodes);
}
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
        return;
    postOrderGeneric(root->left,visit);
    postOrderGeneric(root->right,visit);
    visit(root);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
        return;
    inOrderGeneric(root->left,visit);
    visit(root);
    inOrderGeneric(root->right,visit);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
        return;
    visit(root);
    preOrderGeneric(root->left,visit);
    preOrderGeneric(root->right,visit);
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (root == NULL)
        return;
    Queue *queue = createQueue();
    enqueue(queue, root);
    while (!isQueueEmpty(queue))
    {
        PokemonNode *current = dequeue(queue);
        visit(current);
        if (current->left)
            enqueue(queue, current->left);
        if (current->right)
            enqueue(queue, current->right);
    }
    free(queue);
}
void postOrderTraversal(PokemonNode *root)
{
    postOrderGeneric(root,printPokemonNode);
}

void inOrderTraversal(PokemonNode *root)
{
    inOrderGeneric(root,printPokemonNode);
}

void preOrderTraversal(PokemonNode *root)
{
    preOrderGeneric(root,printPokemonNode);
}

void displayBFS(PokemonNode *root)
{
    BFSGeneric(root,printPokemonNode);
}
//
void initNodeArray(NodeArray *na, int cap)
{
    na->nodes = (PokemonNode **)malloc(cap * sizeof(PokemonNode *));
    na->size = 0;
    na->capacity = cap;
}
void addNode(NodeArray *na, PokemonNode *node)
{
    if (na->size >= na->capacity)
	{
        na->capacity *= 2;
        na->nodes = (PokemonNode **)realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
    }
    na->nodes[na->size++] = node;
}
void collectAll(PokemonNode *root, NodeArray *na)
{
    if (root == NULL)
        return;

    addNode(na, root);
    collectAll(root->left, na);
    collectAll(root->right, na);
}
int compareByNameNode(const void *a, const void *b)
{
    PokemonNode *nodeA = *(PokemonNode **)a;
    PokemonNode *nodeB = *(PokemonNode **)b;
    const char *nameA = nodeA->data->name;
    const char *nameB = nodeB->data->name;
    while (*nameA && *nameB)
    {
        char upperA = toupper((unsigned char)*nameA);
        char upperB = toupper((unsigned char)*nameB);
        if (upperA != upperB)
            return (unsigned char)upperA - (unsigned char)upperB;
        nameA++;
        nameB++;
    }
    return (unsigned char)*nameA - (unsigned char)*nameB;
}
//
void freeAllOwners()
{
    if (ownerHead == NULL)
        return;
    OwnerNode *current = ownerHead;
    OwnerNode *nextNode;
    do 
    {
        nextNode = current->next;
        freeOwnerNode(current);  
        current = nextNode; 
    } while (current != ownerHead); 
    ownerHead = NULL; 
}
//
bool isNameUnique(OwnerNode *temp,char *ownerName) 
{
    if(temp == NULL)
      return true;
    do
    {
        if (strcmp(temp->ownerName, ownerName) == 0) 
        {
            return false;
        }
        temp = temp->next;
    }while (temp != ownerHead); 
    return true;
}
void swapOwnerData(OwnerNode *a, OwnerNode *b)
{
    if (a == NULL || b == NULL)
        return;
    char *tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;
    PokemonNode *tempRoot = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempRoot;
}
void linkOwnerInCircularList(OwnerNode *newOwner)
{
    OwnerNode *temp = ownerHead;
    if (newOwner == NULL)
        return;
    if (ownerHead == NULL) 
    {
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }
    else if(isNameUnique(temp,newOwner->ownerName))
    {
        OwnerNode *tail = ownerHead->prev;
        newOwner->next = ownerHead;
        newOwner->prev = tail;
        tail->next = newOwner;
        ownerHead->prev = newOwner;
    }
    else
      freeOwnerNode(newOwner);
}
OwnerNode *findOwnerByName(const char *name)
{
    if (ownerHead == NULL || name == NULL)
        return NULL;
    OwnerNode *current = ownerHead;
    do {
        if (strcmp(current->ownerName, name) == 0)
            return current;

        current = current->next;
    } while (current != ownerHead);
    return NULL;
}
void removeOwnerFromCircularList(OwnerNode *target)
{
    if (target == NULL || ownerHead == NULL)
        return;
    if (target->next == target) 
    {
        ownerHead = NULL;
    }
    else 
    {
        target->prev->next = target->next;
        target->next->prev = target->prev;
        if (ownerHead == target)
            ownerHead = target->next;
    }
    freeOwnerNode(target);
}
//
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode)
{
    if (root == NULL)
    {
        return newNode;
    }
    if (newNode->data->id < root->data->id)
        root->left = insertPokemonNode(root->left, newNode);
    else if (newNode->data->id > root->data->id)
        root->right = insertPokemonNode(root->right, newNode);
    else
        free(newNode);
    return root;
}
PokemonNode *searchPokemonBFS(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;
    Queue *queue = createQueue();
    enqueue(queue, root);
    while (!isQueueEmpty(queue))
    {
        PokemonNode *current = dequeue(queue);
        if (current->data->id == id)
        {
            free(queue);
            return current;
        }
        if (current->left)
            enqueue(queue, current->left);
        if (current->right)
            enqueue(queue, current->right);
    }
    free(queue);
    return NULL;
}
PokemonNode *removeNodeBST(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;
    if (id < root->data->id) 
    {
        root->left = removeNodeBST(root->left, id);
    } 
    else if (id > root->data->id)
    {
        root->right = removeNodeBST(root->right, id);
    } 
    else 
    {
        if (root->left == NULL)
        {
            PokemonNode *temp = root->right;
            freePokemonNode(root);
            return temp;
        }
		    else if (root->right == NULL)
        {
            PokemonNode *temp = root->left;
            freePokemonNode(root);
            return temp;
        } 
		    else
        {
            PokemonNode *temp = root->right;
            while(temp->left != NULL)
                temp = temp->left;
            temp->left = root->left;
            PokemonNode *newRoot = root->right; 
            freePokemonNode(root);
            return newRoot;
        }
    }
    return root;
}
PokemonNode *removePokemonByID(PokemonNode *root, int id)
{
    root = removeNodeBST(root, id);
    return root;
}
//
void freePokemonNode(PokemonNode *node)
{
    node->right = NULL;
    node->left = NULL;
    free(node);
}
void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");
    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}
void addPokemon(OwnerNode *owner)
{
    int ID = readIntSafe("Enter ID to add: ");
    if(ID <= 0 || ID > MAX_POKEDEX)
    {
      printf("Invalid input.");
      return;
    }
    if(owner->pokedexRoot == NULL)
    {
      PokemonNode *newNode = createPokemonNode(&pokedex[ID-1]);
      owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot,newNode);
      PokemonData *data1 = newNode->data;
      printf("Pokemon %s (ID %d) added.",data1->name,ID);
      return;
    }
    if(searchPokemonBFS(owner->pokedexRoot,ID) == NULL)
    {
      PokemonNode *newNode = createPokemonNode(&pokedex[ID-1]);
      owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot,newNode);
      PokemonData *data1 = &pokedex[ID-1];
      printf("Pokemon %s (ID %d) added.",data1->name,ID);
    }
    else
      printf("Pokemon with ID %d is already in the Pokedex. No changes made.",ID);  
}
void freePokemon(OwnerNode *owner)
{
    int ID = readIntSafe("Enter ID to release: ");
    if(searchPokemonBFS(owner->pokedexRoot,ID) != NULL)
    {
      owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, ID);
      printf("Pokemon with ID %d has been released.",ID); 
    }  
    else    
      printf("No pokemon with ID %d has been found.",ID);
}
void pokemonFight(OwnerNode *owner)
{
    if(owner->pokedexRoot == NULL)
    {
      printf("Pokedex is empty");
      return;
    }
    int ID1,ID2;
    double score1,score2;
    ID1 = readIntSafe("Enter ID of the first Pokemon: ");
    ID2 = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode *finder1 = searchPokemonBFS(owner->pokedexRoot,ID1);
    PokemonNode *finder2 = searchPokemonBFS(owner->pokedexRoot,ID2);
    score1 = 1.2 * finder1->data->hp + 1.5 * finder1->data->attack;
    score2 = 1.2 * finder2->data->hp + 1.5 * finder2->data->attack;
    printf("Pokemon 1: %s (Score = %.2lf)\n", finder1->data->name,score1);
    printf("Pokemon 2: %s (Score = %.2lf)\n", finder2->data->name,score2);
    if(score1 > score2)
        printf("%s wins!",finder1->data->name);
    else if(score1 < score2)
        printf("%s wins!",finder2->data->name);
    else
        printf("Its a tie!");
}
void evolvePokemon(OwnerNode *owner)
{
    if(owner->pokedexRoot == NULL)
    {
      printf("Cannot evolve. Pokedex empty.\n");
      return;
    }
    int ID;
    ID = readIntSafe("Enter ID of Pokemon to evolve: ");
	if(searchPokemonBFS(owner->pokedexRoot,ID) != NULL)
	{
		owner->pokedexRoot = removePokemonByID(owner->pokedexRoot,ID);
		printf("Removing Pokemon %s (ID %d).\n",pokedex[ID-1].name,ID);
		if(searchPokemonBFS(owner->pokedexRoot,ID+1) == NULL)
		{
			PokemonNode *newNode = createPokemonNode(&pokedex[ID]);
			owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot,newNode);
			printf("Pokemon evolved from %s (ID %d) to %s (ID %d).",pokedex[ID-1].name,ID,pokedex[ID].name,ID+1);
		}
		else
		{
			printf("Pokemon with ID %d is already in the Pokedex. No additions made.",ID+1);
		}
	}
	else
	{
		printf("Pokemon is not in this pokedex. No changes made.");
	}
    
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    if (ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }
    int choice;
    printf("\nExisting Pokedexes:\n");
    OwnerNode *cur = ownerHead;
    int count = 1;
    do {
        printf("%d. %s\n",count,cur->ownerName);
        cur = cur->next;
        count++;
    } while (cur != ownerHead); 
    
    choice = readIntSafe("Choose a Pokedex by number: ");

    for(int i = 1; i < choice; i++)
        cur = cur->next;
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);
    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

PokemonNode* createPokemonNode(PokemonData *data) 
{
    PokemonNode *newNode = (PokemonNode *)malloc(sizeof(PokemonNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

OwnerNode* createOwner(char *ownerName, PokemonNode *starter) 
{
    OwnerNode *newOwner = (OwnerNode *) malloc(sizeof(OwnerNode));
    newOwner->ownerName = myStrdup(ownerName);
    newOwner->next = newOwner->prev = NULL;
    newOwner->pokedexRoot = starter;
    return newOwner;
}

void openPokedexMenu()
{
    int choice;
    OwnerNode *temp = ownerHead;
    printf("Your name: ");
    char *ownerName1 = getDynamicInput();
    if(!isNameUnique(temp,ownerName1))
    {
      printf("Owner '%s' already exists. Not creating a new Pokedex.",ownerName1);
      free(ownerName1);
      return;
    }
    printf("Choose Starter: \n");
    printf("1. Bulbasaur \n");
    printf("2. Charmander \n");
    printf("3. Squirtle \n");
    choice = readIntSafe("Your choice: ");
    PokemonNode *starter1 = createPokemonNode(&pokedex[3*choice-3]);
    OwnerNode *newOwner = createOwner(ownerName1,starter1);
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.",newOwner->ownerName,newOwner->pokedexRoot->data->name);   
}
//
void freePokemonTree(PokemonNode *root)
{
    if (root == NULL)
        return;
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);    
}
void deletePokedex()
{
    if (ownerHead == NULL)
    {
        printf(" No existing Pokedexes to delete.\n");
        return;
    }
    int choice;
    printf("\n=== Delete a Pokedex ===\n");
    OwnerNode *cur = ownerHead;
    int count = 1;
    do {
        printf("%d. %s\n",count,cur->ownerName);
    } while (cur != ownerHead); 
    
    choice = readIntSafe("Choose a Pokedex to delete by number: ");
    for(int i = 1; i < choice; i++)
        cur = cur->next;
    printf("\nDeleting %s's entire Pokedex...\n", cur->ownerName);    
    removeOwnerFromCircularList(cur);
    printf("Pokedex deleted.");
}
//
void freeOwnerNode(OwnerNode *owner)
{
    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    owner->next = NULL;
    owner->prev = NULL;
    free(owner);
}
void mergePokedexMenu()
{
    if(ownerHead == NULL || ownerHead->next == ownerHead)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
	  printf("\n=== Merge Pokedexes ===\n");
    OwnerNode *owner1;
    OwnerNode *owner2;
    char *name1,*name2;
    printf("Enter name of first owner:");
    name1 = getDynamicInput();
    clearInputReader();
    printf("Enter name of second owner:");
    name2 = getDynamicInput();
    clearInputReader();
    owner1 = findOwnerByName(name1);
    owner2 = findOwnerByName(name2);
	  owner1->pokedexRoot = mergePokemonTrees(owner1->pokedexRoot, owner2->pokedexRoot);
	  removeOwnerFromCircularList(owner2);
    printf("\nMerge completed.\n");
    printf("Owner '%s' has been removed after merging.\n",name2);
    free(name1);
    free(name2);
}
PokemonNode *mergePokemonTrees(PokemonNode *root1, PokemonNode *root2)
{
    if (root1 == NULL) return root2;
    if (root2 == NULL) return root1;
	  if (root1 == root2) return root1;
      collectAndInsert(root2, &root1);
    return root1;
}

void collectAndInsert(PokemonNode *root, PokemonNode **targetRoot)
{
    if (root == NULL)
        return;
    *targetRoot = insertPokemonNode(*targetRoot, createPokemonNode(&pokedex[(root->data->id)-1]));
    collectAndInsert(root->left, targetRoot);
    collectAndInsert(root->right, targetRoot);
}
//
void sortOwners()
{
    if (ownerHead == NULL || ownerHead->next == ownerHead) 
    {
         printf("0 or 1 owners only => no need to sort.\n");
        return;
    }    
    int swapped;
    OwnerNode *current;
    OwnerNode *last = NULL;
    do {
        swapped = 0;
        current = ownerHead;
        while (current->next != last && current->next != ownerHead) {
            if (strcmp(current->ownerName, current->next->ownerName) > 0) {
                char *tempName = current->ownerName;
                current->ownerName = current->next->ownerName;
                current->next->ownerName = tempName;

                PokemonNode *tempPokedex = current->pokedexRoot;
                current->pokedexRoot = current->next->pokedexRoot;
                current->next->pokedexRoot = tempPokedex;

                swapped = 1;
            }
            current = current->next;
        }
        last = current;
    } while (swapped);
    printf("Owners sorted by name.");
}
//
void printOwnersCircular()
{
    if(ownerHead == NULL)
    {
      printf("No owners.");
      return;
    }
    char  direction;
    int prints;
    printf("Enter direction (F or B): ");
    scanf("%c", &direction);
    clearInputReader();
    prints = readIntSafe("How many prints? ");
    OwnerNode *temp = ownerHead;
    switch(direction)
    {
    case 'f':
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s\n",i,temp->ownerName);
            temp = temp->next;
        }
        break;
    case 'F':
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s\n",i,temp->ownerName);
            temp = temp->next;
        }
        break;
    default:
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s\n",i,temp->ownerName);
            temp = temp->prev;
        }
        break;
    }
}
//
// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
