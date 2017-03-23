#include <ngx_queue.h>
typedef struct  {
    u_char* str;
    ngx_queue_t qEle;
    int num;
} TestNode;

ngx_int_t compTestNode(const ngx_queue_t* a, const ngx_queue_t* b)
{
    TestNode* aNode = ngx_queue_data(a, TestNode, qEle);
    TestNode* bNode = ngx_queue_data(b, TestNode, qEle);

    return aNode->num > bNode->num;
}

int main()
{
    ngx_queue_t queueContainer;
    ngx_queue_init(&queueContainer);

    int i = 0;
    TestNode node[5];

    for (; i< 5; i++) {
        node[i].num = i;
    }

    ngx_queue_insert_tail(&queueContainer, &node[0].qEle);
    ngx_queue_insert_head(&queueContainer, &node[1].qEle);
    ngx_queue_insert_tail(&queueContainer, &node[2].qEle);
    ngx_queue_insert_after(&queueContainer, &node[3].qEle);
    ngx_queue_insert_tail(&queueContainer, &node[4].qEle);

    ngx_queue_t* q;
    for (q = ngx_queue_head(&queueContainer);
            q != ngx_queue_sentinel(&queueContainer);
            q = ngx_queue_next(q)) {
        TestNode* eleNode = ngx_queue_data(q, TestNode, qEle);
        printf("elem value is :%d\n", eleNode->num);
    }
    printf("after sort ==============\n");

    ngx_queue_sort(&queueContainer, compTestNode);

    for (q = ngx_queue_head(&queueContainer);
            q != ngx_queue_sentinel(&queueContainer);
            q = ngx_queue_next(q)) {
        TestNode* eleNode = ngx_queue_data(q, TestNode, qEle);
        printf("elem value is :%d\n", eleNode->num);
    }

    return 0;
}
