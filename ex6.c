#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define INT_BUFFER 128
#define MAX_NAME_LENGTH 21

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
void printPokemonNode(PokemonNode *node)
{
    printf("ID: %d, Name: %s, Type: %s, HP: %d,",node->data->id,node->data->name,getTypeName(node->data->TYPE),node->data->hp);
    printf(" Attack: %d, Can Evolve: %s\n",node->data->attack,EvoStatus[node->data->CAN_EVOLVE]);
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
    postOrderGeneric(root->left);
    postOrderGeneric(root->right);
    visit(root);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
        return;
    inOrderGeneric(root->left);
    visit(root);
    inOrderGeneric(root->right);
}

void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
        return;
    visit(root);
    preOrderGeneric(root->left);
    preOrderGeneric(root->right);
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit)
{
    if (!root)
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

    freeQueue(queue);
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
    if (na->size >= na->capacity) {
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
    return strcmp(nodeA->data->name, nodeB->data->name);
}
//
void freeAllOwners()
{
    if (ownerHead == NULL)
        return;
    Owner *current = ownerHead;
    Owner *nextNode;
    do 
    {
        nextNode = current->next;
        freeOwnerNode(current);  
        current = nextNode; 
    } while (current != ownerHead); 
    ownerHead = NULL; 
}
//
bool isNameUnique(Owner *temp, const char *ownerName) 
{
    int count = 0;
    do
    {
        if (strcmp(temp->name, ownerName) == 0) 
        {
            return false;
        }
        temp = temp->next;
    }while (temp != ownerHead) 
    return count == 0;
}
void swapOwnerData(OwnerNode *a, OwnerNode *b)
{
    if (a == NULL || b == NULL)
        return;
    char *tempName = a->name;
    a->name = b->name;
    b->name = tempName;
    PokemonNode *tempRoot = a->pokemonRoot;
    a->pokemonRoot = b->pokemonRoot;
    b->pokemonRoot = tempRoot;
}
void linkOwnerInCircularList(OwnerNode *newOwner)
{
    OwnerNode temp = ownerHead;
    if (newOwner == NULL)
        return;
    if (ownerHead == NULL) {
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
        if (strcmp(current->name, name) == 0)
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
        return newNode;
    if (newNode->data->id < root->data->id)
        root->left = insertPokemonNode(root->left, newNode);
    else if (newNode->data->id > root->data->id)
        root->right = insertPokemonNode(root->right, newNode);
    else {
        free(newNode->data->name);
        free(newNode->data);
        free(newNode);
    }
    return root;
}
PokemonNode *searchPokemonBFS(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;

    Queue *queue = createQueue();
    enqueue(queue, root);

    while (!isQueueEmpty(queue)) {
        PokemonNode *current = dequeue(queue);
        if (current->data->id == id) {
            destroyQueue(queue);
            return current;
        }
        if (current->left)
            enqueue(queue, current->left);
        if (current->right)
            enqueue(queue, current->right);
    }
    destroyQueue(queue);
    return NULL;
}
PokemonNode *removeNodeBST(PokemonNode *root, int id)
{
    if (root == NULL)
        return NULL;

    if (id < root->data->id) 
    {
        root->left = removeNodeBST(root->left, id);
    } else if (id > root->data->id)
    {
        root->right = removeNodeBST(root->right, id);
    } else 
    {
        if (root->left == NULL)
        {
            PokemonNode *temp = root->right;
            freePokemonNode(root);
            return temp;
        } else if (root->right == NULL)
        {
            PokemonNode *temp = root->left;
            freePokemonNode(root);
            return temp;
        } else
        {
            PokemonNode *temp = root->right;
            while (temp->left)
                temp = temp->left;
            root->data = temp->data;
            root->right = removeNodeBST(root->right, temp->data->id);
        }
    }
    return root;
}
PokemonNode *removePokemonByID(PokemonNode *root, int id)
{
    PokemonNode *foundNode = searchPokemonBFS(root, id);
    if (foundNode != NULL) {
        root = removeNodeBST(root, id);
    }
    return root;
}
//
void freePokemonNode(PokemonNode *node)
{
    free(node->data->name);
    free(node->data);
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

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
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
    int flag = 1;
    int ID = readIntSafe("Enter ID to add: \n");
    PokemonNode *adder = owner->pokedexRoot;
    while(flag)
    {
        if(adder->data->id == ID)
        {
            printf("Pokemon with ID %d is already in the Pokedex. No changes made.",ID);
            flag = 0;
        }
        else if(adder->data->id < ID)
        {
            if(adder->right == null)
            {
                adder->right = pokedex[ID-1];
                printf("Pokemon %s (ID %d) added.",pokedex[ID-1]->name,pokedex[ID-1]->id)
                flag = 0;
            }
            else
            {
                adder = adder->right;
            }
        }
        else
        {
            if(adder->left == null)
            {
                adder->left = pokedex[ID-1];
                printf("Pokemon %s (ID %d) added.",pokedex[ID-1]->name,pokedex[ID-1]->id)
                flag = 0;
            }
            else
            {
                adder = adder->left;
            }    
        }
    }
}
void freePokemon(OwnerNode *owner)
{
    int flag = 1;
    int ID = readIntSafe("Enter ID to release:");
    printf("\n");
    PokemonNode *deleter = owner->pokedexRoot;
    while(flag)
    {
        if(deleter == null)
        {
            printf("No Pokemon to release.\n");
            flag = 0;
        }
        else if (deleter->data->id == ID)
        {
            PokemonNode *leftSubtree = deleter->left;
            PokemonNode *rightSubtree = deleter->right;
            PokemonNode *mergedTree = mergePokemonTrees(leftSubtree, rightSubtree);
            if (parent == NULL)
            {
                owner->pokedexRoot = mergedTree;
            }
            else if (parent->left == deleter)
            {
                parent->left = mergedTree;
            }
            else
            {
                parent->right = mergedTree;
            }
            freePokemonNode(deleter);
            printf("Pokemon with ID %d has been released.\n", ID);
            flag = 0;
        }
        else if (deleter->data->id < ID)
        {
            parent = deleter;
            deleter = deleter->right;
        }
        else
        {
            parent = deleter;
            deleter = deleter->left;
        }
    }     
}
void pokemonFight(OwnerNode *owner)
{
    int ID1,ID2;
    float score1,score2;
    ID1 = readIntSafe("Enter ID of the first Pokemon:");
    printf("\n");
    ID2 = readIntSafe("Enter ID of the second Pokemon:");
    printf("\n");
    PokemonNode *finder1 = owner->pokedexRoot;
    PokemonNode *finder2 = owner->pokedexRoot;
    while(1)
    {
        if(finder1 == null)
        {
            printf("One or both Pokemon IDs not found.\n");
            return;
        }
        else if(finder1->data->id == ID1)
        {
            break;
        }
        else if(finder1->data->id < ID1)
        {
            finder1 = finder1->right;
        }
        else
        {
            finder1 = finder1->left;
        }
    }
    while(1)
    {
        if(finder2 == null)
        {
            printf("One or both Pokemon IDs not found.\n");
            return;
        }
        else if(finder2->data->id == ID2)
        {
            break;
        }
        else if(finder2->data->id < ID2)
        {
            finder2 = finder2->right;
        }
        else
        {
            finder2 = finder2->left;
        }
    }
    score1 = 1.2 * finder1->data->hp + 1.5 * finder1->data->attack;
    score2 = 1.2 * finder2->data->hp + 1.5 * finder2->data->attack;
    printf("Pokemon 1: %s (Score = %.2lf", finder1->data->name);
    printf("Pokemon 2: %s (Score = %.2lf", finder2->data->name);
    if(score1 > score2)
        printf("%s wins!\n",finder1->data->name);
    else if(score1 < score2)
        printf("%s wins!\n",finder2->data->name);
    else
        printf("Its a tie!\n");
}
void evolvePokemon(OwnerNode *owner)
{
    int ID;
    ID = readIntSafe("Enter ID of Pokemon to evolve: \n");
    PokemonNode *evolver = owner->pokedexRoot;
    while (1)
    {
        if (evolver == NULL)
        {
            printf("No Pokemon with ID %d found.\n", ID);
            return;
        }
        else if (evolver->data->id == ID)
        {
            if (evolver->data->CAN_EVOLVE)
            {
                PokemonNode *existingEvolved = searchPokemonBFS(owner->pokedexRoot, ID + 1);
                if (existingEvolved != NULL)
                {
                    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, ID + 1);
                    printf("Duplicate evolved Pokémon with ID %d removed.\n", ID + 1);
                }
                evolver->data = pokedex[ID + 1];
                printf("Pokemon with ID %d has evolved into ID %d.\n", ID, ID + 1);
                return;
            }
            else
            {
                printf("This Pokemon cannot be evolved.\n");
                return;
            }
        }
        else if (evolver->data->id < ID)
        {
            evolver = evolver->right;
        }
        else
        {
            evolver = evolver->left;
        }
    }
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    int choice;
    printf("\nExisting Pokedexes:\n");
    if (ownerHead == NULL) {
        printf("The list is empty.\n");
        return;
    }
    OwnerNode *cur = ownerHead;
    int count = 1;
    do {
        printf("%d. %s",count,cur->ownerName);
    } while (cur != ownerHead); 
    
    choice = readIntSafe("Choose a Pokedex by number: \n");
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);
    
    for(int i = 1; i < choice; i++)
        cur = cur->next;
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

PokemonNode* createPokemonNode(const PokemonData *data) 
{
    PokemonNode *newNode = (PokemonNode*)malloc(sizeof(PokemonNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

OwnerNode* createOwner(char *ownerName, int starterChoice) 
{
    OwnerNode *newOwner = (OwnerNode*) malloc(sizeof(OwnerNode));
    newOwner->ownerName = myStrdup(ownerName);
    newOwner->next = newOwner->prev = NULL;
    newOwner->pokedexRoot = createPokemonNode(&pokedex[starterChoice - 1]);
    return newOwner;
}

void openPokedexMenu()
{
    int choice;
    OwnerNode *newOwner = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (ownerHead == NULL)
    {
        newOwner->next = newOwner->prev = newOwner;
        ownerHead = newOwner;
        return;
    }
    OwnerNode *last = ownerHead->prev;
    last->next = newOwner;
    newOwner->prev = last;
    newOwner->next = ownerHead;
    ownerHead->prev = newOwner;
    printf("Your name: ");
    newOwner->ownerName = getDynamicInput()
    printf("\n");
    printf("Choose Starter: \n");
    printf("1. Bulbasaur \n");
    printf("2. Charmander \n");
    printf("3. Squirtle \n");
    choice = readIntSafe("Your choice: ");
    printf("\n");
    PokemonNode *starter = (PokemonNode *)malloc(sizeof(PokemonNode));
    newOwner->pokedexRoot = starter;
    starter->data = pokedex[choice-1];
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
    int choice;
    printf("\n=== Delete a Pokedex ===\n");
    if (ownerHead == NULL) {
        printf("The list is empty.\n");
        return;
    }
    OwnerNode *cur = ownerHead;
    int count = 1;
    do {
        printf("%d. %s",count,cur->ownerName);
    } while (cur != ownerHead); 
    
    choice = readIntSafe("Choose a Pokedex to delete by number:");
    for(int i = 1; i < choice; i++)
        cur = cur->next;
    printf("\nDeleting %s's entire Pokedex...\n", cur->ownerName);    
    removeOwnerFromCircularList(cur);
    free(cur);
    printf("Pokedex deleted.")
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
    if(ownerHead == null || ownerHead->next == ownerHead)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
    OwnerNode *owner1;
    OwnerNode *owner2;
    char name1[MAX_NAME_LENGTH],name2[MAX_NAME_LENGTH];
    printf("Enter name of first owner:");
    scanf("%s",name1);
    trimWhitespace();
    printf("\nEnter name of second owner:");
    scanf("%s",name2);
    trimWhitespace();
    owner1 = findOwnerByName(name1);
    owner2 = findOwnerByName(name2);
    owner1->pokemonRoot = mergePokemonTrees(owner1->pokemonRoot, owner2->pokemonRoot);
    removeOwnerFromCircularList(owner2);
    printf("\nMerge completed.\n");
    printf("Owner '%s' has been removed after merging.\n",name2);
}
PokemonNode *mergePokemonTrees(PokemonNode *root1, PokemonNode *root2)
{
    if (root1 == NULL) return root2;
    if (root2 == NULL) return root1;
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
    char  direction;
    int prints;
    printf("Enter direction (F or B): ");
    scanf("%c", &direction);
    trimWhitespace();
    prints = readIntSafe("How many prints? ");
    OwnerNode *temp = ownerHead;
    switch(direction)
    {
    case 'f':
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s",i,temp->ownerName);
            temp = temp->next;
        }
        break;
    case 'F':
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s",i,temp->ownerName);
            temp = temp->next;
        }
        break;
    default:
        for(int i = 1; i <= prints; i++)
        {
            printf("[%d] %s",i,temp->ownerName);
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
