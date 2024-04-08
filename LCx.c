#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node {
    struct node* next;
    struct node* prev; 
    size_t val;
};

struct list {
    struct node* head;
    struct node* tail;
    size_t size;
};

struct table {
    struct node** arr;
    struct list lst;
    size_t size;
};

const size_t COEF = 4;
const int COEF2 = 19;
struct node* const POISON = (struct node* const) 0x6;

void create_table(struct table* tbl, size_t size) {
    struct node* cur;

    tbl->size = COEF * size;
    tbl->lst.size = size;

    tbl->arr = (struct node **) calloc(tbl->size, sizeof(struct node*));

    cur = tbl->lst.head = (struct node *) calloc(1, sizeof(struct node));

    for (size_t i = 0; i < size - 1; ++i) {
        cur->next = (struct node *) calloc(1, sizeof(struct node));
        cur->next->prev = cur;
        cur = cur->next;
    }

    cur->next = tbl->lst.head;
    tbl->lst.head->prev = cur;
    tbl->lst.tail = cur;
}

void delete_table(struct table* tbl) {
    struct node* cur = tbl->lst.head;

    while (cur->next != tbl->lst.tail) {
        free(cur->prev);
        cur = cur->next;
    }

    free(cur->prev);
    free(cur);

    free(tbl->arr);
}

size_t get_hash(struct table* tbl, int val) {
    return val % tbl->size;
}

void print_list(const struct list* lst) {
    struct node* cur = lst->head;
    size_t i = 0;

    while (i < lst->size + 2 && cur->next != lst->head) {
        printf("%p %lu (next->%p)\n", cur, cur->val, cur->next);
        cur = cur->next;
        ++i;
    }

    printf("%p %lu (next->%p)\n", cur, cur->val, cur->next);
}

void print_table(const struct table* tbl) {
    printf("LIST:\n");
    print_list(&tbl->lst);

    printf("ARRAY:\n");
    for (size_t i = 0; i < tbl->size; ++i) {
        printf("%lu %p\n", i, tbl->arr[i]);
    }

    printf("\n");
}

void node_to_head(struct list* lst, struct node* nd) {
    nd->prev->next = nd->next;
    nd->next->prev = nd->prev;

    lst->tail->next = nd;
    lst->head->prev = nd;

    nd->next = lst->head;
    nd->prev = lst->tail;
    lst->head = nd;
}

void tail_to_head(struct list* lst, int val) {
    lst->tail->val = val;
    lst->tail = lst->tail->prev;
    lst->head = lst->head->prev;
}

void tail_to_psn(struct table* tbl) {
    size_t hasht = get_hash(tbl, tbl->lst.tail->val);

    while (tbl->arr[hasht] != NULL) {
        if (tbl->arr[hasht] != POISON && tbl->arr[hasht]->val == tbl->lst.tail->val) {
            tbl->arr[hasht] = POISON;
            break;
        }

        ++hasht;

        if (hasht >= tbl->size) {
            hasht = 0;
        }
    }
}

int cache(struct table *tbl, size_t val) {
    const size_t hashi = get_hash(tbl, val);
    size_t hash = hashi;
    // printf("val: %d, hash: %lu\n", val, hash);
    struct node* cur = NULL;
    int is_psn = 0;
    size_t fir_psn = -1;

    if (tbl->lst.head->val == val) {
        return 1;
    }

    while (tbl->arr[hash] != NULL) {
        if (is_psn == 0 && tbl->arr[hash] == POISON) {
            is_psn = 1;
            fir_psn = hash;
        }

        if (tbl->arr[hash] != POISON && tbl->arr[hash]->val == val) {
            cur = tbl->arr[hash];
            break;
        }

        ++hash;

        if (hash >= tbl->size) {
            hash = 0;
        }

        if (hash == hashi) {
            hash = fir_psn;
            tbl->arr[fir_psn] = NULL;
            is_psn = 0;
            break;
        }
    }

    if (tbl->arr[hash] == NULL) {
        tail_to_psn(tbl);

        tail_to_head(&tbl->lst, val);

        if (!is_psn) {
            fir_psn = hash;
        }

        tbl->arr[fir_psn] = tbl->lst.head;

        return 0;
    }

    // assert(cur->val == val);
    if (cur == tbl->lst.tail) {
        tail_to_head(&tbl->lst, val);

        return 1;
    }

    node_to_head(&tbl->lst, cur);

    return 1;
}

int main() {
    size_t m;
    size_t n;
    struct table tbl;
    size_t num_hit = 0;

    scanf("%lu%lu", &m, &n);

    create_table(&tbl, m);

    for (size_t i = 0; i < n; ++i) {
        size_t next;
        scanf("%lu", &next);

        num_hit += cache(&tbl, next);

        // print_table(&tbl);
    }

    printf("%lu\n", num_hit);

    delete_table(&tbl);

    return 0;
}
