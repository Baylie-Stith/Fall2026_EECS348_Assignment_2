// Made by Claude

/*
 * CEO Email Prioritizer
 * ---------------------
 * Reads a command file and manages the CEO's inbox using a priority queue
 * built on an array-based MaxHeap written from scratch.
 *
 * Build: gcc -Wall -o email email.c
 * Usage: ./email samp.txt
 *        ./email < samp.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE    512
#define MAX_SENDER   32
#define MAX_SUBJECT 256
#define MAX_DATE     16

/* ------------------------------------------------------------------ */
/* Email                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    char sender[MAX_SENDER];
    char subject[MAX_SUBJECT];
    char date[MAX_DATE];     /* kept as the original MM-DD-YYYY string */
    int  priority;           /* sender category rank, higher = sooner  */
    long dateKey;            /* YYYYMMDD, so bigger = newer            */
} Email;

/* Sender category priority. Higher number = read sooner. */
static int senderPriority(const char *sender)
{
    if (strcmp(sender, "Boss") == 0)            return 5;
    if (strcmp(sender, "Subordinate") == 0)     return 4;
    if (strcmp(sender, "Peer") == 0)            return 3;
    if (strcmp(sender, "ImportantPerson") == 0) return 2;
    if (strcmp(sender, "OtherPerson") == 0)     return 1;
    return 0;
}

/* Turn MM-DD-YYYY into YYYYMMDD so plain integer compare works. */
static long makeDateKey(const char *date)
{
    int month = 0, day = 0, year = 0;
    if (sscanf(date, "%d-%d-%d", &month, &day, &year) != 3)
        return 0;
    return (long)year * 10000L + (long)month * 100L + (long)day;
}

/* Returns 1 if a should be read before b. Category first, then newest date. */
static int higherPriority(const Email *a, const Email *b)
{
    if (a->priority != b->priority)
        return a->priority > b->priority;
    return a->dateKey > b->dateKey;
}

/* ------------------------------------------------------------------ */
/* MaxHeap (array based, root at index 0)                              */
/* ------------------------------------------------------------------ */

typedef struct {
    Email *items;
    int    count;
    int    capacity;
} MaxHeap;

static void heapInit(MaxHeap *h)
{
    h->capacity = 16;
    h->count    = 0;
    h->items    = malloc(sizeof(Email) * h->capacity);
    if (h->items == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
}

static void heapFree(MaxHeap *h)
{
    free(h->items);
    h->items    = NULL;
    h->count    = 0;
    h->capacity = 0;
}

/* index helpers */
static int parentOf(int i) { return (i - 1) / 2; }
static int leftOf(int i)   { return 2 * i + 1;   }
static int rightOf(int i)  { return 2 * i + 2;   }

static void swapItems(MaxHeap *h, int i, int j)
{
    Email tmp  = h->items[i];
    h->items[i] = h->items[j];
    h->items[j] = tmp;
}

static void siftUp(MaxHeap *h, int i)
{
    while (i > 0 && higherPriority(&h->items[i], &h->items[parentOf(i)])) {
        swapItems(h, i, parentOf(i));
        i = parentOf(i);
    }
}

static void siftDown(MaxHeap *h, int i)
{
    for (;;) {
        int largest = i;
        int l = leftOf(i);
        int r = rightOf(i);

        if (l < h->count && higherPriority(&h->items[l], &h->items[largest]))
            largest = l;
        if (r < h->count && higherPriority(&h->items[r], &h->items[largest]))
            largest = r;
        if (largest == i)
            break;

        swapItems(h, i, largest);
        i = largest;
    }
}

/* Add to the end, then bubble it up into place. */
static void heapInsert(MaxHeap *h, Email e)
{
    if (h->count == h->capacity) {
        Email *bigger = realloc(h->items, sizeof(Email) * h->capacity * 2);
        if (bigger == NULL) {
            fprintf(stderr, "Out of memory.\n");
            exit(1);
        }
        h->items    = bigger;
        h->capacity *= 2;
    }
    h->items[h->count] = e;
    siftUp(h, h->count);
    h->count++;
}

/* Highest-priority email without removing it. NULL if the heap is empty. */
static Email *heapPeek(MaxHeap *h)
{
    if (h->count == 0)
        return NULL;
    return &h->items[0];
}

/* Remove the highest-priority email. Does nothing if the heap is empty. */
static void heapExtractMax(MaxHeap *h)
{
    if (h->count == 0)
        return;
    h->count--;
    if (h->count > 0) {
        h->items[0] = h->items[h->count];   /* last element moves to root */
        siftDown(h, 0);
    }
}

/* ------------------------------------------------------------------ */
/* Commands                                                            */
/* ------------------------------------------------------------------ */

/* Copy src into dst, truncating safely and always NUL terminating. */
static void copyField(char *dst, size_t size, const char *src)
{
    size_t n = strlen(src);
    if (n >= size)
        n = size - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

/* Trim leading and trailing spaces in place. */
static char *trim(char *s)
{
    char *end;
    while (*s == ' ' || *s == '\t')
        s++;
    end = s + strlen(s);
    while (end > s && (end[-1] == ' ' || end[-1] == '\t'))
        end--;
    *end = '\0';
    return s;
}

/* rest looks like: Boss,Never Mind,01-03-2025 */
static void addEmail(MaxHeap *h, char *rest)
{
    char *sender, *subject, *date;
    char *c1, *c2;
    Email e;

    c1 = strchr(rest, ',');
    if (c1 == NULL)
        return;                     /* malformed line, skip it */
    *c1 = '\0';
    c2 = strchr(c1 + 1, ',');
    if (c2 == NULL)
        return;
    *c2 = '\0';

    sender  = trim(rest);
    subject = trim(c1 + 1);
    date    = trim(c2 + 1);

    copyField(e.sender,  sizeof(e.sender),  sender);
    copyField(e.subject, sizeof(e.subject), subject);
    copyField(e.date,    sizeof(e.date),    date);
    e.priority = senderPriority(e.sender);
    e.dateKey  = makeDateKey(e.date);

    heapInsert(h, e);
}

static void nextEmail(MaxHeap *h)
{
    Email *e = heapPeek(h);
    if (e == NULL) {
        printf("No emails to read.\n");
        return;
    }
    printf("Next email:\n");
    printf("Sender: %s\n", e->sender);
    printf("Subject: %s\n", e->subject);
    printf("Date: %s\n", e->date);
}

static void countEmails(MaxHeap *h)
{
    printf("There are %d emails to read.\n", h->count);
}

/* ------------------------------------------------------------------ */
/* Driver                                                              */
/* ------------------------------------------------------------------ */

static void run(FILE *in)
{
    char line[MAX_LINE];
    MaxHeap inbox;

    heapInit(&inbox);

    while (fgets(line, sizeof(line), in) != NULL) {
        size_t len = strlen(line);

        /* strip newline / carriage return */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = '\0';
        if (len == 0)
            continue;

        if (strncmp(line, "EMAIL ", 6) == 0)
            addEmail(&inbox, line + 6);
        else if (strcmp(line, "NEXT") == 0)
            nextEmail(&inbox);
        else if (strcmp(line, "READ") == 0)
            heapExtractMax(&inbox);     /* silently does nothing if empty */
        else if (strcmp(line, "COUNT") == 0)
            countEmails(&inbox);
    }

    heapFree(&inbox);
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        FILE *in = fopen(argv[1], "r");
        if (in == NULL) {
            fprintf(stderr, "Could not open %s\n", argv[1]);
            return 1;
        }
        run(in);
        fclose(in);
    } else {
        run(stdin);
    }
    return 0;
}