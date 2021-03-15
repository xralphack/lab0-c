#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL) {
        return;
    }

    list_ele_t *nextt = q->head;

    while (nextt) {
        free(nextt->value);
        list_ele_t *tmp = nextt->next;
        free(nextt);
        nextt = tmp;
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
    char *copied_string;

    if (q == NULL) {
        return false;
    }

    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return false;
    }

    copied_string = malloc(strlen(s) + 1);
    if (copied_string == NULL) {
        free(newh);
        return false;
    }

    strlcpy(copied_string, s, (strlen(s) + 1) * sizeof(char));

    newh->next = q->head;
    newh->value = copied_string;
    q->head = newh;
    if (q->tail == NULL) {
        q->tail = newh;
    }
    q->size++;

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
    list_ele_t *newt;
    char *copied_string;

    if (q == NULL) {
        return false;
    }

    newt = malloc(sizeof(list_ele_t));
    if (newt == NULL) {
        return false;
    }

    copied_string = malloc(strlen(s) + 1);
    if (copied_string == NULL) {
        free(newt);
        return false;
    }
    strlcpy(copied_string, s, (strlen(s) + 1) * sizeof(char));

    newt->next = NULL;
    newt->value = copied_string;
    if (q->tail) {
        q->tail->next = newt;
    }
    q->tail = newt;
    if (q->head == NULL) {
        q->head = newt;
    }
    q->size++;

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
    list_ele_t *nextt = NULL;

    if (q == NULL || q->head == NULL) {
        return false;
    }

    char *string = q->head->value;

    if (sp != NULL) {
        int i;
        for (i = 0; i < bufsize - 1; i++) {
            *(sp + i) = *string;
            string++;
        }
        *(sp + i) = '\0';
    }

    free(q->head->value);
    nextt = q->head->next;
    free(q->head);
    q->head = nextt;
    if (q->head == NULL) {
        q->tail = NULL;
    }

    q->size--;

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL || q->head == NULL) {
        return 0;
    }
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
    if (q == NULL || q->head == NULL) {
        return;
    }

    int i;
    list_ele_t *prevt = NULL;
    list_ele_t *thist = q->head;

    for (i = 0; i < q->size; i++) {
        if (i == 0) {
            q->tail = thist;
        } else if (i == q->size - 1) {
            q->head = thist;
        }

        list_ele_t *tmp = thist->next;
        thist->next = prevt;
        prevt = thist;
        thist = tmp;
    }
}

list_ele_t *merge(list_ele_t *head1, list_ele_t *head2)
{
    list_ele_t *head = strcmp(head1->value, head2->value) <= 0 ? head1 : head2;
    list_ele_t *cursor = NULL;

    while (head1 && head2) {
        if (strcmp(head1->value, head2->value) <= 0) {  // 1 <= 2
            if (!cursor) {
                cursor = head1;
            } else {
                cursor->next = head1;
                cursor = cursor->next;
            }
            head1 = head1->next;
        } else {
            if (!cursor) {
                cursor = head2;
            } else {
                cursor->next = head2;
                cursor = cursor->next;
            }
            head2 = head2->next;
        }
    }

    if (head1) {
        cursor->next = head1;
    } else if (head2) {
        cursor->next = head2;
    }

    return head;
}

list_ele_t *merge_sort(list_ele_t *node)
{
    if (!node || !node->next) {
        return node;
    }

    list_ele_t *slow = node;
    list_ele_t *fast = node->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    fast = slow->next;
    slow->next = NULL;

    list_ele_t *node1 = merge_sort(node);
    list_ele_t *node2 = merge_sort(fast);

    return merge(node1, node2);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == NULL) {
        return;
    }

    q->head = merge_sort(q->head);

    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
