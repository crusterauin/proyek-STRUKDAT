#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_FILM 100
#define MAX_STRING 100

void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

// Struktur untuk node film
typedef struct Film
{
    char judul[MAX_STRING];
    char genre[MAX_STRING];
    char aktor[MAX_STRING];
    int tahun;
    int visited;
} Film;

// Struktur untuk node adjacency list
typedef struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode *next;
} AdjListNode;

// Struktur untuk adjacency list
typedef struct AdjList
{
    AdjListNode *head;
} AdjList;

// Struktur untuk graf
typedef struct Graph
{
    int num_films;
    Film films[MAX_FILM];
    AdjList *array;
} Graph;

// Struktur untuk queue (BFS)
typedef struct Queue
{
    int items[MAX_FILM];
    int front;
    int rear;
} Queue;

// Struktur untuk stack (DFS)
typedef struct Stack
{
    int items[MAX_FILM];
    int top;
} Stack;

// Fungsi untuk membuat node adjacency list baru
AdjListNode *newAdjListNode(int dest, int weight)
{
    AdjListNode *newNode = (AdjListNode *)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Fungsi untuk membuat graf
Graph *createGraph()
{
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->num_films = 0;
    graph->array = (AdjList *)malloc(MAX_FILM * sizeof(AdjList));

    for (int i = 0; i < MAX_FILM; i++)
    {
        graph->array[i].head = NULL;
    }

    return graph;
}

// Fungsi untuk membuat queue
Queue *createQueue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

// Fungsi untuk membuat stack
Stack *createStack()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->top = -1;
    return stack;
}

// Fungsi untuk mengecek apakah queue kosong
int isEmptyQueue(Queue *queue)
{
    return queue->rear == -1;
}

// Fungsi untuk mengecek apakah stack kosong
int isEmptyStack(Stack *stack)
{
    return stack->top == -1;
}

// Fungsi untuk menambahkan elemen ke queue
void enqueue(Queue *queue, int value)
{
    if (queue->rear == MAX_FILM - 1)
    {
        printf("Queue penuh!\n");
    }
    else
    {
        if (queue->front == -1)
        {
            queue->front = 0;
        }
        queue->rear++;
        queue->items[queue->rear] = value;
    }
}

// Fungsi untuk menghapus elemen dari queue
int dequeue(Queue *queue)
{
    int item;
    if (isEmptyQueue(queue))
    {
        printf("Queue kosong!\n");
        item = -1;
    }
    else
    {
        item = queue->items[queue->front];
        queue->front++;
        if (queue->front > queue->rear)
        {
            queue->front = queue->rear = -1;
        }
    }
    return item;
}

// Fungsi untuk menambahkan elemen ke stack
void push(Stack *stack, int value)
{
    stack->top++;
    stack->items[stack->top] = value;
}

// Fungsi untuk menghapus elemen dari stack
int pop(Stack *stack)
{
    int item;
    if (isEmptyStack(stack))
    {
        printf("Stack kosong!\n");
        item = -1;
    }
    else
    {
        item = stack->items[stack->top];
        stack->top--;
    }
    return item;
}

// Fungsi untuk menghitung bobot kesamaan antara dua film
int hitungBobot(Film film1, Film film2)
{
    int bobot = 0;

    if (strcmp(film1.genre, film2.genre) == 0)
    {
        bobot += 3;
    }
    if (strcmp(film1.aktor, film2.aktor) == 0)
    {
        bobot += 2;
    }
    if (abs(film1.tahun - film2.tahun) <= 5)
    {
        bobot += 1;
    }

    return bobot;
}

// Fungsi untuk menambahkan edge ke graf
void addEdge(Graph *graph, int src, int dest)
{
    int weight = hitungBobot(graph->films[src], graph->films[dest]);

    if (weight > 0)
    {
        // Tambah edge dari src ke dest
        AdjListNode *newNode = newAdjListNode(dest, weight);
        newNode->next = graph->array[src].head;
        graph->array[src].head = newNode;

        // Tambah edge dari dest ke src (graf tidak berarah)
        newNode = newAdjListNode(src, weight);
        newNode->next = graph->array[dest].head;
        graph->array[dest].head = newNode;
    }
}

// Fungsi untuk menambahkan film ke graf
void addFilm(Graph *graph, char judul[], char genre[], char aktor[], int tahun)
{
    if (graph->num_films >= MAX_FILM)
    {
        printf("Kapasitas film penuh!\n");
        return;
    }

    Film newFilm;
    strcpy(newFilm.judul, judul);
    strcpy(newFilm.genre, genre);
    strcpy(newFilm.aktor, aktor);
    newFilm.tahun = tahun;
    newFilm.visited = 0;

    graph->films[graph->num_films] = newFilm;

    // Tambah edge dengan film yang sudah ada
    for (int i = 0; i < graph->num_films; i++)
    {
        addEdge(graph, graph->num_films, i);
    }

    graph->num_films++;
    printf("Film '%s' berhasil ditambahkan!\n", judul);
}

// Fungsi untuk menghapus film dari graf
void deleteFilm(Graph *graph, int index)
{
    if (index < 0 || index >= graph->num_films)
    {
        printf("Index film tidak valid!\n");
        return;
    }

    printf("Film '%s' berhasil dihapus!\n", graph->films[index].judul);

    // Geser film yang ada
    for (int i = index; i < graph->num_films - 1; i++)
    {
        graph->films[i] = graph->films[i + 1];
    }

    // Reset adjacency list dan bangun ulang
    for (int i = 0; i < MAX_FILM; i++)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp != NULL)
        {
            AdjListNode *toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
        graph->array[i].head = NULL;
    }

    graph->num_films--;

    // Bangun ulang semua edge
    for (int i = 0; i < graph->num_films; i++)
    {
        for (int j = i + 1; j < graph->num_films; j++)
        {
            addEdge(graph, i, j);
        }
    }
}

// Fungsi untuk mencari film berdasarkan judul
int searchFilm(Graph *graph, char *judul)
{
    char searchTitle[MAX_STRING];
    strcpy(searchTitle, judul);
    toLowerCase(searchTitle);
    for (int i = 0; i < graph->num_films; i++)
    {
        char filmTitle[MAX_STRING];
        strcpy(filmTitle, graph->films[i].judul);
        toLowerCase(filmTitle);

        if (strstr(filmTitle, searchTitle) != NULL)
        {
            return i;
        }
    }
    return -1;
}

// Fungsi untuk menampilkan semua film
void displayAllFilms(Graph *graph)
{
    printf("\n=== DAFTAR SEMUA FILM ===\n");
    for (int i = 0; i < graph->num_films; i++)
    {
        printf("%d. %s (%d)\n", i + 1, graph->films[i].judul, graph->films[i].tahun);
        printf("   Genre: %s\n", graph->films[i].genre);
        printf("   Aktor Utama: %s\n", graph->films[i].aktor);

        AdjListNode *temp = graph->array[i].head;
        if (temp != NULL)
        {
            // Hitung jumlah node
            int count = 0;
            AdjListNode *t = temp;
            while (t != NULL)
            {
                count++;
                t = t->next;
            }

            // Salin ke array
            AdjListNode **arr = (AdjListNode **)malloc(count * sizeof(AdjListNode *));
            t = temp;
            for (int k = 0; k < count; k++)
            {
                arr[k] = t;
                t = t->next;
            }

            // Sort array berdasarkan bobot (descending)
            for (int a = 0; a < count - 1; a++)
            {
                for (int b = 0; b < count - a - 1; b++)
                {
                    if (arr[b]->weight < arr[b + 1]->weight)
                    {
                        AdjListNode *tmp = arr[b];
                        arr[b] = arr[b + 1];
                        arr[b + 1] = tmp;
                    }
                }
            }

            // Cetak hasil sorting
            printf("   Film Terkait : ");
            for (int k = 0; k < count; k++)
            {
                printf("%s (bobot: %d)", graph->films[arr[k]->dest].judul, arr[k]->weight);
                if (k < count - 1) printf(", ");
            }
            printf("\n");
            free(arr);
        }
    }
}

// Fungsi BFS traversal dengan indentasi
void BFS(Graph *graph, int startVertex)
{
    printf("\n=== COMFORT ZONE (BFS TRAVERSAL) ===\n");

    // Reset visited status
    for (int i = 0; i < graph->num_films; i++)
    {
        graph->films[i].visited = 0;
    }

    Queue *queue = createQueue();
    int level[MAX_FILM] = {0};

    graph->films[startVertex].visited = 1;
    level[startVertex] = 0;
    enqueue(queue, startVertex);

    while (!isEmptyQueue(queue))
    {
        int currentVertex = dequeue(queue);

        // Cetak dengan indentasi sesuai level
        for (int i = 0; i < level[currentVertex]; i++)
        {
            printf("  ");
        }
        printf("- %s (%d)\n", graph->films[currentVertex].judul, graph->films[currentVertex].tahun);

        AdjListNode *temp = graph->array[currentVertex].head;
        while (temp != NULL)
        {
            int adjVertex = temp->dest;

            if (!graph->films[adjVertex].visited)
            {
                graph->films[adjVertex].visited = 1;
                level[adjVertex] = level[currentVertex] + 1;
                enqueue(queue, adjVertex);
            }
            temp = temp->next;
        }
    }

    // Tampilkan node yang tidak terhubung
    printf("\nFilm yang tidak terhubung:\n");
    for (int i = 0; i < graph->num_films; i++)
    {
        if (!graph->films[i].visited)
        {
            for (int j = 0; j < level[startVertex] + 1; j++)
            {
                printf("  ");
            }
            printf("- %s (%d)\n", graph->films[i].judul, graph->films[i].tahun);
        }
    }

    free(queue);
}

// Fungsi DFS rekursif dengan indentasi
void DFSRecursive(Graph *graph, int vertex, int depth)
{
    // Cetak dengan indentasi sesuai depth
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    printf("- %s (%d)\n", graph->films[vertex].judul, graph->films[vertex].tahun);

    graph->films[vertex].visited = 1;

    AdjListNode *temp = graph->array[vertex].head;
    while (temp != NULL)
    {
        int adjVertex = temp->dest;
        if (!graph->films[adjVertex].visited)
        {
            DFSRecursive(graph, adjVertex, depth + 1);
        }
        temp = temp->next;
    }
}

// Fungsi DFS traversal
void DFS(Graph *graph, int startVertex)
{
    printf("\n=== EXPLORATION (DFS TRAVERSAL) ===\n");

    // Reset visited status
    for (int i = 0; i < graph->num_films; i++)
    {
        graph->films[i].visited = 0;
    }

    DFSRecursive(graph, startVertex, 0);

    // Tampilkan node yang tidak terhubung
    printf("\nFilm yang tidak terhubung:\n");
    for (int i = 0; i < graph->num_films; i++)
    {
        if (!graph->films[i].visited)
        {
            for (int j = 0; j < 1; j++)
            {
                printf("  ");
            }
            printf("- %s (%d)\n", graph->films[i].judul, graph->films[i].tahun);
        }
    }
}

// Fungsi untuk mendapatkan daftar unik genre
void getUniqueGenres(Graph *graph, char genres[][MAX_STRING], int *count)
{
    *count = 0;
    for (int i = 0; i < graph->num_films; i++)
    {
        int found = 0;
        for (int j = 0; j < *count; j++)
        {
            if (strcmp(genres[j], graph->films[i].genre) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            strcpy(genres[*count], graph->films[i].genre);
            (*count)++;
        }
    }
}

// Fungsi untuk mendapatkan daftar unik aktor
void getUniqueActors(Graph *graph, char actors[][MAX_STRING], int *count)
{
    *count = 0;
    for (int i = 0; i < graph->num_films; i++)
    {
        int found = 0;
        for (int j = 0; j < *count; j++)
        {
            if (strcmp(actors[j], graph->films[i].aktor) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            strcpy(actors[*count], graph->films[i].aktor);
            (*count)++;
        }
    }
}

// Fungsi untuk mendapatkan daftar unik tahun
void getUniqueYears(Graph *graph, int years[], int *count)
{
    *count = 0;
    for (int i = 0; i < graph->num_films; i++)
    {
        int found = 0;
        for (int j = 0; j < *count; j++)
        {
            if (years[j] == graph->films[i].tahun)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            years[*count] = graph->films[i].tahun;
            (*count)++;
        }
    }
}

// Fungsi untuk merekomendasikan film berdasarkan preferensi
void recommendByPreference(Graph *graph)
{
    if (graph->num_films == 0)
    {
        printf("Tidak ada film dalam database!\n");
        return;
    }

    char genres[MAX_FILM][MAX_STRING];
    char actors[MAX_FILM][MAX_STRING];
    int years[MAX_FILM];
    int genreCount, actorCount, yearCount;

    getUniqueGenres(graph, genres, &genreCount);
    getUniqueActors(graph, actors, &actorCount);
    getUniqueYears(graph, years, &yearCount);

    printf("\n=== REKOMENDASI BERDASARKAN PREFERENSI ===\n");

    // Pilih genre
    printf("Pilih genre:\n");
    for (int i = 0; i < genreCount; i++)
    {
        printf("%d. %s\n", i + 1, genres[i]);
    }
    int genreChoice;
    printf("Masukkan pilihan (1-%d): ", genreCount);
    scanf("%d", &genreChoice);
    getchar();

    // Pilih aktor
    printf("\nPilih aktor utama:\n");
    for (int i = 0; i < actorCount; i++)
    {
        printf("%d. %s\n", i + 1, actors[i]);
    }
    int actorChoice;
    printf("Masukkan pilihan (1-%d): ", actorCount);
    scanf("%d", &actorChoice);
    getchar();

    // Pilih tahun
    printf("\nPilih tahun:\n");
    for (int i = 0; i < yearCount; i++)
    {
        printf("%d. %d\n", i + 1, years[i]);
    }
    int yearChoice;
    printf("Masukkan pilihan (1-%d): ", yearCount);
    scanf("%d", &yearChoice);
    getchar();

    // Buat film preferensi
    Film preference;
    strcpy(preference.genre, genres[genreChoice - 1]);
    strcpy(preference.aktor, actors[actorChoice - 1]);
    preference.tahun = years[yearChoice - 1];

    // Hitung skor untuk setiap film
    int scores[MAX_FILM];
    int maxScore = -1, secondMax = -1, thirdMax = -1;
    int bestFilm = -1, secondBest = -1, thirdBest = -1;

    for (int i = 0; i < graph->num_films; i++)
    {
        scores[i] = hitungBobot(preference, graph->films[i]);

        if (scores[i] > maxScore)
        {
            thirdMax = secondMax;
            thirdBest = secondBest;
            secondMax = maxScore;
            secondBest = bestFilm;
            maxScore = scores[i];
            bestFilm = i;
        }
        else if (scores[i] > secondMax)
        {
            thirdMax = secondMax;
            thirdBest = secondBest;
            secondMax = scores[i];
            secondBest = i;
        }
        else if (scores[i] > thirdMax)
        {
            thirdMax = scores[i];
            thirdBest = i;
        }
    }

    printf("\n=== HASIL REKOMENDASI ===\n");

    if (bestFilm != -1 && maxScore > 0)
    {
        printf("REKOMENDASI UTAMA:\n");
        printf("- %s (Skor: %d)\n", graph->films[bestFilm].judul, maxScore);
        printf("  Genre: %s, Aktor: %s, Tahun: %d\n\n", graph->films[bestFilm].genre, graph->films[bestFilm].aktor, graph->films[bestFilm].tahun);

        printf("REKOMENDASI TAMBAHAN:\n");
        if (secondBest != -1 && secondMax > 0)
        {
            printf("- %s (Skor: %d)\n", graph->films[secondBest].judul, secondMax);
            printf("  Genre: %s, Aktor: %s, Tahun: %d\n", graph->films[secondBest].genre, graph->films[secondBest].aktor, graph->films[secondBest].tahun);
        }
        if (thirdBest != -1 && thirdMax > 0)
        {
            printf("- %s (Skor: %d)\n", graph->films[thirdBest].judul, thirdMax);
            printf("  Genre: %s, Aktor: %s, Tahun: %d\n", graph->films[thirdBest].genre, graph->films[thirdBest].aktor, graph->films[thirdBest].tahun);
        }
    }
    else
    {
        printf("Tidak ditemukan film yang sesuai dengan preferensi Anda.\n");
    }
}

// Fungsi untuk inisialisasi data film awal
void initializeData(Graph *graph)
{
    // Data film awal (minimal 10 film)
    addFilm(graph, "Inception", "Sci-Fi", "Leonardo DiCaprio", 2010);
    addFilm(graph, "Titanic", "Romance", "Leonardo DiCaprio", 1997);
    addFilm(graph, "Interstellar", "Sci-Fi", "Matthew McConaughey", 2014);
    addFilm(graph, "The Dark Knight", "Action", "Christian Bale", 2008);
    addFilm(graph, "Memento", "Mystery", "Guy Pearce", 2000);
    addFilm(graph, "Shutter Island", "Mystery", "Leonardo DiCaprio", 2010);
    addFilm(graph, "Avatar", "Sci-Fi", "Sam Worthington", 2009);
    addFilm(graph, "Dunkirk", "War", "Fionn Whitehead", 2017);
    addFilm(graph, "Joker", "Drama", "Joaquin Phoenix", 2019);
    addFilm(graph, "Tenet", "Sci-Fi", "John David Washington", 2020);
}

// Fungsi utama
int main()
{
    Graph *graph = createGraph();
    initializeData(graph);

    int choice;
    char judul[MAX_STRING], genre[MAX_STRING], aktor[MAX_STRING];
    int tahun, index;

    printf("=== SISTEM REKOMENDASI FILM ===\n");

    do
    {
        printf("\n=== MENU UTAMA ===\n");
        printf("1. TANYA REKOMENDASI FILM\n");
        printf("2. TAMPILKAN SELURUH FILM\n");
        printf("3. TAMBAH FILM BARU\n");
        printf("4. CARI FILM\n");
        printf("5. HAPUS FILM\n");
        printf("0. KELUAR\n");
        printf("Pilihan: ");
        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        case 1:
            printf("\n1. Berdasarkan Preferensi Pribadi\n");
            printf("2. Comfort Zone (BFS)\n");
            printf("3. Exploration (DFS)\n");
            printf("Pilihan rekomendasi: ");
            int subChoice;
            scanf("%d", &subChoice);
            getchar();

            switch (subChoice)
            {
            case 1:
                recommendByPreference(graph);
                break;
            case 2:
                displayAllFilms(graph);
                printf("Masukkan index film awal (0-%d): ", graph->num_films - 1);
                scanf("%d", &index);
                getchar();
                if (index >= 0 && index < graph->num_films)
                {
                    BFS(graph, index);
                }
                else
                {
                    printf("Index tidak valid!\n");
                }
                break;
            case 3:
                displayAllFilms(graph);
                printf("Masukkan index film awal (0-%d): ", graph->num_films - 1);
                scanf("%d", &index);
                getchar();
                if (index >= 0 && index < graph->num_films)
                {
                    DFS(graph, index);
                }
                else
                {
                    printf("Index tidak valid!\n");
                }
                break;
            default:
                printf("Pilihan tidak valid!\n");
            }
            break;

        case 2:
            displayAllFilms(graph);
            break;

        case 3:
            printf("Masukkan judul film: ");
            fgets(judul, MAX_STRING, stdin);
            judul[strcspn(judul, "\n")] = 0;

            printf("Masukkan genre: ");
            fgets(genre, MAX_STRING, stdin);
            genre[strcspn(genre, "\n")] = 0;

            printf("Masukkan aktor utama: ");
            fgets(aktor, MAX_STRING, stdin);
            aktor[strcspn(aktor, "\n")] = 0;

            printf("Masukkan tahun rilis: ");
            scanf("%d", &tahun);
            getchar();

            addFilm(graph, judul, genre, aktor, tahun);
            break;

        case 4:
            printf("Masukkan judul film yang dicari: ");
            fgets(judul, MAX_STRING, stdin);
            judul[strcspn(judul, "\n")] = 0;

            index = searchFilm(graph, judul);
            if (index != -1)
            {
                printf("\nFilm ditemukan:\n");
                printf("Judul: %s\n", graph->films[index].judul);
                printf("Genre: %s\n", graph->films[index].genre);
                printf("Aktor Utama: %s\n", graph->films[index].aktor);
                printf("Tahun: %d\n", graph->films[index].tahun);
            }
            else
            {
                printf("Film tidak ditemukan!\n");
            }
            break;

        case 5:
            displayAllFilms(graph);
            printf("Masukkan index film yang akan dihapus (1-%d): ", graph->num_films);
            scanf("%d", &index);
            getchar();
            if (index >= 1 && index <= graph->num_films)
            {
                deleteFilm(graph, index - 1);
            }
            else
            {
                printf("Index tidak valid!\n");
            }
            break;

        case 0:
            printf("Terima kasih telah menggunakan sistem rekomendasi film!\n");
            break;

        default:
            printf("Pilihan tidak valid!\n");
        }
    } while (choice != 0);

    // Free memory
    for (int i = 0; i < MAX_FILM; i++)
    {
        AdjListNode *temp = graph->array[i].head;
        while (temp != NULL)
        {
            AdjListNode *toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
    }
    free(graph->array);
    free(graph);

    return 0;
}