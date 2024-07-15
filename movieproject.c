#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define maximum size constants
#define MAX_NAME_SIZE 100
#define MAX_GENRE_SIZE 50

// Define structure for the movie details in BST
typedef struct TreeNode {
    char movieName[MAX_NAME_SIZE];
    char genre[MAX_GENRE_SIZE];
    float imdbRating;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Define structure for a movie in the genre graph
typedef struct MovieNode {
    char movieName[MAX_NAME_SIZE];
    float imdbRating;
    struct MovieNode* next;
} MovieNode;

// Define structure for the genre node in the graph
typedef struct GraphNode {
    char genre[MAX_GENRE_SIZE];
    MovieNode* movieList;
    struct GraphNode* nextGenre;
} GraphNode;

// Utility function to convert string to lowercase
void toLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Functions for BST
TreeNode* newBSTNode(char* movieName, char* genre, float imdbRating) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    strcpy(newNode->movieName, movieName);
    strcpy(newNode->genre, genre);
    newNode->imdbRating = imdbRating;
    newNode->left = newNode->right = NULL;
    return newNode;
}

TreeNode* insertBST(TreeNode* root, char* movieName, char* genre, float imdbRating) {
    char lowerMovieName[MAX_NAME_SIZE];
    char nodeLower[MAX_NAME_SIZE];

    strcpy(lowerMovieName, movieName);
    toLowerCase(lowerMovieName);

    if (!root) {
        return newBSTNode(movieName, genre, imdbRating);
    }

    strcpy(nodeLower, root->movieName);
    toLowerCase(nodeLower);

    if (strcmp(lowerMovieName, nodeLower) < 0) {
        root->left = insertBST(root->left, movieName, genre, imdbRating);
    } else if (strcmp(lowerMovieName, nodeLower) > 0) {
        root->right = insertBST(root->right, movieName, genre, imdbRating);
    }

    return root;
}

char* findMovieGenre(TreeNode* root, char* movieName) {
    char inputLower[MAX_NAME_SIZE];
    char nodeLower[MAX_NAME_SIZE];
    strcpy(inputLower, movieName);
    toLowerCase(inputLower);

    while (root) {
        strcpy(nodeLower, root->movieName);
        toLowerCase(nodeLower);

        if (strcmp(inputLower, nodeLower) == 0) {
            return root->genre;
        } else if (strcmp(inputLower, nodeLower) < 0) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
}

// Functions for genre-based graph
GraphNode* newGenreNode(char* genre) {
    GraphNode* newNode = (GraphNode*)malloc(sizeof(GraphNode));
    strcpy(newNode->genre, genre);
    newNode->movieList = NULL;
    newNode->nextGenre = NULL;
    return newNode;
}

MovieNode* newMovieNode(char* movieName, float imdbRating) {
    MovieNode* newNode = (MovieNode*)malloc(sizeof(MovieNode));
    strcpy(newNode->movieName, movieName);
    newNode->imdbRating = imdbRating;
    newNode->next = NULL;
    return newNode;
}

GraphNode* findGenre(GraphNode* head, char* genre) {
    while (head) {
        if (strcmp(head->genre, genre) == 0) {
            return head;
        }
        head = head->nextGenre;
    }
    return NULL;
}

void insertMovieToGenre(GraphNode* genreNode, char* movieName, float imdbRating) {
    MovieNode* movie = newMovieNode(movieName, imdbRating);
    movie->next = genreNode->movieList;
    genreNode->movieList = movie;
}

GraphNode* insertGenreAndMovie(GraphNode* head, char* genre, char* movieName, float imdbRating) {
    GraphNode* genreNode = findGenre(head, genre);
    if (!genreNode) {
        genreNode = newGenreNode(genre);
        genreNode->nextGenre = head;
        head = genreNode;
    }
    insertMovieToGenre(genreNode, movieName, imdbRating);
    return head;
}

// Utility function to trim newline characters

void trimNewline(char* str) {
    char* newLineChar = strchr(str, '\n');
    if (newLineChar) {
        *newLineChar = '\0';
    }
}

// Function to read CSV and populate data structures

void readCSVAndPopulate(char* filename, TreeNode** bstRoot, GraphNode** genreGraphHead) {

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char line[500];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        char* movieName = strtok(line, ",");
        char* genre = strtok(NULL, ",");
        strtok(NULL, ",");
        strtok(NULL, ",");
        
        char* ratingStr = strtok(NULL, ",");

        float imdbRating = atof(ratingStr);
        trimNewline(movieName);
        trimNewline(genre);

        *bstRoot = insertBST(*bstRoot, movieName, genre, imdbRating);

        char* singleGenre = strtok(genre, " ");
        while (singleGenre) {
            *genreGraphHead = insertGenreAndMovie(*genreGraphHead, singleGenre, movieName, imdbRating);
            singleGenre = strtok(NULL, " ");
        }
    }

    fclose(file);
}

// Display and recommendation functions

void displayMovies(TreeNode* root) {
    if (!root) return;

    displayMovies(root->left);

    // Check if the "genre" is a numeric value (year of release)
    char* genre = root->genre;
    int isYear = 1;
    for (int i = 0; genre[i]; i++) {
        if (!isdigit(genre[i])) {
            isYear = 0;
            break;
        }
    }

    if (!isYear) {
        printf("Movie: %s | Genre: %s\n", root->movieName, root->genre);
    }

    displayMovies(root->right);
}

void recommendMovies(GraphNode* genreGraphHead, char* genre) {
    GraphNode* genreNode = findGenre(genreGraphHead, genre);
    if (!genreNode) {
        printf("No recommendations available for this genre.\n");
        return;
    }

    printf("Recommendations based on genre '%s':\n", genre); // Corrected this line

    MovieNode* movie = genreNode->movieList;
    int count = 0;
    while (movie && count < 3) {
        printf("Movie: %s | IMDb Rating: %.1f\n", movie->movieName, movie->imdbRating);
        movie = movie->next;
        count++;
    }
}
void provideRecommendations(TreeNode* bstRoot, GraphNode* genreGraphHead, char* movieName) {
    char* genre = findMovieGenre(bstRoot, movieName);
    if (!genre) {
        printf("Movie not found.\n");
        return;
    }

    recommendMovies(genreGraphHead, genre);
}

// Function to free the memory allocated for the BST
void freeBST(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

// Function to free the memory allocated for the genre graph
void freeGenreGraph(GraphNode* head) {
    while (head != NULL) {
        MovieNode* movie = head->movieList;
        while (movie != NULL) {
            MovieNode* temp = movie;
            movie = movie->next;
            free(temp);
        }
        GraphNode* temp = head;
        head = head->nextGenre;
        free(temp);
    }
}

// Main function to integrate everything

int main() {
    TreeNode* bstRoot = NULL;
    GraphNode* genreGraphHead = NULL;
    readCSVAndPopulate("E:\\SEM 3\\dsl\\tree\\imdb_top_1000.csv", &bstRoot, &genreGraphHead);

    printf("All Movies and their Genres:\n");
    displayMovies(bstRoot);

    char movieName[MAX_NAME_SIZE];
    printf("\nEnter a movie name for recommendations: ");
    fgets(movieName, sizeof(movieName), stdin);
    trimNewline(movieName);

    provideRecommendations(bstRoot, genreGraphHead, movieName);

    // Cleanup code
    freeBST(bstRoot);
    freeGenreGraph(genreGraphHead);

    return 0;
}

