#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <jansson.h>
#include <unistd.h>

void renameFile(const char *oldName, const char *newName) {
    char command[200];
    sprintf(command, "mv %s %s", oldName, newName);
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <install/get> <mod name>\n", argv[0]);
        return 1;
    }

    char *commandType = argv[1];
    char *modName = argv[2];

    // Set the destination directory based on the command type
    char destinationDir[200];
    if (strcmp(commandType, "install") == 0) {
        sprintf(destinationDir, "%s/.minecraft/mods", getenv("HOME"));
    } else if (strcmp(commandType, "get") == 0) {
        sprintf(destinationDir, "%s/downloads/mods", getenv("HOME"));
    } else {
        printf("Invalid command type. Please use 'install' or 'get'.\n");
        return 1;
    }

    // Create the destination directory if it doesn't exist
    struct stat st;
    if (stat(destinationDir, &st) == -1) {
        mkdir(destinationDir, 0700);
    }

    // Open the "mods" directory
    DIR *dir = opendir("mods");
    if (dir == NULL) {
        printf("Failed to open the 'mods' directory.\n");
        return 1;
    }

    // Iterate through the files in the directory
    struct dirent *entry;
    int found = 0;
    while ((entry = readdir(dir)) != NULL) {
        char filePath[200];
        sprintf(filePath, "mods/%s", entry->d_name);

        struct stat fileStat;
        if (stat(filePath, &fileStat) == -1) {
            continue;
        }

        if (S_ISDIR(fileStat.st_mode) && strcmp(entry->d_name, modName) == 0) {
            found = 1;

            // Open the mod folder
            DIR *modDir = opendir(filePath);
            if (modDir == NULL) {
                printf("Failed to open the mod folder '%s'.\n", entry->d_name);
                closedir(dir);
                return 1;
            }

            // Iterate through the files in the mod folder
            struct dirent *modEntry;
            int metadataFound = 0;
            while ((modEntry = readdir(modDir)) != NULL) {
                if (strcmp(modEntry->d_name, "metadata.json") == 0) {
                    metadataFound = 1;
                    break;
                }
            }

            // Close the mod folder
            closedir(modDir);

            if (metadataFound) {
                // Read and parse the "metadata.json" file
                char metadataPath[200];
                sprintf(metadataPath, "%s/metadata.json", filePath);

                FILE *metadataFile = fopen(metadataPath, "r");
                if (metadataFile == NULL) {
                    printf("Failed to open the 'metadata.json' file in the mod folder.\n");
                    closedir(dir);
                    return 1;
                }

                fseek(metadataFile, 0, SEEK_END);
                long fileSize = ftell(metadataFile);
                fseek(metadataFile, 0, SEEK_SET);

                char *jsonData = (char *)malloc(fileSize + 1);
                fread(jsonData, 1, fileSize, metadataFile);
                jsonData[fileSize] = '\0';

                fclose(metadataFile);

                // Parse the JSON data
                json_error_t error;
                json_t *root = json_loads(jsonData, 0, &error);
                free(jsonData);

                if (root == NULL) {
                    printf("Failed to parse the 'metadata.json' file: %s\n", error.text);
                    closedir(dir);
                    return 1;
                }

                // Extract the value associated with the "url" key
                json_t *urlValue = json_object_get(root, "url");
                if (json_is_string(urlValue)) {
                    const char *url = json_string_value(urlValue);
                    printf("URL: %s\n", url);

                    // Run wget command
                    char command[200];
                    sprintf(command, "wget -P %s %s", destinationDir, url);
                    int status = system(command);

                    if (status != 0) {
                        printf("Failed to run wget command.\n");
                    } else {
                        // Rename the downloaded file
                        char oldName[200];
                        sprintf(oldName, "%s/%s", destinationDir, strrchr(url, '/') + 1);

                        char newName[200];
                        sprintf(newName, "%s/%s", destinationDir, modName);

                        renameFile(oldName, newName);
                    }
                } else {
                    printf("Failed to extract the URL from the 'metadata.json' file.\n");
                }

                json_decref(root);
            } else {
                printf("Mod folder '%s' found in the 'mods' directory, but 'metadata.json' file is missing.\n", entry->d_name);
            }

            break;
        }
    }

    // Close the directory
    closedir(dir);

    // Check if the mod folder was found
    if (!found) {
        printf("Mod folder '%s' not found in the 'mods' directory.\n", modName);
    }

    return 0;
}
