#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dudect/constant.h"
#include "harness.h"
#include "queue.h"

static list_ele_t **find_mid_ele(list_ele_t **head)
{
    list_ele_t **faster = head;
    list_ele_t **slower = head;
    while (*faster && (*faster)->next) {
        slower = &(*slower)->next;
        faster = &(*faster)->next->next;
    }
    return slower;
}

static list_ele_t *new_ele(char *s)
{
    list_ele_t *newE = malloc(sizeof(list_ele_t));
    if (!newE)
        goto fail;
    size_t len = strlen(s) + 1;
    newE->value = malloc(len);
    if (!newE->value)
        goto freeEle;
    strncpy(newE->value, s, len);
    newE->next = NULL;
    return newE;

freeEle:
    free(newE);
    newE = NULL;
fail:
    return NULL;
}
/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->size = 0;
    q->head = NULL;
    q->tail = &q->head;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q)
        return;
    while (q->head) {
        list_ele_t *buf = q->head;
        q->head = buf->next;
        free(buf->value);
        buf->value = NULL;
        free(buf);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    if (!q)
        return false;

    newh = new_ele(s);
    if (!newh)
        return false;

    newh->next = q->head;
    q->head = newh;
    if (*q->tail == q->head)
        q->tail = &q->head->next;
    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;
    list_ele_t *newE = new_ele(s);
    if (!newE)
        return false;

    *q->tail = newE;
    q->tail = &newE->next;

    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    list_ele_t *buf = q->head;
    if (&q->head->next == q->tail)
        q->tail = &q->head;
    q->head = q->head->next;
    q->size -= 1;

    if (sp) {
        strncpy(sp, buf->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    free(buf->value);
    buf->value = NULL;
    free(buf);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t *ptr = q->head;
    list_ele_t *prev = NULL;
    q->tail = &ptr->next;
    while (ptr) {
        list_ele_t *next = ptr->next;
        ptr->next = prev;
        prev = ptr;
        ptr = next;
    }
    q->head = prev;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
static list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    list_ele_t **ptr = find_mid_ele(&head);
    list_ele_t *mid = *ptr;
    *ptr = NULL;

    head = merge_sort(head);
    mid = merge_sort(mid);

    list_ele_t *mergered = NULL;
    ptr = &mergered;
    while (head && mid) {
        list_ele_t *smeller;
        if (strcmp(head->value, mid->value) > 0) {
            smeller = mid;
            mid = mid->next;
        } else {
            smeller = head;
            head = head->next;
        }
        *ptr = smeller;
        ptr = &(*ptr)->next;
    }

    while (head) {
        *ptr = head;
        head = head->next;
        ptr = &(*ptr)->next;
    }
    while (mid) {
        *ptr = mid;
        mid = mid->next;
        ptr = &(*ptr)->next;
    }
    *ptr = NULL;
    return mergered;
}

void q_sort(queue_t *q)
{
    if (!q || !q->head)
        return;

    q->head = merge_sort(q->head);
    while (*q->tail) {
        q->tail = &(*q->tail)->next;
    }
}

void q_half(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t **ptr = find_mid_ele(&q->head);
    printf("%s\n", (*ptr)->value);
}
