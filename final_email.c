/*
Final email checker
A C program the sorts emails by priority and shows user the emails they access. Prints out how many emails they have to read
Inputs: A txt file containing emails and commands
Outputs: emails user has read and how many are left to read
Collaborater: None
Sources: Ai Copilot
Author: Baylie Stith
Created: 9/16/2026
*/

/*
Code:
gcc -Wall -o final_email final_email.c
./final_email < samp.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000   // Maximum number of emails allowed in heap

// ------------------------------------------------------------
// Priority mapping based on sender category (Code by Copilot comments from author)
// Input: Sender of email
// Output: priority of email
// ------------------------------------------------------------
int getPriority(const char *sender) {
    if (strcmp(sender, "Boss") == 0) return 5; // If sender is boss -> highest priority
    if (strcmp(sender, "Subordinate") == 0) return 4; // If sender subodinate -> next priority
    if (strcmp(sender, "Peer") == 0) return 3; // If sender a peer -> middle priority
    if (strcmp(sender, "ImportantPerson") == 0) return 2; // sender and important person -> 2nd to last priority
    return 1; // OtherPerson -> last priority
}

// ------------------------------------------------------------
// Email struct: stores sender, subject, date, and priority
// ------------------------------------------------------------
typedef struct {
    char sender[50];
    char subject[200];
    int month, day, year;
    int priority;
} Email;

// ------------------------------------------------------------
// Comparison function for heap ordering (Code by Copilot comments from author)
// Takes in the email and priority, checks if emails prioritys are the same
// Outputs new priority using date of email if necassary (Sender is same for two emails)
// ------------------------------------------------------------
int emailLess(Email *a, Email *b) {
    // Compare sender priority first
    if (a->priority != b->priority)
        return a->priority < b->priority;

    // If same priority, compare dates (newest first)
    if (a->year != b->year)
        return a->year < b->year;
    if (a->month != b->month)
        return a->month < b->month;
    return a->day < b->day;
}

// ------------------------------------------------------------
// MaxHeap structure using array-based implementation (Code by Copilot comments from author)
// ------------------------------------------------------------
typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;

// Swap two Email objects
// Take in email and swaps their priority
void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

// ------------------------------------------------------------
// Bubble-up: restores heap property after insertion (Code by Copilot comments from author)
// Takes in the MaxHeao and an index(email)
// Returns a heap properly sorted by priority
// ------------------------------------------------------------
void bubbleUp(MaxHeap *h, int index) {
    int parent = (index - 1) / 2; // Assigns parent

    // Move upward while child has higher priority than parent
    while (index > 0 && emailLess(&h->data[parent], &h->data[index])) {
        swap(&h->data[parent], &h->data[index]); // swaps emails based on priority
        index = parent; // Replaces index with parent
        parent = (index - 1) / 2; // Creates new parent
    }
}

// ------------------------------------------------------------
// Bubble-down: restores heap property after removal (Code by Copilot comments from author)
// takes MaxHeap and an index(email)
// sorts list by priority and returns sorted heap
// ------------------------------------------------------------
void bubbleDown(MaxHeap *h, int index) {
    // creates parent and left/right side tree
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // Check left child
    if (left < h->size && emailLess(&h->data[largest], &h->data[left]))
        largest = left;

    // Check right child
    if (right < h->size && emailLess(&h->data[largest], &h->data[right]))
        largest = right;

    // If largest changed, swap and continue bubbling down
    if (largest != index) {
        swap(&h->data[index], &h->data[largest]);
        bubbleDown(h, largest);
    }
}

// ------------------------------------------------------------
// Insert email into heap (Code by Copilot comments from author)
// takes in current heap and new email
// outputs heap with new email added sorterd by priority
// ------------------------------------------------------------
void push(MaxHeap *h, Email e) {
    h->data[h->size] = e;     // Place at end
    bubbleUp(h, h->size);     // Restore heap property
    h->size++;
}

// ------------------------------------------------------------
// Peek at highest priority email (does NOT remove) (Code by Copilot comments from author)
// Takes the heap and returns email
// ------------------------------------------------------------
Email *peek(MaxHeap *h) {
    if (h->size == 0) return NULL;
    return &h->data[0];
}

// ------------------------------------------------------------
// Remove highest priority email (Code by Copilot comments from author)
// takes in heap and returns heap with a removed element 
// ------------------------------------------------------------
void pop(MaxHeap *h) {
    if (h->size == 0) return;

    // Move last element to root and shrink heap
    swap(&h->data[0], &h->data[h->size - 1]);
    h->size--;

    // Restore heap property
    bubbleDown(h, 0);
}

// ------------------------------------------------------------
// Main driver: reads commands and processes them (Code by Copilot comments from author)
// ------------------------------------------------------------
int main() {
    MaxHeap heap;
    heap.size = 0; // Initializes sixe

    char line[300];

    // Read input line-by-line until EOF
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; // Strip newline

// ------------------------------------------------------------
// Handle the EMAIL command (Code by Copilot comments from author)
// Takes in emails and adds to the heap
// ------------------------------------------------------------
if (strncmp(line, "EMAIL", 5) == 0) {

    // Find the first space after the word "EMAIL". The input format guarantees exactly one space: EMAIL <data>
    // strchr(line, ' ') returns a pointer to the space character.
    char *rest = strchr(line, ' ');

    // Move past the space so 'rest' points directly to the sender category.
    rest++;

    // Temporary buffers to hold parsed fields.
    char sender[50], subject[200], date[20];

    // sscanf with "%[^,]" reads characters until a comma.
    sscanf(rest, "%[^,],%[^,],%s", sender, subject, date);

    // Create an Email struct to store parsed data.
    Email e;

    // Copy parsed strings into the struct fields.
    strcpy(e.sender, sender);
    strcpy(e.subject, subject);

    // Parse the date string into numeric month/day/year.
    sscanf(date, "%d-%d-%d", &e.month, &e.day, &e.year);

    // Convert sender category into numeric priority.
    e.priority = getPriority(sender);

    // Insert the fully constructed email into the MaxHeap. push() will bubble it up to the correct position.
    push(&heap, e);
}

        // --------------------------------------------------------
        // COUNT command: print number of unread emails (Code by Copilot comments from author)
        // --------------------------------------------------------
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }

        // --------------------------------------------------------
        // NEXT command: show highest priority email without removing (Code by Copilot comments from author)
        // --------------------------------------------------------
        else if (strcmp(line, "NEXT") == 0) {
            Email *e = peek(&heap); // looks at top of heap
            if (!e) {
                printf("No emails to read.\n"); // if heap empty returns that no emails need to be read
            } else {
                // if items in heap -> prints next email, sender, subject, and date
                printf("Next email:\n");
                printf("Sender: %s\n", e->sender);
                printf("Subject: %s\n", e->subject);
                printf("Date: %02d-%02d-%04d\n", e->month, e->day, e->year);
            }
        }

        // --------------------------------------------------------
        // READ command: remove highest priority email (Code by Copilot comments from author)
        // --------------------------------------------------------
        else if (strcmp(line, "READ") == 0) { // If txt file reads an email its popped off list
            pop(&heap);
        }
    }

    return 0;
}