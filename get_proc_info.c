#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Berke Udunman 270201046
// Sezin Cagla Serficeli 280201041

void printError(const char *message) {
    printf("Error: %s\n", message);
    exit(1);
}

void insertKernelModule(const char *argType, const char *argValue) {
    char command[256];
    if (strcmp(argType, "-pid") == 0) {
        snprintf(command, sizeof(command), "sudo insmod proc_info_module.ko pid=%s", argValue);
    } else {
        snprintf(command, sizeof(command), "sudo insmod proc_info_module.ko pname=\"%s\"", argValue);
    }

    if (system(command) != 0) {
        printError("Failed to insert kernel module");
    }
}

void removeKernelModule() {
    if (system("sudo rmmod proc_info_module") != 0) {
        printError("Failed to remove kernel module");
    }
}

void readProcInfoModule() {
    FILE *kernelModule;
    char line[1035];

    kernelModule = fopen("/proc/proc_info_module", "r");
    if (kernelModule == NULL) {
        printError("Failed to open /proc/proc_info_module");
    }

    // Reading
    while (fgets(line, sizeof(line), kernelModule) != NULL) {
        printf("%s", line);
    }

    fclose(kernelModule);
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
	printf("Valid Usage of Program is ->\nGive an argument like: -pid `ProcessId` or -pname `ProcessName`\n");
        printError("Incorrect number of arguments");
    }

    if (strcmp(argv[1], "-pid") != 0 && strcmp(argv[1], "-pname") != 0) {
	printf("Valid Usage of Program is ->\nGive an argument like: -pid `ProcessId` or -pname `ProcessName`\n");
        printError("Invalid argument type");
    }


    insertKernelModule(argv[1], argv[2]);
    readProcInfoModule();
    removeKernelModule();

    return 0;
}

