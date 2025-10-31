#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define WORDS_PER_PAGE 300
#define MAX_WORD_LENGTH 50
#define LAST_BOOK_FILE "last_book.txt"

    typedef struct note
{
    char text[256];
    struct note *next;
} note;
typedef struct bookmark
{
    int page_number;
    char note_text[256];
    struct bookmark *next;
} bookmark;
typedef struct readingsession
{
    time_t start_time;
    time_t end_time;
    struct readingsession *next;
} readingsession;
typedef struct page
{
    char *content;
    note *notes;
    struct page *next;
} page;
typedef struct book
{
    char filename[100];
    page *pages;
    int total_pages;
    int current_page;
    bookmark *bookmarks;

    readingsession *history;
    struct book *next;
} book;
typedef struct bookmanager
{
    book *head;
    book *current_book;
} bookmanager;
typedef struct stack
{
    int *items;
    int top;
    int capacity;
} stack;
// STACK FUNCTIONS
stack *createstack(int capacity)
{
    stack *s = (stack *)malloc(sizeof(stack));
    s->items = (int *)malloc(capacity * sizeof(int));
    s->top = -1;
    s->capacity = capacity;
    return s;
}
int isempty(stack *s)
{
    return s->top == -1;
}
void push(stack *s, int item)
{
    if (s->top < s->capacity - 1)
        s->items[++s->top] = item;
}
int pop(stack *s)
{
    if (isempty(s))
        return -1;

    return s->items[s->top--];
}
void clearstack(stack *s)
{
    s->top = -1;
}
void freestack(stack *s)
{
    free(s->items);
    free(s);
}
// PAGE FUNCTIONS
page *createpagenode(char *content)
{
    page *new_page = (page *)malloc(sizeof(page));
    new_page->content = strdup(content);
    new_page->notes = NULL;
    new_page->next = NULL;
    return new_page;
}
void freepages(page *head)
{
    while (head)
    {
        page *temp = head;
        head = head->next;
        note *n = temp->notes;
        while (n)
        {
            note *tmpn = n;
            n = n->next;
            free(tmpn);
        }
        free(temp->content);
        free(temp);
    }
}

// BOOK FUNCTIONS
book *createbook(char *filename)
{
    book *b = (book *)malloc(sizeof(book));
    strcpy(b->filename, filename);
    b->pages = NULL;
    b->total_pages = 0;
    b->current_page = 1;
    b->bookmarks = NULL;
    b->history = NULL;
    b->next = NULL;
    return b;
}
page *loadbookfile(const char *filename, int *total_pages)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Error: Cannot open file %s\n", filename);
        return NULL;
    }
    char word[MAX_WORD_LENGTH];
    char buffer[WORDS_PER_PAGE * MAX_WORD_LENGTH];
    buffer[0] = '\0';
    int word_count = 0;
    page *head = NULL, *tail = NULL;
    while (fscanf(fp, "%s", word) == 1)
    {
        strcat(buffer, word);
        strcat(buffer, " ");
        word_count++;
        if (word_count >= WORDS_PER_PAGE)
        {
            page *new_page = createpagenode(buffer);
            if (!head)
                head = tail = new_page;
            else
            {
                tail->next = new_page;
                tail = new_page;
            }

            buffer[0] = '\0';
            word_count = 0;
        }
    }
    if (word_count > 0)
    {
        page *new_page = createpagenode(buffer);
        if (!head)
            head = tail = new_page;
        else
        {
            tail->next = new_page;
            tail = new_page;
        }
    }
    fclose(fp);
    *total_pages = 0;
    page *temp = head;
    while (temp)
    {
        (*total_pages)++;
        temp = temp->next;
    }
    return head;
}
// NOTES & BOOKMARKS
void addnotetopage(page *pg)
{
    if (!pg)
        return;
    char note_text[256];
    printf("Enter note for this page: ");
    getchar();
    fgets(note_text, 256, stdin);
    note_text[strcspn(note_text, "\n")] = 0;
    note *new_note = (note *)malloc(sizeof(note));
    strcpy(new_note->text, note_text);
    new_note->next = pg->notes;
    pg->notes = new_note;
    printf("Note added!\n");
}

void viewnotes(page *pg)
{
    if (!pg || !pg->notes)
    {
        printf("No notes for this page.\n");
        return;
    }
    printf("Notes for this page:\n");
    note *n = pg->notes;
    int count = 1;
    while (n)
    {
        printf("%d. %s\n", count++, n->text);
        n = n->next;
    }
}
void addbookmark(book *bk)
{
    int page_num;
    char note_text[256];
    printf("Enter page number to bookmark: ");
    scanf("%d", &page_num);
    if (page_num < 1 || page_num > bk->total_pages)
    {
        printf("Invalid page number.\n");
        return;
    }
    getchar();
    printf("Enter note for bookmark: ");
    fgets(note_text, 256, stdin);
    note_text[strcspn(note_text, "\n")] = 0;
    bookmark *new_bm = (bookmark *)malloc(sizeof(bookmark));
    new_bm->page_number = page_num;
    strcpy(new_bm->note_text, note_text);
    new_bm->next = bk->bookmarks;
    bk->bookmarks = new_bm;
    printf("Bookmark added!\n");
}
void listbookmarks(book *bk)
{

    if (!bk->bookmarks)
    {
        printf("No bookmarks.\n");
        return;
    }
    printf("Bookmarks:\n");
    bookmark *b = bk->bookmarks;
    while (b)
    {
        printf("Page %d: %s\n", b->page_number, b->note_text);
        b = b->next;
    }
}
// READING HISTORY
void startreadingsession(book *bk, time_t *start)
{
    *start = time(NULL);
}
void endreadingsession(book *bk, time_t start)
{
    time_t end = time(NULL);
    readingsession *new_session = (readingsession *)malloc(sizeof(readingsession));
    new_session->start_time = start;
    new_session->end_time = end;
    new_session->next = bk->history;
    bk->history = new_session;
    double seconds = difftime(end, start);
    printf("Session duration: %.2f seconds\n", seconds);
}
void viewreadinghistory(book *bk)
{
    if (!bk->history)
    {
        printf("No reading history.\n");
        return;
    }
    printf("Reading history:\n");
    readingsession *h = bk->history;
    int count = 1;
    while (h)

    {
        char start_str[26], end_str[26];
        strcpy(start_str, ctime(&h->start_time));
        strcpy(end_str, ctime(&h->end_time));
        start_str[strcspn(start_str, "\n")] = 0;
        end_str[strcspn(end_str, "\n")] = 0;
        printf("%d. Start: %s | End: %s | Duration: %.2f seconds\n",
               count++, start_str, end_str, difftime(h->end_time, h->start_time));
        h = h->next;
    }
}
// SEARCH FUNCTION
void searchword(book *bk)
{
    char word[MAX_WORD_LENGTH];
    printf("Enter word to search: ");
    scanf("%s", word);
    page *p = bk->pages;
    int page_num = 1;
    int found = 0;
    while (p)
    {
        if (strstr(p->content, word))
        {
            printf("Word found on page %d\n", page_num);
            found = 1;
        }
        p = p->next;
        page_num++;
    }
    if (!found)
        printf("Word not found in the book.\n");
}
// PAGE DISPLAY
page *getpage(book *bk, int page_num)
{
    page *p = bk->pages;
    int i = 1;

    while (p && i < page_num)
    {
        p = p->next;
        i++;
    }
    return p;
}
void showpage(book *bk, int page_num)
{
    page *p = getpage(bk, page_num);
    if (!p)
    {
        printf("Invalid page.\n");
        return;
    }
    printf("\n--- Page %d / %d ---\n%s\n", page_num, bk->total_pages, p->content);
}
// BOOK MANAGER
bookmanager *createbookmanager()
{
    bookmanager *bm = (bookmanager *)malloc(sizeof(bookmanager));
    bm->head = NULL;
    bm->current_book = NULL;
    return bm;
}
book *findbook(bookmanager *bm, char *filename)
{
    book *b = bm->head;
    while (b)
    {
        if (strcmp(b->filename, filename) == 0)
            return b;
        b = b->next;
    }
    return NULL;
}
book *addbook(bookmanager *bm, char *filename)
{
    book *existing = findbook(bm, filename);

    if (existing)
        return existing;
    book *b = createbook(filename);
    b->pages = loadbookfile(filename, &b->total_pages);
    if (!b->pages)
    {
        free(b);
        return NULL;
    }
    b->next = bm->head;
    bm->head = b;
    return b;
}
// LAST READ BOOK
void savelastbook(book *bk)
{
    FILE *fp = fopen(LAST_BOOK_FILE, "w");
    if (!fp)
        return;
    fprintf(fp, "%s %d\n", bk->filename, bk->current_page);
    fclose(fp);
}
book *loadlastbook(bookmanager *bm)
{
    FILE *fp = fopen(LAST_BOOK_FILE, "r");
    if (!fp)
        return NULL;
    char filename[100];
    int page_num;
    fscanf(fp, "%s %d", filename, &page_num);
    fclose(fp);
    book *b = addbook(bm, filename);
    if (b)
        b->current_page = page_num;
    return b;
}
// MAIN READER LOOP
void ebookreader(bookmanager *bm)
{

    time_t session_start;
    while (1)
    {
        book *bk = bm->current_book;
        if (!bk)
        {
            printf("No book loaded.\n");
            return;
        }
        showpage(bk, bk->current_page);
        startreadingsession(bk, &session_start);
        printf("\nOptions:\n");
        printf("1. Next Page\n2. Previous Page\n3. Add Bookmark\n4. List Bookmarks\n");
        printf("5. Add Note\n6. View Notes\n7. Search Word\n8. View Reading History\n");
        printf("9. Switch Book\n10. Exit\nEnter choice: ");
        int choice;
        scanf("%d", &choice);
        endreadingsession(bk, session_start);
        switch (choice)
        {
        case 1:
            if (bk->current_page < bk->total_pages)
                bk->current_page++;
            else
                printf("End of book.\n");
            break;
        case 2:
            if (bk->current_page > 1)
                bk->current_page--;
            else
                printf("Start of book.\n");
            break;
        case 3:
            addbookmark(bk);
            break;
        case 4:
            listbookmarks(bk);
            break;
        case 5:
            addnotetopage(getpage(bk, bk->current_page));
            break;
        case 6:
            viewnotes(getpage(bk, bk->current_page));
            break;
        case 7:
            searchword(bk);
            break;
        case 8:
            viewreadinghistory(bk);
            break;
        case 9:
        {
            char filename[100];
            printf("Enter book filename to switch: ");
            scanf("%s", filename);

            book *new_bk = addbook(bm, filename);
            if (new_bk)
            {

                bm->current_book = new_bk;
                printf("Switched to %s\n", filename);
            }
            else

            {

                printf("Failed to load book.\n");
            }
            break;
        }
        case 10:
            savelastbook(bk);
            printf("Exiting reader. Goodbye!\n");
            return;
        default:
            printf("Invalid choice.\n");
        }
    }
}
// MAIN
int main()
{
    bookmanager *bm = createbookmanager();
    book *last = loadlastbook(bm);
    if (last)
    {
        bm->current_book = last;
        printf("Last book read: %s at page %d\n", last->filename, last->current_page);
        char choice;
        printf("Do you want to continue reading? (Y/N): ");
        getchar();
        scanf("%c", &choice);
        if (choice != 'Y' && choice != 'y')
            bm->current_book = NULL;
    }
    if (!bm->current_book)
    {
        char filename[100];
        printf("Enter book filename to load: ");
        scanf("%s", filename);
        book *b = addbook(bm, filename);

        if (!b)
        {
            printf("Failed to load book. Exiting.\n");
            return 0;
        }
        bm->current_book = b;
    }
    ebookreader(bm);
    return 0;
}