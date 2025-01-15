#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

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

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    Queue *q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q)) {
        PokemonNode *cur = dequeue(q);
        visit(cur);
        if(cur->left != NULL) {
            enqueue(q, cur->left);
        }
        if(cur->right != NULL) {
            enqueue(q, cur->right);
        }
    }
    free(q);
}
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
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
        BFSGeneric(owner->pokedexRoot, printPokemonNode);
        //displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderGeneric(owner->pokedexRoot, printPokemonNode);
        //preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderGeneric(owner->pokedexRoot, printPokemonNode);
        //inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderGeneric(owner->pokedexRoot, printPokemonNode);
        //postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        //displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
Queue* createQueue() {
    //LEAK
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void enqueue(Queue *q, PokemonNode *node) {
    if (!q) {
        return;
    }
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        return;
    }
    newNode->data = node;
    newNode->next = NULL;

    if (q->tail == NULL) {
        q->head = newNode;
        q->tail = newNode;
        return;
    }

    q->tail->next = newNode;
    q->tail = newNode;
}

PokemonNode *dequeue(Queue *q) {
    if (!q || !q->head)
        return NULL;
    QueueNode *temp = q->head;
    PokemonNode *result = temp->data;
    q->head = q->head->next;

    if(q->head == NULL)
        q->tail = NULL;
    free(temp);
    return result;
}

int isEmpty(Queue *q) {
    return (q->head == NULL);
}

void freeQueue(Queue *q) {
    while (!isEmpty(q)) {
        dequeue(q);
    }
    free(q);
}

void displayBFS(PokemonNode *root) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    Queue *q = createQueue();
    enqueue(q, root);
    while (!isEmpty(q)) {
        PokemonNode *cur = dequeue(q);
        printPokemonNode(cur);
        if(cur->left != NULL) {
            enqueue(q, cur->left);
        }
        if(cur->right != NULL) {
            enqueue(q, cur->right);
        }
    }
    free(q);
}


void preOrderTraversal(PokemonNode *root) {
    if (root == NULL)
        return;
    printPokemonNode(root);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}
void inOrderTraversal(PokemonNode *root) {
    if (root == NULL)
        return;
    inOrderTraversal(root->left);
    printPokemonNode(root);
    inOrderTraversal(root->right);
}
void postOrderTraversal(PokemonNode *root) {
    if (root == NULL)
        return;
    postOrderTraversal(root->left);
    postOrderTraversal(root->right);
    printPokemonNode(root);
}
void displayAlphabetical(PokemonNode *root) {

}

void pokemonFight(OwnerNode *owner) {
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    int first = readIntSafe("Enter ID of the first pokemon: ");
    int second = readIntSafe("Enter ID of the second pokemon: ");
    PokemonNode *a = searchPokemonBFS(owner->pokedexRoot, first);
    PokemonNode *b = searchPokemonBFS(owner->pokedexRoot, second);
    if(!a || !b) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    double scoreA = a->data->attack * 1.5 + a->data->hp * 1.2;
    double scoreB = b->data->attack * 1.5 + b->data->hp * 1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", a->data->name, scoreA);
    printf("Pokemon 2: %s (Score = %.2f)\n", b->data->name, scoreB);
    double diff = scoreA - scoreB;
    if(diff < 0.0)
        printf("%s wins!\n", b->data->name);
    else if(diff > 0.0)
        printf("%s wins!\n", a->data->name);
    else
        printf("It's a tie!\n");
}

void evolvePokemon(OwnerNode *owner) {
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    int choice = readIntSafe("Enter ID of pokemon to evolve: ");
    if(!searchPokemonBFS(owner->pokedexRoot, choice)) {
        printf("No Pokemon with ID %d found.\n", choice);
        return;
    }
    if(!pokedex[choice - 1].CAN_EVOLVE) {
        printf("%s (ID %d) cannot evolve.\n", pokedex[choice - 1].name, pokedex[choice - 1].id);
        return;
    }

    //check if the pokemon is in the pokedex already
    if(searchPokemonBFS(owner->pokedexRoot, choice+1)) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n", pokedex[choice].id,
                                            pokedex[choice].name, pokedex[choice-1].name,pokedex[choice-1].id);
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, choice);
        return;
    }

    //allocate data for the evolution of the pokemon
    PokemonNode *insert = (PokemonNode*)malloc(sizeof(PokemonNode));
    if (!insert) {
        printf("Memory allocation failed.\n");
        return;
    }

    insert->data = (PokemonData*)malloc(sizeof(PokemonData));
    if (!insert->data) {
        printf("Memory allocation failed.\n");
        return;
    }

    //remove the original pokemon and add it's evolution
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, choice);
    *insert->data = pokedex[(choice)];
    insert->left = NULL;
    insert->right = NULL;
    //if there are no pokemon in the pokedex, allocate memory for the root
    if(owner->pokedexRoot == NULL) {
        owner->pokedexRoot = (PokemonNode*)malloc(sizeof(PokemonNode));
        if (!owner->pokedexRoot) {
            printf("Memory allocation failed.\n");
            return;
        }

        owner->pokedexRoot->data = (PokemonData*)malloc(sizeof(PokemonData));
        if (!owner->pokedexRoot->data) {
            printf("Memory allocation failed.\n");
            return;
        }
        owner->pokedexRoot->data = insert->data;
        owner->pokedexRoot->left = NULL;
        owner->pokedexRoot->right = NULL;
        owner->pokedexRoot->parent = NULL;
        printf("Pokemon evolved from %s (ID %d) to %s(ID %d).\n",pokedex[choice-1].name,pokedex[choice-1].id,
                                                                        pokedex[choice].name,pokedex[choice].id);
        return;
    }
    insertPokemonNode(owner->pokedexRoot, insert);
    printf("Pokemon evolved from %s (ID %d) to %s(ID %d).\n",pokedex[choice-1].name,pokedex[choice-1].id,
                                                                    pokedex[choice].name,pokedex[choice].id);
}

PokemonNode *findMinNode(PokemonNode *root) {
    while (root && root->left != NULL)
        root = root->left;
    return root;
}

PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    if(root->data->id > newNode->data->id) {
        if(root->left != NULL)
            return insertPokemonNode(root->left, newNode);
        newNode->parent = root;
        root->left = newNode;
        return root;
    }
    if(root->data->id < newNode->data->id) {
        if(root->right != NULL)
            return insertPokemonNode(root->right, newNode);
        newNode->parent = root;
        root->right = newNode;
        return root;
    }
    printf("Could not insert pokemon.\n");
    return NULL;
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    if(!root)
        return NULL;
    if(root->data->id == id)
        return root;
    if(root->data->id > id)
        return searchPokemonBFS(root->left, id);
    return searchPokemonBFS(root->right, id);
}

PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    if(!root)
        return NULL;
    if(root->data->id > id)
        root->left = removeNodeBST(root->left, id);
    else if(root->data->id < id)
        root->right = removeNodeBST(root->right, id);
    else {
        //if the node has no children
        if(root->left == NULL && root->right == NULL) {
            free(root->data);
            free(root);
            return NULL;
        }
        //if the node has 1 child
        if(!root->left) {
            PokemonNode *temp = root->right;
            temp->parent = root->parent;
            free(root->data);
            free(root);
            return temp;
        }
        if(!root->right) {
            PokemonNode *temp = root->left;
            temp->parent = root->parent;
            free(root->data);
            free(root);
            return temp;
        }
        //if the node has 2 children
        PokemonNode *successor = findMinNode(root->right);
        *root->data = *successor->data;
        root->right = removeNodeBST(root->right, successor->data->id);
    }
    return root;
}

PokemonNode *removePokemonByID(PokemonNode *root, int id) {
    if (!searchPokemonBFS(root, id)) {
        printf("No Pokemon with ID %d found.\n", id);
        return root;
    }
    printf("Removing Pokemon %s (ID %d).\n", pokedex[id - 1].name, pokedex[id - 1].id);
    return removeNodeBST(root, id);
}

void freePokemon(OwnerNode *owner) {
    int choice = readIntSafe("Enter Pokemon ID to release: ");
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, choice);
}

void addPokemon(OwnerNode *owner) {
    if(owner->pokedexRoot == NULL) {
        printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
        int choice = readIntSafe("Your choice: ");

        owner->pokedexRoot = (PokemonNode*)malloc(sizeof(PokemonNode));
        if (!owner->pokedexRoot) {
            printf("Memory allocation failed.\n");
            return;
        }

        owner->pokedexRoot->data = (PokemonData*)malloc(sizeof(PokemonData));
        if (!owner->pokedexRoot->data) {
            printf("Memory allocation failed.\n");
            return;
        }
        *owner->pokedexRoot->data = pokedex[(choice-1)*3];
        owner->pokedexRoot->left = NULL;
        owner->pokedexRoot->right = NULL;
        owner->pokedexRoot->parent = NULL;
        return;
    }

    int choice = readIntSafe("Enter ID to add: ");
    if(searchPokemonBFS(owner->pokedexRoot, choice) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.", choice);
        return;
    }
    printf("TEST\n");
    PokemonNode *insert = (PokemonNode*)malloc(sizeof(PokemonNode));
    if (!insert) {
        printf("Memory allocation failed.\n");
        return;
    }
    printf("TEST\n");

    insert->data = (PokemonData*)malloc(sizeof(PokemonData));
    if (!insert->data) {
        printf("Memory allocation failed.\n");
        return;
    }
    printf("TEST\n");

    *insert->data = pokedex[(choice-1)];
    insert->left = NULL;
    insert->right = NULL;

    insertPokemonNode(owner->pokedexRoot, insert);
    printf("Pokemon %s (ID %d) added.\n", insert->data->name, insert->data->id);
}


/* ------------------------------------------------------------
   2) Creating & Freeing Nodes
   ------------------------------------------------------------ */

void freePokemonNode(PokemonNode *node) {
    free(node->data);
    free(node);
}
void freePokemonTree(PokemonNode *root) {
    if (root == NULL)
        return;
    postOrderTraversal(root->left);
    postOrderTraversal(root->right);
    freePokemonNode(root);
    free(root);
}
void freeOwnerNode(OwnerNode *owner) {
    freePokemonNode(owner->pokedexRoot);
    free(owner);
}

/* ------------------------------------------------------------
   9) Circular List Linking & Searching
   ------------------------------------------------------------ */

void removeOwnerFromCircularList(OwnerNode *target) {
    //if there is only 1 pokedex
    if(target->next == target){
        freeOwnerNode(target);
        ownerHead = NULL;
        return;
    }

    //if we are removing the head, move the head
    if(ownerHead == target)
        ownerHead = target->next;

    target->prev->next = target->next;
    target->next->prev = target->prev;

    freeOwnerNode(target);
}

/* ------------------------------------------------------------
   10) Owner Menus
   ------------------------------------------------------------ */

void enterExistingPokedexMenu(int n)
{
    if(ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)
    OwnerNode *temp = ownerHead;
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, temp->ownerName);
        temp = temp->next;
    }
    int choice = readIntSafe("Choose a Pokedex by number: ");
    //navigate the list to the current choice and save it so we can work on it
    for(int i = 0; i < choice-1; i++)
        temp = temp->next;
    OwnerNode *cur = temp;
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

            if(cur->pokedexRoot == NULL) {
                printf("No Pokemon to release.\n");
                break;
            }
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

void deletePokedex(int n) {
    printf("=== Delete a Pokedex ===\n");
    OwnerNode *temp = ownerHead;
    for (int i = 0; i < n; i++) {
        printf("%d. %s\n", i + 1, temp->ownerName);
        temp = temp->next;
    }
    int choice = readIntSafe("Choose a Pokedex to delete by number: ");

    //navigate to the node to delete
    for(int i = 0; i < choice-1; i++)
        temp = temp->next;

    printf("\nDeleting %s's entire Pokedex...\n", temp->ownerName);
    removeOwnerFromCircularList(temp);
}

void mergePokedexMenu(int n) {
    printf("=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char* firstName = getDynamicInput();
    printf("Enter name of second owner: ");
    char* secondName = getDynamicInput();
    OwnerNode *first = NULL, *second = NULL;

    OwnerNode *temp = ownerHead;
    //save the wanted owners
    for(int i = 0; i < n; i++) {
        if(strcmp(firstName, temp->ownerName) == 0) {
            first = temp;
        }
        if(strcmp(secondName, temp->ownerName) == 0) {
            second = temp;
        }
        temp = temp->next;
    }

    printf("Merging %s, %s\n", firstName, secondName);
    //LEAK
    Queue *q = createQueue();
    enqueue(q, second->pokedexRoot);
    //transfer pokemons from 1 owner to another
    while(!isEmpty(q)) {
        PokemonNode *cur = dequeue(q);
        PokemonNode *newNode = (PokemonNode *)malloc(sizeof(PokemonNode));
        newNode->data = (PokemonData *)malloc(sizeof(PokemonData));
        *newNode->data = *cur->data;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->parent = NULL;
        insertPokemonNode(first->pokedexRoot, newNode);
        printf("%s\n", first->pokedexRoot->data->name);
        if(cur->left != NULL) {
            enqueue(q, cur->left);
        }
        if(cur->right != NULL) {
            enqueue(q, cur->right);
        }
    }
    freeQueue(q);
    removeOwnerFromCircularList(second);

    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", secondName);
    free(firstName);
    free(secondName);
}

void sortOwners(int n) {
    OwnerNode *temp = ownerHead;
    for(int i =0; i < n; i++) {
        for(int j = i; j < n-1; j++) {
            if(strcmp(temp->ownerName, temp->next->ownerName) > 0)
                swapOwnerData(temp, temp->next);
        }
        temp = ownerHead;
    }
    printf("Owners sorted by name.\n");
}

void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    //swap names
    char* tempName = a->ownerName;
    a->ownerName = b->ownerName;
    b->ownerName = tempName;

    //swap data
    PokemonNode *tempPokedexRoot = a->pokedexRoot;
    a->pokedexRoot = b->pokedexRoot;
    b->pokedexRoot = tempPokedexRoot;
}

void openPokedexMenu(int n) {
    printf("Your name: ");
    if(ownerHead == NULL) {
        ownerHead = (OwnerNode*)malloc(sizeof(OwnerNode));
        if (!ownerHead) {
            printf("Memory allocation failed.\n");
            return;
        }
        ownerHead->ownerName = getDynamicInput();
        ownerHead->pokedexRoot = NULL;
        addPokemon(ownerHead);

        ownerHead->next = ownerHead;
        ownerHead->prev = ownerHead;
    }
    else {
        OwnerNode *newPokedex = malloc(sizeof(OwnerNode));
        if (!newPokedex) {
            printf("Memory allocation failed.\n");
            return;
        }
        newPokedex->ownerName = getDynamicInput();
        newPokedex->pokedexRoot = NULL;
        addPokemon(newPokedex);
        OwnerNode *last = ownerHead->prev;
        newPokedex->next = ownerHead;
        newPokedex->prev = last;
        last->next = newPokedex;
        ownerHead->prev = newPokedex;
    }
    printf("New Pokedex created for %s with starter %s.\n", ownerHead->prev->ownerName,
                                                        ownerHead->prev->pokedexRoot->data->name);
}


// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice, numOfPokedexes = 0;
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
            numOfPokedexes++;
            openPokedexMenu(numOfPokedexes);
            break;
        case 2:
            enterExistingPokedexMenu(numOfPokedexes);
            break;
        case 3:
            if(ownerHead == NULL) {
                printf("No existing Pokedexes.\n");
                break;
            }
            deletePokedex(numOfPokedexes);
            numOfPokedexes--;
            break;
        case 4:
            if(!ownerHead || ownerHead == ownerHead->next) {
                printf("Not enough owners to merge.\n");
                break;
            }
            mergePokedexMenu(numOfPokedexes);
            numOfPokedexes--;
            break;
        case 5:
            if(ownerHead == NULL || ownerHead == ownerHead->next) {
                printf("0 or 1 owners only => no need to sort.\n");
                break;
            }
            sortOwners(numOfPokedexes);
            break;
        case 6:
            //printOwnersCircular();
            break;
        case 7:
            //freeAllOwners();
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
    //freeAllOwners();
    return 0;
}
