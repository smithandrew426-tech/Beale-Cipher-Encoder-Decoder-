/*
Andrew Smith G01373154
CS 262, Lab section 211
Project 2: This function decodes a message encoded 
with a Beale Cipher. It loads a cipher key from a 
file if it isn't already loaded, then prompts the 
user for the name of a file containing the encoded 
message. The function processes the encoded indices, 
maps them back to the characters in the cipher key, and 
outputs the decoded message.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void printHeader();
void printMenu();

char* strdup_custom(const char* str);
void cipherKey(FILE **currentFile, char *fileWords[5000], int *wordCount);
void cipherCreator(FILE **currentFile, char *fileWords[5000], int *wordCount);
void cipherDecoder(FILE **currentFile, char *fileWords[5000], int *wordCount);

int main() {

    printHeader();
    char buffer[100];
    FILE *currentFile = NULL;
    char *fileWords[5000];
    int words = 0;
    int *wordCount = &words;
    int menuChoice = 0;
    int inProgress = 1;

    while (inProgress) {
        printMenu();
        printf("\n");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            sscanf(buffer, "%i", &menuChoice);
        }

        if (menuChoice == 1) {
            cipherKey(&currentFile, fileWords, wordCount);
        } else if (menuChoice == 2) {
            cipherCreator(&currentFile, fileWords, wordCount);
        } else if (menuChoice == 3) {
            cipherDecoder(&currentFile, fileWords, wordCount);
        } else if (menuChoice == 4) {
            inProgress = 0;
        } else {
            printf("Improper input, please enter 1,2,3, or 4.\n");
        }
    }

    // Free allocated memory
    for (int i = 0; i < *wordCount; i++) {
        free(fileWords[i]);
    }

    return 0;
}

void cipherKey(FILE **currentFile, char *fileWords[5000], int *wordCount) {
    char buffer[100];
    char fileName[100];

    printf("Please enter the name of a text file containing the desired key (Spaces allowed): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Remove trailing newline character from fgets
        buffer[strcspn(buffer, "\n")] = '\0'; 
        
        // Copy buffer to fileName
        strcpy(fileName, buffer);  
    }

    *currentFile = fopen(fileName, "r");
    if (*currentFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char fileBuffer[100];
    int count = 0;

    // Read file line by line
    while (fgets(fileBuffer, sizeof(fileBuffer), *currentFile) != NULL) {
        char *token = strtok(fileBuffer, " \t\n");  // Get first word
        while (token != NULL) {
            if (count >= 5000) {
                printf("Too many words!\n");
                fclose(*currentFile);
                return;
            }

            // Allocate memory and store the word
            fileWords[count] = malloc(strlen(token) + 1);
            if (fileWords[count] == NULL) {
                printf("Memory allocation failed!\n");
                fclose(*currentFile);
                return;
            }

            strcpy(fileWords[count], token);  // Copy token into allocated memory
            for (int i = 0; i < strlen(fileWords[count]); i++) {
                fileWords[count][i] = tolower(fileWords[count][i]);
            }
            count++;  // Move to the next word

            token = strtok(NULL, " \t\n");  // Get next word
        }
    }

    *wordCount = count;
    fclose(*currentFile);

    /*// Print stored words (for debugging purposes)
    printf("\nStored Words:\n");
    for (int i = 0; i < *wordCount; i++) {
        printf("%s\n", fileWords[i]);
    }*/
}

void cipherCreator(FILE **currentFile, char *fileWords[5000], int *wordCount) {
    if (*currentFile == NULL || *wordCount == 0) {
        cipherKey(currentFile, fileWords, wordCount);
        if (*currentFile == NULL) {
            printf("Failed to load cipher key.\n");
            return;
        }
    }

    char buffer[1500];
    char message[1500];
    char encoded[10000] = ""; // Final encoded message string

    // Seed from current time
    srand((unsigned int)time(NULL));

    printf("Please enter a secret message (plain text): ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(message, buffer);

    for (int i = 0; message[i] != '\0'; i++) {
        char ch = tolower(message[i]);

        if (ch == ' ') {
            strcat(encoded, " ");
            continue;
        }

        int positions[5000][2];  // Store [word index][char index]
        int posCount = 0;

        for (int w = 0; w < *wordCount; w++) {
            for (int c = 0; c < strlen(fileWords[w]); c++) {
                if (fileWords[w][c] == ch) {
                    positions[posCount][0] = w;
                    positions[posCount][1] = c;
                    posCount++;
                }
            }
        }

        if (posCount > 0) {
            int index = rand() % posCount;

            char pair[20];
            sprintf(pair, "%d,%d", positions[index][0], positions[index][1]);
            strcat(encoded, pair);
        } else {
            strcat(encoded, "#");
        }

        // Add comma unless next char is space or null
        if (message[i + 1] != ' ' && message[i + 1] != '\0')
            strcat(encoded, ",");
    }

    char saveFileName[100];
    printf("Enter the name of the file to save encoded message: ");
    fgets(saveFileName, sizeof(saveFileName), stdin);
    saveFileName[strcspn(saveFileName, "\n")] = '\0';

    FILE *out = fopen(saveFileName, "w");
    if (!out) {
        printf("Failed to save file.\n");
        return;
    }

    fprintf(out, "%s", encoded);
    fclose(out);
    printf("Encoded message saved.\n");
}

void cipherDecoder(FILE **currentFile, char *fileWords[5000], int *wordCount) {
    if (*currentFile == NULL || *wordCount == 0) {
        cipherKey(currentFile, fileWords, wordCount);
        if (*currentFile == NULL) {
            printf("Failed to load cipher key.\n");
            return;
        }
    }

    char buffer[100];
    char fileName[100];
    printf("Enter the name of the file containing encoded message: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(fileName, buffer);

    FILE *in = fopen(fileName, "r");
    if (!in) {
        printf("Failed to open encoded file.\n");
        return;
    }

    char encoded[10000];
    fgets(encoded, sizeof(encoded), in);
    fclose(in);

    int i = 0;
    while (encoded[i] != '\0') {
        if (encoded[i] == ' ') {
            printf(" "); // Preserve space
            i++;
        } else if (encoded[i] == '#') {
            printf("#"); // Preserve #
            i++;
        } else if (isdigit(encoded[i])) {
            // Parse word index
            int wordIndex = 0;
            while (isdigit(encoded[i])) {
                wordIndex = wordIndex * 10 + (encoded[i] - '0');
                i++;
            }

            // Skip comma
            if (encoded[i] == ',') i++;

            // Parse char index
            int charIndex = 0;
            while (isdigit(encoded[i])) {
                charIndex = charIndex * 10 + (encoded[i] - '0');
                i++;
            }

            // Output decoded character
            if (wordIndex >= 0 && wordIndex < *wordCount &&
                charIndex >= 0 && charIndex < strlen(fileWords[wordIndex])) {
                printf("%c", fileWords[wordIndex][charIndex]);
            } else {
                printf("#"); // Invalid fallback
            }
        } else {
            // Ignore anything else (newlines, etc.)
            i++;
        }
    }

    printf("\n");
}

void printMenu(){

    printf("1) Read in the name of a text file to use as a cipher key\n");
    printf("2) Create a cipher using the input text file (and save the result to a file)\n");
    printf("3) Decode an existing cipher (prompt user for a file to read containing the cipher text)\n");
    printf("4) Exit the program\n");
    printf("Enter here: ");

}

void printHeader(){


    printf("**********************************************************\n");
    printf("*  ██████╗   ███████╗   █████╗   ██╗       ███████╗         *\n");
    printf("*  ██╔══██╗  ██╔════╝  ██╔══██╗  ██║       ██╔════╝         *\n");
    printf("*  ██████╔╝  █████╗    ███████║  ██║       █████╗           *\n");
    printf("*  ██╔══██╗  ██╔══╝    ██╔══██║  ██║       ██╔══╝           *\n");
    printf("*  ██████╔   ███████╗  ██║  ██║  ███████╗  ███████╗         *\n");
    printf("*  ╚═════╝   ╚══════╝  ╚═╝  ╚═╝  ╚══════╝  ╚══════╝         *\n");
    printf("**********************************************************\n");
}