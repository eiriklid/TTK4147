#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
    list_t result = (list_t) malloc(sizeof(struct list));
    result->head = NULL;
    result->tail = NULL;
    result->length = 0;

    return result;
}

void list_delete(list_t list)
{
    struct node* iter = list->head;

    while (iter != NULL) {
        struct node* temp = iter;
        iter = iter->next;
        free(temp);
    }

    free(list);
}

void list_insert(list_t list, int index, int data)
{
    struct node* insert = (struct node*) malloc(sizeof(struct node));

    if (list->length == 0) {
        list->head = insert;
        list->tail = insert;
        insert->prev = NULL;
        insert->next = NULL;
    } else if (list->length <= index) {
        insert->next = NULL;
        list->tail->next = insert;
        insert->prev = list->tail;
        list->tail = insert;
    } else if (index == 0) {
        insert->prev = NULL;
        insert->next = list->head;
        list->head = insert;
    } else {
        struct node* iter = list->head;
        for (int i = 0; i < index; i++) {
            iter = iter->next;
        }

        iter->prev->next = insert;
        insert->prev = iter->prev;
        iter->prev = insert;
        insert->next = iter;
    }
    insert->data = data;
    list->length++;
}

void list_append(list_t list, int data)
{
    list_insert(list, list->length, data);
}

void list_print(list_t list)
{
    for (struct node* iter = list->head; iter != NULL; iter = iter->next) {
        printf("%i ", iter->data);
    }
    printf("\n");
}

long list_sum(list_t list)
{
    long sum = 0;
    for (struct node* iter = list->head; iter != NULL; iter = iter->next) {
        sum += (long) iter->data;
    }

    return sum;
}

int list_get(list_t list, int index)
{
    struct node* iter = list->head;
    for (int i = 0; i < index; i++) {
        iter = iter->next;
    }
    return iter->data;
}

int list_extract(list_t list, int index)
{
    int data;

    if (list->length == 0) {
        return -1;
    } else if (list->length <= index) {
        data = list->tail->data;
        struct node* temp = list->tail;
        list->tail = list->tail->prev;
        free(temp);
    } else if (index == 0) {
        data = list->head->data;
        struct node* temp = list->head;
        list->head = list->head->next;
        free(temp);
    } else {
        struct node* iter = list->head;
        for (int i = 0; i < index; i++) {
            iter = iter->next;
        }

        data = iter->data;
        iter->prev->next = iter->next;
        iter->next->prev = iter->prev;
        free(iter);
    }
    list->length--;
    return data;
}
