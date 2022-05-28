
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#define N_CLIENTES 10
#define N_CADEIRAS 5

//Zona Crítica
sem_t cadeiras_usadas;        
sem_t cadeira_do_babeiro;    
sem_t cabelo_cortado;       
sem_t cliente_na_cadeira; 

void *barbeiro(){
    sleep(1); // espera um pouco antes de ir dormir

    while(1){
        if(sem_trywait(&cadeira_do_babeiro) == 0){
            printf("Barbeiro dormiu na cadeira \n");
            //sem_wait(&cadeira_do_babeiro);
        }else{
            sem_wait(&cliente_na_cadeira);
            printf("Barbeira cortou cabelo de um Cliente \n");
            sem_post(&cabelo_cortado); //sinaliza que terminou de corta o cabelo
        }
    }
    return NULL;
}

void *cliente(void *v){

    int id = *(int *)v;
    sleep(id % 3); // faz ele espera um pouco ante de tentar entrar

    if(sem_trywait(&cadeiras_usadas) == 0){
        printf("Cliente %d entrou na barbearia.\n", id);
        sem_wait(&cadeira_do_babeiro); // clinte bloqueia a cadeira do barbeiro
        printf("Cliente %d sentou na cadeira do barbeiro.\n", id);
        sem_post(&cliente_na_cadeira); // libera a cadeira da barbearia
        sem_post(&cadeiras_usadas);    // libera um vaga na recepção
        sem_wait(&cabelo_cortado);     // espera o barbeiro corta o cabelo
        sem_post(&cadeira_do_babeiro); // lebera a cadeira do barbeiro
        printf("Cliente %d deixou a barbearia.\n", id);
    }else
        printf("Cliente %d não entrou na barbearia.\n", id);
    return NULL;
    
}


int main(){
    pthread_t thr_clientes[N_CLIENTES], thr_barbeiro;
    int i, id[N_CLIENTES];

    
    // inicia os semaforos
    sem_init(&cadeiras_usadas, 0, 5);
    sem_init(&cadeira_do_babeiro, 0, 1);
    sem_init(&cliente_na_cadeira, 0, 0);
    sem_init(&cabelo_cortado, 0, 0);

    //Cria as threads dos clientes
    for (i = 0; i < N_CLIENTES; i++)
    {
        id[i] = i;
        pthread_create(&thr_clientes[i], NULL, cliente, (void *)&id[i]); // id dos clientes
    }

    pthread_create(&thr_barbeiro, NULL, barbeiro, NULL);

    for (i = 0; i < N_CLIENTES; i++)
        pthread_join(thr_clientes[i], NULL);

    
    return 0;
}