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
        str[i] = tolower((unsigned char)str[i]);
    }
}

// Case-insensitive equals (menghindari penggunaan strcasecmp untuk portabilitas)
int equalsIgnoreCase(const char *a, const char *b)
{
    char ta[MAX_STRING], tb[MAX_STRING];
    strncpy(ta, a, MAX_STRING);
    strncpy(tb, b, MAX_STRING);
    ta[MAX_STRING - 1] = '\0';
    tb[MAX_STRING - 1] = '\0';
    toLowerCase(ta);
    toLowerCase(tb);
    return strcmp(ta, tb) == 0;
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
        // printf("Queue kosong!\n"); // jangan spam pesan di traversal
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
        // printf("Stack kosong!\n");
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
    if (src < 0 || src >= graph->num_films || dest < 0 || dest >= graph->num_films)
        return;

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

void exportToCSV(Graph *graph, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        printf("Gagal membuat file CSV!\n");
        return;
    }

    // Header CSV
    fprintf(fp, "Judul,Genre,Aktor,Tahun\n");

    // Isi CSV
    for (int i = 0; i < graph->num_films; i++)
    {
        fprintf(fp, "\"%s\",\"%s\",\"%s\",%d\n",
                graph->films[i].judul,
                graph->films[i].genre,
                graph->films[i].aktor,
                graph->films[i].tahun);
    }

    fclose(fp);
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
    strncpy(newFilm.judul, judul, MAX_STRING);
    strncpy(newFilm.genre, genre, MAX_STRING);
    strncpy(newFilm.aktor, aktor, MAX_STRING);
    newFilm.judul[MAX_STRING - 1] = '\0';
    newFilm.genre[MAX_STRING - 1] = '\0';
    newFilm.aktor[MAX_STRING - 1] = '\0';
    newFilm.tahun = tahun;
    newFilm.visited = 0;

    graph->films[graph->num_films] = newFilm;

    // Tambah edge dengan film yang sudah ada
    for (int i = 0; i < graph->num_films; i++)
    {
        addEdge(graph, graph->num_films, i);
    }

    graph->num_films++;
    exportToCSV(graph, "film.csv");
    printf("Film '%s' berhasil ditambahkan!\n", judul);
}

void loadFromCSV(Graph *graph, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("CSV tidak ditemukan. Program dimulai tanpa data.\n");
        return;
    }

    char line[256];

    // Lewati baris header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp))
    {
        char judul[100], genre[100], aktor[100];
        int tahun;

        // Format: "Judul","Genre","Aktor",Tahun
        if (sscanf(line, " \"%[^\"]\",\"%[^\"]\",\"%[^\"]\",%d", judul, genre, aktor, &tahun) == 4)
        {
            Film f;
            strncpy(f.judul, judul, MAX_STRING);
            strncpy(f.genre, genre, MAX_STRING);
            strncpy(f.aktor, aktor, MAX_STRING);
            f.judul[MAX_STRING - 1] = '\0';
            f.genre[MAX_STRING - 1] = '\0';
            f.aktor[MAX_STRING - 1] = '\0';
            f.tahun = tahun;
            f.visited = 0;

            graph->films[graph->num_films++] = f;
        }
        else
        {
            // coba format tanpa spasi awal
            if (sscanf(line, "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",%d", judul, genre, aktor, &tahun) == 4)
            {
                Film f;
                strncpy(f.judul, judul, MAX_STRING);
                strncpy(f.genre, genre, MAX_STRING);
                strncpy(f.aktor, aktor, MAX_STRING);
                f.judul[MAX_STRING - 1] = '\0';
                f.genre[MAX_STRING - 1] = '\0';
                f.aktor[MAX_STRING - 1] = '\0';
                f.tahun = tahun;
                f.visited = 0;

                graph->films[graph->num_films++] = f;
            }
        }
    }

    for (int i = 0; i < graph->num_films; i++)
    {
        for (int j = i + 1; j < graph->num_films; j++)
        {
            addEdge(graph, i, j);
        }
    }

    fclose(fp);
    printf("Data berhasil dimuat dari CSV.\n");
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
    for (int i = 0; i < graph->num_films; i++)
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
    exportToCSV(graph, "film.csv");
}

// Fungsi untuk mencari film berdasarkan judul
int searchFilm(Graph *graph, char *judul)
{
    char searchTitle[MAX_STRING];
    strncpy(searchTitle, judul, MAX_STRING);
    searchTitle[MAX_STRING - 1] = '\0';
    toLowerCase(searchTitle);
    for (int i = 0; i < graph->num_films; i++)
    {
        char filmTitle[MAX_STRING];
        strncpy(filmTitle, graph->films[i].judul, MAX_STRING);
        filmTitle[MAX_STRING - 1] = '\0';
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
                if (k < count - 1)
                    printf(", ");
            }
            printf("\n");
            free(arr);
        }
    }
}

// Fungsi BFS traversal dengan indentasi
void BFS(Graph *graph, int startVertex)
{
    if (startVertex < 0 || startVertex >= graph->num_films)
    {
        printf("Index awal tidak valid untuk BFS.\n");
        return;
    }

    printf("\n=== COMFORT ZONE (BFS TRAVERSAL) ===\n");

    // Reset visited status
    for (int i = 0; i < graph->num_films; i++)
    {
        graph->films[i].visited = 0;
    }

    Queue *queue = createQueue();
    int level[MAX_FILM];
    int parentWeight[MAX_FILM];
    for (int i = 0; i < MAX_FILM; i++)
    {
        level[i] = -1;
        parentWeight[i] = 0;
    }

    graph->films[startVertex].visited = 1;
    level[startVertex] = 0;
    parentWeight[startVertex] = 0;
    enqueue(queue, startVertex);

    while (!isEmptyQueue(queue))
    {
        int currentVertex = dequeue(queue);
        if (currentVertex == -1)
            break;

        // Cetak dengan indentasi sesuai level
        for (int i = 0; i < level[currentVertex]; i++)
        {
            printf("  ");
        }

        // Tampilkan node
        if (level[currentVertex] == 0)
        {
            printf("- %s (%d)\n", graph->films[currentVertex].judul, graph->films[currentVertex].tahun);
        }
        else
        {
            printf("- %s (bobot: %d)\n", graph->films[currentVertex].judul, parentWeight[currentVertex]);
        }

        // Kumpulkan semua tetangga yang BELUM VISITED
        AdjListNode *temp = graph->array[currentVertex].head;
        int neighbors[MAX_FILM];
        int weights[MAX_FILM];
        int neighborCount = 0;

        // Cek visited SEBELUM sorting
        while (temp != NULL)
        {
            int adjVertex = temp->dest;
            if (!graph->films[adjVertex].visited) // Cek di sini
            {
                neighbors[neighborCount] = adjVertex;
                weights[neighborCount] = temp->weight;
                neighborCount++;
                graph->films[adjVertex].visited = 1; // MARK VISITED DI SINI!
            }
            temp = temp->next;
        }

        // Sort tetangga berdasarkan bobot (descending)
        for (int i = 0; i < neighborCount - 1; i++)
        {
            for (int j = 0; j < neighborCount - i - 1; j++)
            {
                if (weights[j] < weights[j + 1])
                {
                    // Swap weights
                    int tempWeight = weights[j];
                    weights[j] = weights[j + 1];
                    weights[j + 1] = tempWeight;

                    // Swap neighbors
                    int tempNeighbor = neighbors[j];
                    neighbors[j] = neighbors[j + 1];
                    neighbors[j + 1] = tempNeighbor;
                }
            }
        }

        // Enqueue tetangga yang sudah disort
        for (int i = 0; i < neighborCount; i++)
        {
            level[neighbors[i]] = level[currentVertex] + 1;
            parentWeight[neighbors[i]] = weights[i];
            enqueue(queue, neighbors[i]);
        }
    }

    // Tampilkan node yang tidak terhubung
    printf("\nFilm yang tidak terhubung:\n");
    for (int i = 0; i < graph->num_films; i++)
    {
        if (!graph->films[i].visited)
        {
            printf("  - %s (%d)\n", graph->films[i].judul, graph->films[i].tahun);
        }
    }

    free(queue);
}

// Fungsi DFS rekursif dengan indentasi
void DFSRecursive(Graph *graph, int vertex, int depth, int edgeWeight)
{
    // 1. Cetak node saat ini
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    if (depth == 0)
    {
        printf("- %s (%d)\n", graph->films[vertex].judul, graph->films[vertex].tahun);
    }

    else
    {
        printf("- %s (bobot: %d)\n", graph->films[vertex].judul, edgeWeight);
    }

    graph->films[vertex].visited = 1;

    // 2. Kumpulkan tetangga yang belum dikunjungi
    AdjListNode *temp = graph->array[vertex].head;
    int neighbors[MAX_FILM];
    int weights[MAX_FILM];
    int neighborCount = 0;

    while (temp != NULL)
    {
        int adjVertex = temp->dest;
        if (!graph->films[adjVertex].visited)
        {
            neighbors[neighborCount] = adjVertex;
            weights[neighborCount] = temp->weight;
            neighborCount++;
        }
        temp = temp->next;
    }

    // 3. Sort tetangga berdasarkan bobot (Descending) agar menelusuri yang paling mirip dulu
    for (int i = 0; i < neighborCount - 1; i++)
    {
        for (int j = 0; j < neighborCount - i - 1; j++)
        {
            if (weights[j] < weights[j + 1])
            {
                // Swap weights
                int tempWeight = weights[j];
                weights[j] = weights[j + 1];
                weights[j + 1] = tempWeight;

                // Swap neighbors
                int tempNeighbor = neighbors[j];
                neighbors[j] = neighbors[j + 1];
                neighbors[j + 1] = tempNeighbor;
            }
        }
    }

    // 4. Lakukan rekursi pada tetangga yang sudah diurutkan
    for (int i = 0; i < neighborCount; i++)
    {
        // Cek lagi visited karena bisa jadi sudah dikunjungi oleh cabang rekursi lain dalam loop ini
        if (!graph->films[neighbors[i]].visited)
        {
            DFSRecursive(graph, neighbors[i], depth + 1, weights[i]);
        }
    }
}

// Fungsi traversal DFS
void DFS(Graph *graph, int startVertex)
{
    if (startVertex < 0 || startVertex >= graph->num_films)
    {
        printf("Index awal tidak valid untuk DFS.\n");
        return;
    }

    printf("\n=== EXPLORATION (DFS TRAVERSAL) ===\n");

    // Reset visited status
    for (int i = 0; i < graph->num_films; i++)
    {
        graph->films[i].visited = 0;
    }

    DFSRecursive(graph, startVertex, 0, 0); // Start dengan bobot 0

    // Tampilkan node yang tidak terhubung
    printf("\nFilm yang tidak terhubung:\n");
    for (int i = 0; i < graph->num_films; i++)
    {
        if (!graph->films[i].visited)
        {
            printf("  - %s (%d)\n", graph->films[i].judul, graph->films[i].tahun);
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
            strncpy(genres[*count], graph->films[i].genre, MAX_STRING);
            genres[*count][MAX_STRING - 1] = '\0';
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
            strncpy(actors[*count], graph->films[i].aktor, MAX_STRING);
            actors[*count][MAX_STRING - 1] = '\0';
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

// Fungsi untuk merekomendasikan film berdasarkan preferensi (existing)
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

    if (genreChoice < 1 || genreChoice > genreCount)
    {
        printf("Pilihan tidak valid!\n");
        return;
    }

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
    strncpy(preference.genre, genres[genreChoice - 1], MAX_STRING);
    strncpy(preference.aktor, actors[actorChoice - 1], MAX_STRING);
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

void recommendSingleFilter(Graph *graph)
{
    if (graph->num_films == 0)
    {
        printf("Tidak ada film dalam database!\n");
        return;
    }

    int choice;
    printf("\n=== REKOMENDASI BY GENRE / TAHUN / AKTOR ===\n");
    printf("1. Berdasarkan Genre\n");
    printf("2. Berdasarkan Tahun\n");
    printf("3. Berdasarkan Aktor\n");
    printf("Pilihan: ");
    scanf("%d", &choice);
    getchar();

    char genres[MAX_FILM][MAX_STRING];
    char actors[MAX_FILM][MAX_STRING];
    int years[MAX_FILM];
    int genreCount, actorCount, yearCount;

    getUniqueGenres(graph, genres, &genreCount);
    getUniqueActors(graph, actors, &actorCount);
    getUniqueYears(graph, years, &yearCount);

    int selected, found = 0;

    printf("\n=== HASIL REKOMENDASI ===\n");

    switch (choice)
    {
    case 1:
        printf("\nPilih Genre:\n");
        for (int i = 0; i < genreCount; i++)
            printf("%d. %s\n", i + 1, genres[i]);

        printf("Masukkan pilihan (1-%d): ", genreCount);
        scanf("%d", &selected);
        getchar();

        if (selected < 1 || selected > genreCount)
        {
            printf("Pilihan tidak valid!\n");
            return;
        }

        for (int i = 0; i < graph->num_films; i++)
        {
            if (strcmp(graph->films[i].genre, genres[selected - 1]) == 0)
            {
                printf("- %s (%d), Aktor: %s\n",
                       graph->films[i].judul,
                       graph->films[i].tahun,
                       graph->films[i].aktor);
                found = 1;
            }
        }
        break;

    case 2:
        printf("\nPilih Tahun:\n");
        for (int i = 0; i < yearCount; i++)
            printf("%d. %d\n", i + 1, years[i]);

        printf("Masukkan pilihan (1-%d): ", yearCount);
        scanf("%d", &selected);
        getchar();

        if (selected < 1 || selected > yearCount)
        {
            printf("Pilihan tidak valid!\n");
            return;
        }

        for (int i = 0; i < graph->num_films; i++)
        {
            if (graph->films[i].tahun == years[selected - 1])
            {
                printf("- %s (%d), Genre: %s, Aktor: %s\n",
                       graph->films[i].judul,
                       graph->films[i].tahun,
                       graph->films[i].genre,
                       graph->films[i].aktor);
                found = 1;
            }
        }
        break;

    case 3:
        printf("\nPilih Aktor:\n");
        for (int i = 0; i < actorCount; i++)
            printf("%d. %s\n", i + 1, actors[i]);

        printf("Masukkan pilihan (1-%d): ", actorCount);
        scanf("%d", &selected);
        getchar();

        if (selected < 1 || selected > actorCount)
        {
            printf("Pilihan tidak valid!\n");
            return;
        }

        for (int i = 0; i < graph->num_films; i++)
        {
            if (strcmp(graph->films[i].aktor, actors[selected - 1]) == 0)
            {
                printf("- %s (%d), Genre: %s\n",
                       graph->films[i].judul,
                       graph->films[i].tahun,
                       graph->films[i].genre);
                found = 1;
            }
        }
        break;

    default:
        printf("Pilihan tidak valid!\n");
        return;
    }

    if (!found)
    {
        printf("Tidak ditemukan film yang cocok.\n");
    }
}

// Fungsi utama
int main()
{
    Graph *graph = createGraph();
    loadFromCSV(graph, "film.csv");

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
        if (scanf("%d", &choice) != 1)
        {
            printf("Input tidak valid.\n");
            while (getchar() != '\n')
                ;
            continue;
        }
        getchar();

        switch (choice)
        {
        case 1:
        {
            printf("\n1. Berdasarkan Preferensi Pribadi\n");
            printf("2. Comfort Zone (BFS)\n");
            printf("3. Exploration (DFS)\n");
            printf("4. Berdasarkan Genre/Tahun/Aktor\n");
            printf("0. Kembali ke menu\n");
            printf("Pilihan rekomendasi: ");
            int subChoice;
            if (scanf("%d", &subChoice) != 1)
            {
                printf("Input tidak valid.\n");
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();

            switch (subChoice)
            {
            case 1:
                recommendByPreference(graph);
                break;
            case 2:
                displayAllFilms(graph);
                printf("Masukkan index film awal (1-%d): ", graph->num_films);
                if (scanf("%d", &index) != 1)
                {
                    printf("Input tidak valid.\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                getchar();
                if (index >= 1 && index <= graph->num_films)
                {
                    BFS(graph, index - 1);
                }
                else
                {
                    printf("Index tidak valid!\n");
                }
                break;
            case 3:
                displayAllFilms(graph);
                printf("Masukkan index film awal (1-%d): ", graph->num_films);
                if (scanf("%d", &index) != 1)
                {
                    printf("Input tidak valid.\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                getchar();
                if (index >= 1 && index <= graph->num_films)
                {
                    DFS(graph, index - 1);
                }
                else
                {
                    printf("Index tidak valid!\n");
                }
                break;
            case 4:
                recommendSingleFilter(graph);
                break;
            case 0:
                printf("Kembali ke menu.\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
            }
            break;
        }

        case 2:
            displayAllFilms(graph);
            break;

        case 3:
            printf("Masukkan judul film: ");
            fgets(judul, MAX_STRING, stdin);
            judul[strcspn(judul, "\n")] = '\0';

            printf("Masukkan genre: ");
            fgets(genre, MAX_STRING, stdin);
            genre[strcspn(genre, "\n")] = '\0';

            printf("Masukkan aktor utama: ");
            fgets(aktor, MAX_STRING, stdin);
            aktor[strcspn(aktor, "\n")] = '\0';

            printf("Masukkan tahun rilis: ");
            if (scanf("%d", &tahun) != 1)
            {
                printf("Input tahun tidak valid.\n");
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();

            {
                char konfirmasi;
                printf("Apakah anda yakin akan menambah film?(y/n)\n");
                if (scanf("%c", &konfirmasi) != 1)
                {
                    printf("Input tidak valid.\n");
                    while (getchar() != '\n')
                        ;
                    break;
                }
                getchar();
                if (konfirmasi == 'y')
                {
                    addFilm(graph, judul, genre, aktor, tahun);
                }
                else if (konfirmasi == 'n')
                {
                    printf("Batal menambahkan film.\n");
                }
                else
                {
                    printf("Input tidak valid.\n");
                }
            }

            break;

        case 4:
            printf("Masukkan judul film yang dicari: \n");
            fgets(judul, MAX_STRING, stdin);
            judul[strcspn(judul, "\n")] = '\0';

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
            printf("\nMasukkan index film yang akan dihapus (1-%d): \n", graph->num_films);
            printf("(Ketik 0 untuk membatalkan.)\n");
            if (scanf("%d", &index) != 1)
            {
                printf("Input tidak valid.\n");
                while (getchar() != '\n')
                    ;
                break;
            }
            getchar();
            if (index >= 1 && index <= graph->num_films)
            {
                deleteFilm(graph, index - 1);
            }
            else if (index == 0)
            {
                printf("Membatalkan penghapusan film.\n");
                break;
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