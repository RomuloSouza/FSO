#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int sudoku[9][9];
int validade_sudoku = 1;

void *valida_colunas (void *args);
void *valida_linhas (void *args);
void *valida_sub_sudokus (void *args);

pthread_mutex_t lock;

int main(){
    FILE *arquivo = NULL;
    pthread_t tid[11];

    if(pthread_mutex_init(&lock,NULL) != 0){
        fprintf(stderr, "Erro ao criar mutex.\n");
 		exit(EXIT_FAILURE);
    }

    char caminho[] = "0.txt";

    for (int i = 0; i < 10; i++){

        if (i == 9){
            printf("Arquivo = 10.txt");
            if(!(arquivo = fopen("10.txt","r"))){
                printf("Não foi possível abrir o arquivo");
                exit(EXIT_FAILURE);
            }
        }else {
            caminho[0] = (i+1) + '0';
            printf("Arquivo = %s\n", caminho);

            if(!(arquivo = fopen(caminho,"r"))){
                printf("Não foi possível abrir o arquivo");
                exit(EXIT_FAILURE);
            }
        }

        int l = 0;
        int c = 0;
        int a;
        while (fscanf(arquivo,"%d",&a) != EOF){

            sudoku[l][c] = a;
            c++;

            if(c > 8){
                c = 0;
                l++;
            }
        }

        printf("\nSudoku %d:\n", i+1  );

        for (int m = 0; m < 9; m++){
            for (int n = 0; n < 9; n++){
                printf("%d ",sudoku[m][n]);
            }
            printf("\n");
        }

        printf("Erros:\n");

        if(pthread_create(&tid[0],NULL,valida_colunas,NULL) != 0){
            printf("Erro na criação da nova thread.\n");
            exit(EXIT_FAILURE);
        }

        if(pthread_create(&tid[1],NULL,valida_linhas,NULL) != 0){
            printf("Erro na criação da nova thread.\n");
            exit(EXIT_FAILURE);
        }

        int aux = 0;
        int posicao[10];
        for(int j = 0; j < 25; j+=3){
            posicao[aux] = j;
            if(pthread_create(&tid[aux+2],NULL,valida_sub_sudokus,&posicao[aux]) != 0){
                printf("Erro na criação da nova thread.\n");
                exit(EXIT_FAILURE);
            }
            aux++;
        }

        for (int j = 0; j < 11; j++){
            if(pthread_join(tid[j],NULL) != 0){
                printf("Erro na finalização da nova thread.\n");
                exit(EXIT_FAILURE);
            }
        }

        if (validade_sudoku == 1){
            printf("\nConclusão: Sudoku Válido!\n");
        } else {
            printf("\nConclusão: Sudoku Inválido!\n");
        }
        printf("\n");

        fclose(arquivo);

    }

    pthread_mutex_destroy(&lock);

    return 0;
}

void *valida_colunas (void *args){


    for(int i = 0; i < 9; i++){
        int arr[10] = {0};

        for(int j = 0; j < 9; j++){
            arr[sudoku[j][i]]++;
            if(arr[sudoku[j][i]] > 1){
                pthread_mutex_lock(&lock);
                validade_sudoku = 0;

                printf("- Inválido por valida_colunas\n");
                printf("    Coluna da posição %d\n", j);

                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        }
    }

    pthread_exit(NULL);
}

void *valida_linhas (void *args){

    for(int i = 0; i < 9; i++){
        int arr[10] = {0};

        for(int j = 0; j < 9; j++){
            arr[sudoku[i][j]]++;
            if(arr[sudoku[i][j]] > 1){
                pthread_mutex_lock(&lock);
                validade_sudoku = 0;
                printf("- Inválido por valida_linhas\n");
                printf("    Linha da posição %d\n", i);
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        }
    }

    pthread_exit(NULL);
}

void *valida_sub_sudokus (void *args){
    int col = *(int*)args;
    int linha = 0;
    int arr[10] = {0};

    // printf("col antes = %d\n", col);

    if(col >= 18){
        col -= 18;
        linha = 6;
    } else if (col >=9){
        col -= 9;
        linha = 3;
    }

    // printf("col depois = %d\n", col);
    int col_inicio = col;
    int linha_inicio = linha;

    for( int i = linha; i < linha+3; i++){
        for (int j = col; j < col+3; j++){
            arr[sudoku[i][j]]++;
            // printf("linhas = %d col = %d\n",linha,col);
            if(arr[sudoku[i][j]] > 1){
                pthread_mutex_lock(&lock);
                validade_sudoku = 0;

                printf("- Inválido por valida_sub_sudokus\n");
                printf("    sub sudoku começa em (%d,%d) e termina em (%d,%d)\n",
                linha_inicio,col_inicio, linha_inicio+2, col_inicio+2);
                // printf("linhas = %d col = %d\n",i,j);

                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        }
    }

    pthread_exit(NULL);
}