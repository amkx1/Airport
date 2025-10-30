// ============================================
// flight_logic.c
// ============================================
#include "flight_logic.h"

// ---------------- Priority Queue Functions ----------------
void enqueue(struct PriorityQueue *pq, char id[], int priority)
{
    if (pq->size == MAX_FLIGHTS)
    {
        printf("Runway Queue Full!\n");
        return;
    }

    int i = pq->size - 1;
    while (i >= 0 && pq->planes[i].priority > priority)
    {
        pq->planes[i + 1] = pq->planes[i];
        i--;
    }
    i++;

    strcpy(pq->planes[i].flightID, id);
    pq->planes[i].priority = priority;
    pq->planes[i].luggageCount = 0; // no luggage initially
    pq->size++;
}

void dequeue(struct PriorityQueue *pq)
{
    if (pq->size == 0)
    {
        printf("No flights waiting!\n");
        return;
    }

    printf("\nFlight %s is cleared for takeoff/landing (Priority %d)\n",
           pq->planes[0].flightID, pq->planes[0].priority);

    // Process luggage for the flight
    if (pq->planes[0].luggageCount > 0)
    {
        printf("Loading luggage: ");
        for (int i = 0; i < pq->planes[0].luggageCount; i++)
            printf("%s ", pq->planes[0].luggage[i]);
        printf("\n");
    }
    else
    {
        printf("No luggage for this flight.\n");
    }

    // Shift remaining planes
    for (int i = 0; i < pq->size - 1; i++)
        pq->planes[i] = pq->planes[i + 1];

    pq->size--;
}

void display(struct PriorityQueue *pq)
{
    if (pq->size == 0)
    {
        printf("No flights in queue.\n");
        return;
    }

    printf("\n--- Runway Queue ---\n");
    for (int i = 0; i < pq->size; i++)
    {
        printf("%s (Priority %d, Luggage %d)\n", pq->planes[i].flightID,
               pq->planes[i].priority, pq->planes[i].luggageCount);
    }
    printf("--------------------\n");
}

// ---------------- Luggage Functions ----------------
void addLuggage(struct PriorityQueue *pq)
{
    if (pq->size == 0)
    {
        printf("No flights in queue to add luggage.\n");
        return;
    }

    char flightID[10], luggageID[10];
    printf("Enter Flight ID to add luggage: ");
    scanf("%s", flightID);

    int found = 0;
    for (int i = 0; i < pq->size; i++)
    {
        if (strcmp(pq->planes[i].flightID, flightID) == 0)
        {
            if (pq->planes[i].luggageCount >= MAX_LUGGAGE)
            {
                printf("Luggage limit reached for this flight.\n");
                return;
            }
            printf("Enter Luggage ID: ");
            scanf("%s", luggageID);
            strcpy(pq->planes[i].luggage[pq->planes[i].luggageCount], luggageID);
            pq->planes[i].luggageCount++;
            printf("Luggage added successfully.\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Flight not found in queue.\n");
}

// Alias function for compatibility with main.c
void addLuggageToFlight(struct PriorityQueue *pq)
{
    addLuggage(pq);
}

// ---------------- Console Menu Function ----------------
void runConsoleMenu()
{
    struct PriorityQueue pq;
    pq.size = 0;

    int choice, priority;
    char flightID[10];

    while (1)
    {
        printf("\n1. Add Flight\n2. Add Luggage\n3. Clear Runway (Next Flight)\n4. Display Queue\n5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter Flight ID: ");
            scanf("%s", flightID);
            printf("Enter Priority (1=Emergency, 2=VIP, 3=Normal): ");
            scanf("%d", &priority);
            enqueue(&pq, flightID, priority);
            break;
        case 2:
            addLuggage(&pq);
            break;
        case 3:
            dequeue(&pq);
            break;
        case 4:
            display(&pq);
            break;
        case 5:
            return;
        default:
            printf("Invalid choice!\n");
        }
    }
}