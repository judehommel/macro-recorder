#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>

void initFiles(FILE **mouse, FILE **keyboard) {
    FILE *mouseEventNumFile = fopen("/proc/bus/input/devices", "r");

    char line[100];
    char *mouseEventNum;
    while (fgets(line, sizeof(line), mouseEventNumFile)) {
        if (strstr(line, "mouse")) {
            mouseEventNum = strstr(line, "event") + 5;
            for (int i=0; i<(int)strlen(mouseEventNum); i++) {
                if (mouseEventNum[i] == ' ') {
                    mouseEventNum[i] = '\0';
                    break;
                }
            }
            break;
        }
    }

    char *mousePathBase = "/dev/input/event";

    int mouseEventNumLen = strlen(mouseEventNum);
    int mousePathBaseLen = strlen(mousePathBase);
    int mousePathLen = mouseEventNumLen + mousePathBaseLen;

    char *mousePath = (char*)malloc(sizeof(char) * mousePathLen);
    for (int i=0; i<mousePathLen; i++) {
        if (i<mousePathBaseLen) {
            mousePath[i] = mousePathBase[i];
        } else {
            mousePath[i] = mouseEventNum[i-mousePathBaseLen];
        }
    }

    *mouse = fopen(mousePath, "rb+");
    *keyboard = fopen("/dev/input/event4", "rb+");

    fclose(mouseEventNumFile);
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0); // disable buffering
    FILE *mouseFile, *keyboardFile;
    initFiles(&mouseFile, &keyboardFile);
    if (!mouseFile) {
        perror("Failed to open mouse device");
        return 1;
    }
    if (!keyboardFile) {
        perror("Failed to open keyboard device");
        return 1;
    }

    struct input_event *keyboard = malloc(sizeof(struct input_event));
    int isRecording = 0;
    int isAltHeld = 0;
    while (1) {
        fread(keyboard, sizeof(struct input_event), 1, keyboardFile);
        if (keyboard->type == EV_KEY && keyboard->code == KEY_LEFTALT && (keyboard->value == 1 || keyboard->value == 2)) {
            isAltHeld = 1;
        } else if (keyboard->type == EV_KEY && keyboard->code == KEY_LEFTALT && keyboard->value == 0) {
            isAltHeld = 0;
        }

        if (!isRecording && keyboard->type == EV_KEY && keyboard->code == KEY_R && keyboard->value == 1) {
            isRecording = 1;
        } else if (isRecording && keyboard->type == EV_KEY && keyboard->code == KEY_R && keyboard->value == 1) {
            isRecording = 0;
        }

        if (isRecording) {
            printf("recording");
            //printf("code: %d, time: %d, type: %d, value: %d\n", mouseData->code, mouseData->time, mouseData->type, mouseData->value);
        }
    }

    fclose(mouseFile);
    fclose(keyboardFile);
    return 0;
}
