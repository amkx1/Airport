// ============================================
// flight_logic.h
// ============================================
#ifndef FLIGHT_LOGIC_H
#define FLIGHT_LOGIC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FLIGHTS 20
#define MAX_LUGGAGE 10

struct Plane
{
    char flightID[10];
    int priority;
    char luggage[MAX_LUGGAGE][10];
    int luggageCount;
};

struct PriorityQueue
{
    struct Plane planes[MAX_FLIGHTS];
    int size;
};

// Function declarations
void enqueue(struct PriorityQueue *pq, char id[], int priority);
void dequeue(struct PriorityQueue *pq);
void display(struct PriorityQueue *pq);
void addLuggage(struct PriorityQueue *pq);
void addLuggageToFlight(struct PriorityQueue *pq);
void runConsoleMenu();

#endif // FLIGHT_LOGIC_H