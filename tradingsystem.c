#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_ORDERS 1000

typedef struct {
    char symbol[10];
    int quantity;
    double price;
} Order;

typedef struct {
    Order orders[MAX_ORDERS];
    int order_count;
    pthread_mutex_t lock;
} OrderBook;

OrderBook order_book;

void initialize_order_book(OrderBook *ob) {
    ob->order_count = 0;
    pthread_mutex_init(&ob->lock, NULL);
}

void add_order(OrderBook *ob, const char *symbol, int quantity, double price) {
    pthread_mutex_lock(&ob->lock);
    if (ob->order_count < MAX_ORDERS) {
        strcpy(ob->orders[ob->order_count].symbol, symbol);
        ob->orders[ob->order_count].quantity = quantity;
        ob->orders[ob->order_count].price = price;
        ob->order_count++;
    } else {
        printf("Order book is full. Cannot add more orders.\n");
    }
    pthread_mutex_unlock(&ob->lock);
}

void execute_order(const Order *order) {
    printf("Executed Order: Symbol: %s, Quantity: %d, Price: %.2f\n", 
           order->symbol, order->quantity, order->price);
}

void process_orders(OrderBook *ob) {
    for (int i = 0; i < ob->order_count; i++) {
        execute_order(&ob->orders[i]);
    }
    ob->order_count = 0;
}

void *order_processing_thread(void *arg) {
    OrderBook *ob = (OrderBook *)arg;
    while (1) {
        process_orders(ob);
        usleep(1000);
    }
    return NULL;
}

int main() {
    initialize_order_book(&order_book);
    
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, order_processing_thread, &order_book);

    add_order(&order_book, "AAPL", 10, 150.00);
    add_order(&order_book, "GOOGL", 5, 2800.00);
    add_order(&order_book, "TSLA", 2, 700.00);

    for (int i = 0; i < 10; i++) {
        add_order(&order_book, "MSFT", 1, 300.00 + i);
        usleep(500000);
    }

    pthread_cancel(thread_id);
    pthread_mutex_destroy(&order_book.lock);
    return 0;
}
