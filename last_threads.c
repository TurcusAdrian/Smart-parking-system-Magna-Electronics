#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <mysql.h>

#define BUFFER_SIZE 256

// gcc -Wall -o p last_threads.c -lwiringPi -I/usr/include/mysql -lmysqlclient


typedef struct message_t {
    char* message;
    struct message_t* next;
} message_t;

message_t* send_stack = NULL;
message_t* receive_list = NULL;
pthread_mutex_t mutex;

void push(message_t** head, char* message) {
    message_t* new_node = (message_t*) malloc(sizeof(message_t));
    new_node->message = strdup(message);                                                             ;
    new_node->next = NULL;
    
    if (*head == NULL) {
        *head = new_node;
    } else {
        message_t* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    
}

char* pop(message_t** head) {
    
    if (*head == NULL) {
        
        return NULL;
    }
    char* message = (*head)->message;
    message_t* temp = *head;
    *head = (*head)->next;
    free(temp);
    
    return message;
}

int check_license_plate_in_db(const char* license_plate) {
    MYSQL *con = mysql_init(NULL);
    if (con == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 0;
    }

    if (mysql_real_connect(con, "localhost", "user", "user",
          "BazaTest", 0, NULL, 0) == NULL)
  {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(con);
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM cars WHERE license_number='%s'", license_plate);

    if (mysql_query(con, query)) {
        fprintf(stderr, "SELECT COUNT(*) failed. Error: %s\n", mysql_error(con));
        mysql_close(con);
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        fprintf(stderr, "mysql_store_result() failed. Error: %s\n", mysql_error(con));
        mysql_close(con);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = atoi(row[0]);

    mysql_free_result(result);
    mysql_close(con);

    return count > 0;
}

void* send_thread_func(void* arg) {
    int arduino_fd = *(int*) arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        char* message = pop(&send_stack);
        pthread_mutex_unlock(&mutex);
        if (message != NULL) {
            serialPuts(arduino_fd, message);
            serialPutchar(arduino_fd, '\n');
            printf("Sent to Arduino: %s\n", message);
            free(message);
        }
      
    }
    return NULL;
}

void* receive_thread_func(void* arg) {
    int arduino_fd = *(int*) arg;
    char buffer[BUFFER_SIZE];
    int index = 0;
    while (1) {
        if (serialDataAvail(arduino_fd)) {
            char ch = serialGetchar(arduino_fd);
            if (ch == '\n') {
                buffer[index] = '\0';
                printf("Received from Arduino: %s\n", buffer);
                index = 0;
 
                // Run Python script based on the type of detection
                    if (strncmp(buffer, "Car detected at entrance\r",24) == 0 || strcmp(buffer, "Car detected at exit\r") == 0) {	
                      
                    char command[512];
                    sprintf(command, "python3 procesare_imagine.py");
                    FILE* pipe = popen(command, "r");
                    if (pipe == NULL) {
                        printf("Failed to run command\n");
                    } else                    {
                        char result[BUFFER_SIZE];
                        if (fgets(result, sizeof(result), pipe) != NULL) {
                        
                            printf("Validation Result: %s\n", result);
                            if (strncmp(result, "Valid", 5) == 0) {
                                // Extract license plate from the result if necessary
                                char license_plate[BUFFER_SIZE];
                                int j=0;
                                for(int i=6;i<strlen(result)-1;i++){
                                    if(result[i]==' '){
                                        continue;
                                    }
                                    license_plate[j]=result[i];
                                    j++;
                                }
                                
                                    printf("%s\n",license_plate);
                                // Check \the license plate in the database
                                if (check_license_plate_in_db(license_plate)) {
                                    
                                    if (strncmp(buffer, "Car detected at entrance\r",24) == 0) {
                                        pthread_mutex_lock(&mutex);
                                        push(&send_stack, "Open entrance barrier");
                                        pthread_mutex_unlock(&mutex);
                                    } else if (strcmp(buffer, "Car detected at exit\r") == 0) {
                                        pthread_mutex_lock(&mutex);
                                        push(&send_stack, "Open exit barrier");
                                        pthread_mutex_unlock(&mutex);
                                    }
                                } else {
                                    printf("License plate not found in database\n");
                                }
                            }
                        }
                        pclose(pipe);
                    }
                }
            } else {
                if (index < BUFFER_SIZE - 1) {
                    buffer[index++] = ch;
                }
            }
        }
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);          
    int arduino_fd = serialOpen("/dev/ttyUSB0", 9600);
    if (arduino_fd < 0) {
        fprintf(stderr, "Failed to open serial port\n");
        return 1;
    }

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_thread_func, (void*)&arduino_fd);
    pthread_create(&receive_thread, NULL, receive_thread_func, (void*)&arduino_fd);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    pthread_mutex_destroy(&mutex);
    serialClose(arduino_fd);

    return 0;
}

