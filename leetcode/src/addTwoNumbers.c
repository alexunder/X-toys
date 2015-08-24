/*
* Copyright (C) 2015 2nd Foundation
* All rights reserved
*
*/
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
typedef struct ListNode _ListNode;

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    _ListNode * p1 = l1;
    _ListNode * p2 = l2;
    _ListNode * head = NULL;
    _ListNode * tail = NULL;

    int sum = 0;
    int nextbit = 0;
    while (p1 && p2)
    {
        sum = p1->val + p2->val + nextbit;  
        nextbit = sum / 10;

        _ListNode * ptemp = (_ListNode*)malloc(sizeof(_ListNode));
        ptemp->val = sum % 10;
        ptemp->next = NULL;

        if (head == NULL)
        head = ptemp;

        if (tail == NULL)
        {
            tail = ptemp;
        }
        else
        {
            tail->next = ptemp;
            tail = ptemp;
        }

        p1 = p1->next;
        p2 = p2->next;
    }

    _ListNode * pLeft = NULL;
    if (p1)
        pLeft = p1;

    if (p2)
       pLeft = p2;

    while (pLeft)
    {
        sum = pLeft->val + nextbit;
        nextbit = sum / 10;
        _ListNode * ptemp = (_ListNode*)malloc(sizeof(_ListNode));
        ptemp->val = sum % 10;
        ptemp->next = NULL;

        if (tail == NULL)
        {
            tail = ptemp;
        }
        else
        {
            tail->next = ptemp;
            tail = ptemp;
        }

        pLeft = pLeft->next;
    }

    if (nextbit == 1)
    {
        _ListNode * ptemp = (_ListNode*)malloc(sizeof(_ListNode));
        ptemp->val = 1;
        ptemp->next = NULL;
        
        if (tail == NULL)
        {
            tail = ptemp;
        }
        else
        {
            tail->next = ptemp;
            tail = ptemp;
        }
    }

    return head;
}
