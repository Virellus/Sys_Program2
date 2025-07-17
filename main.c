/*
Student Name: Wallace Switzer
Student NetID: wjs291
*/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]){   // argc is the count of cli arguments, argv arrary of strings containing the arguments
    //verify
    if (argc != 5){
        //printf("Error incorrect number of arguments!"); //checking if the user gave the worng num of inputs
        write(STDOUT_FILENO, "Error incorrect number of arguments!\n", 38);
        exit(1);
    }

    // delaring time and startign it
    time_t start_time;              // stores time
    time(&start_time);              // stores Epoch secounds in time_t veriable
    struct tm *timeinfo;                // pointer that points to date/time structure
    timeinfo = localtime(&start_time);  // take timestamp and makes it readable

    // declaring variables
    char inBuf[BUF_SIZE];           // stores the raw data file in a box 
    char outBuf[BUF_SIZE];          // stores prepared output in a box
    char line[BUF_SIZE];            // puts the indv lines from the inBuff so they can be processed
    char timeString[BUF_SIZE];      // length for time
    int readLen;                    // used later but will store how mnay bytes were read
    int offset = 0;                 // used to recall where in file the program is 
    int fizzCount = 0;              // used to count fizz occurances
    int buzzCount = 0;              // used to count buzz occurances

    //intitalize the arguments
    // the nmae of program will be first
    char *inFile = argv[1];         // the random number file
    char *outFile = argv[2];        // results file
    int fizzNum;                    // first number 
    int buzzNum;                    // secound number

    if (sscanf(argv[3], "%d", &fizzNum) != 1){
        //printf("Error invalid fizz number argument\n");
        write(STDOUT_FILENO, "Error invalid fizz number of arguments\n", 40);
        exit(1);
    }
    if (sscanf(argv[4], "%d", &buzzNum) != 1){
        //printf("Error invalid buzz number argument\n");
        write(STDOUT_FILENO, "Error invalid buzz number of arguments\n", 40);
        exit(1);
    }
    // attempts to open file
    int fd_inFile = open(inFile, O_RDONLY);
    if (fd_inFile == -1) {                      // making sure opens 
        perror("Error opening input file");
        exit(1);
    }
    int fd_outFile = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd_outFile == -1) {                     // making sure opens 
        perror("Error opening output file");
        exit(1);
    }

    // making a header for time stamp
    strftime(timeString, BUF_SIZE, "%a %b %d %H:%M:%S %Y", timeinfo);   // formating the string
    int headerLen = snprintf(outBuf, BUF_SIZE, "%s\n", timeString);     // puting string into the output buffer
    headerLen += snprintf(outBuf + headerLen, BUF_SIZE - headerLen, "----------------------\n"); // formating line
    
    if (write(fd_outFile, outBuf, headerLen) == -1){    // writes the header and if fails returns error
        perror("Error writing header to output file"); 
        close(fd_inFile);
        close(fd_outFile);
        exit(1);
    }
    //get time beofre the mass amount of looping starts
    clock_t fizzbuzzStartTime = clock();
    //Process 
    while ((readLen = pread(fd_inFile, inBuf, BUF_SIZE, offset)) > 0){   //loops through the file until over
        // starts at 0 and reads until line is over or buffer is full
        //returns number of bytes actually read
        // find new line char in buffer

        if (readLen > BUF_SIZE){
            //printf("Error read lengthn larger than buffer size\n");
            write(STDOUT_FILENO, "Error read length larger than buffer size\n", 42);
            close(fd_inFile);
            close(fd_outFile);
            exit(1);
        }
        int lineLen = 0;
        for (int i = 0; i < readLen; i ++){ // loops through charcters in the buffer to find end of the line
            if (inBuf[i] == '\n') {
                lineLen = i; 
                break;
            }
        }
        // for the last line, use rest of buffer
        if (lineLen == 0){
            lineLen = readLen;
        }
        // copy the line to the line buffer and terminate it 
        strncpy(line, inBuf, lineLen);
        line[lineLen] = '\0';
        // convert strings to int
        int number;
        if (sscanf(line, "%d", &number) != 1){
            //printf("Error invalid number from input file\n");
            write(STDOUT_FILENO, "Error invalid number from input file\n", 37);
            close(fd_inFile);
            close(fd_outFile);
            exit(1);
        }
        // creating a output starting with the number
        int outLen = snprintf(outBuf, BUF_SIZE, "%d ", number);
        // fizz check
        if (number % fizzNum == 0){
            outLen += snprintf(outBuf + outLen, BUF_SIZE - outLen, "Fizz");
            fizzCount++;
        }
        if (number % buzzNum == 0){
            outLen += snprintf(outBuf + outLen, BUF_SIZE - outLen, "Buzz");
            buzzCount++;
        }
        // adding new line to end of output
        outLen += snprintf(outBuf + outLen, BUF_SIZE - outLen, "\n");
        // writing results to outfile
        //write(fd_outFile, outBuf, outLen);

        if (write(fd_outFile, outBuf, outLen) == -1){
            perror("Error writing to output file");
            close(fd_inFile);
            close(fd_outFile);
            exit(1);
        }
        
        offset += lineLen + 1;
    }
    // getting end time
    clock_t fizzbuzzEndTime = clock();
    if (readLen == -1){
        perror("Error reading from the input file");
        close(fd_inFile);
        close(fd_outFile);
        exit(1);
    }
    // getting cpu time 
    clock_t ticks_used = fizzbuzzEndTime - fizzbuzzStartTime;   // gets ticks
    double runTime = (double)ticks_used / CLOCKS_PER_SEC;       // clock ticks to persise sec
    long cpuRunTime = (long)(runTime * 1000);                   // sec to mili sec and store as whoel number
    // footer with stats
    int footerLen = snprintf(outBuf, BUF_SIZE, "----------------------\n");                             // seperator
    footerLen += snprintf(outBuf + footerLen, BUF_SIZE - footerLen, "Fizz Count: %d\n", fizzCount);     // adds fizz count to buffer   
    footerLen += snprintf(outBuf + footerLen, BUF_SIZE - footerLen, "Buzz Count: %d\n", buzzCount);     // adds buzz count to buffer
    footerLen += snprintf(outBuf + footerLen, BUF_SIZE - footerLen, "CPU Time(ms): %ld\n", cpuRunTime); // adds cpu time to buffer
    if (write(fd_outFile, outBuf, footerLen) == -1){    // writes the footer to output file
        perror("Error writing footer to output file");
        close(fd_inFile);
        close(fd_outFile);
        exit(1);
    }
    close(fd_inFile);
    close(fd_outFile);
    exit(0);
}