// Created by Copilot
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000

// ------------------------------------
// Priority mapping
// ------------------------------------
int getPriority(const char *sender) {
    if (strcmp(sender, "Boss") == 0) return 5;
    if (strcmp(sender, "Subordinate") == 0) return 4;
    if (strcmp(sender, "Peer") == 0) return 3;
    if (strcmp(sender, "ImportantPerson") == 0) return 2;
    return 1; // OtherPerson
}

// ------------------------------------
// Email struct
// ------------------------------------
typedef struct {
    char sender[50];
    char subject[200];
    int month, day, year;
    int priority;
} Email;

// Compare two emails for heap ordering
int emailLess(Email *a, Email *b) {
    if (a->priority != b->priority)
        return a->priority < b->priority;

    // Newer date first
    if (a->year != b->year)
        return a->year < b->year;
    if (a->month != b->month)
        return a->month < b->month;
    return a->day < b->day;
}

// ------------------------------------
// MaxHeap implementation
// ------------------------------------
typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;

void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleUp(MaxHeap *h, int index) {
    int parent = (index - 1) / 2;
    while (index > 0 && emailLess(&h->data[parent], &h->data[index])) {
        swap(&h->data[parent], &h->data[index]);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void bubbleDown(MaxHeap *h, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < h->size && emailLess(&h->data[largest], &h->data[left]))
        largest = left;
    if (right < h->size && emailLess(&h->data[largest], &h->data[right]))
        largest = right;

    if (largest != index) {
        swap(&h->data[index], &h->data[largest]);
        bubbleDown(h, largest);
    }
}

void push(MaxHeap *h, Email e) {
    h->data[h->size] = e;
    bubbleUp(h, h->size);
    h->size++;
}

Email *peek(MaxHeap *h) {
    if (h->size == 0) return NULL;
    return &h->data[0];
}

void pop(MaxHeap *h) {
    if (h->size == 0) return;
    swap(&h->data[0], &h->data[h->size - 1]);
    h->size--;
    bubbleDown(h, 0);
}

// ------------------------------------
// Driver logic
// ------------------------------------
int main() {
    MaxHeap heap;
    heap.size = 0;

    char line[300];

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; // remove newline

        if (strncmp(line, "EMAIL", 5) == 0) {
            char *rest = strchr(line, ' ');
            rest++;

            char sender[50], subject[200], date[20];
            sscanf(rest, "%[^,],%[^,],%s", sender, subject, date);

            Email e;
            strcpy(e.sender, sender);
            strcpy(e.subject, subject);
            sscanf(date, "%d-%d-%d", &e.month, &e.day, &e.year);
            e.priority = getPriority(sender);

            push(&heap, e);
        }
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
        else if (strcmp(line, "NEXT") == 0) {
            Email *e = peek(&heap);
            if (!e) {
                printf("No emails to read.\n");
            } else {
                printf("Next email:\n");
                printf("Sender: %s\n", e->sender);
                printf("Subject: %s\n", e->subject);
                printf("Date: %02d-%02d-%04d\n", e->month, e->day, e->year);
            }
        }
        else if (strcmp(line, "READ") == 0) {
            pop(&heap);
        }
    }

    return 0;
}