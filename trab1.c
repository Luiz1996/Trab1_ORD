/*
Primeiro trabalho da disciplina de ORD 2018

ALUNO: LUIZ FLÁVIO PEREIRA      RA91706
ALUNO: GUSTAVO GRAVENA BATILANI RA88501
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *p_escrita = NULL;
int cabeca = -1, imprimeBusca = 0, removeReg = 0;
char* prompt[] = {
    "  COD.INCRICAO: ",
    "  NOME........: ",
    "  CURSO.......: ",
    "  SCORE.......: ",
};

int read_registry_tam_bytes(FILE *p_leitura, char buffer[]){
	int Tam = 0;
    
	if(fread(&Tam, sizeof(Tam), 1, p_leitura) == 0){
		return 0;
	}
	fread(buffer, Tam, 1, p_leitura);
   	buffer[Tam] = '\0';
  	return Tam;
}

int showMenu(){
	int opcao;
    
	printf("*-------------------------------------------------------- MENU --------------------------------------------------------*");
    printf("[1] Importar arquivo.\n[2] Inserir registro.\n[3] Remover registro.\n[4] Buscar registro.\n[0] Sair.\n\n");
    printf("Opcao: ");
	scanf("%d", &opcao);

    return opcao;
}

int read_registry(FILE *p_leitura, char campo[], int cont){
    int i = 0;
    char caracter;
    
	caracter = fgetc(p_leitura);
    while(caracter != EOF){
        campo[i] = caracter;
		if(caracter == '|'){
        	cont++;
        	if(cont == 4){
        		campo[i] = '|';
        		return (i+1);
				break;
			}
		}
        i++;
        caracter = fgetc(p_leitura);
    }
}

void import(){
	FILE *p_leitura;
	char nome_arquivo[20], campo[256];
	char *buffer, *nome_arq;
	int tam = 0;
	fflush(stdin); 
	
	printf("Digite o nome do arquivo a ser importado: "); 
    fgets(nome_arquivo, 20, stdin);
    nome_arquivo[strlen(nome_arquivo) - 1] = '.';
    nome_arq = strtok(nome_arquivo, ".");
	strcat(nome_arquivo, ".txt"); //fazendo dessa forma não há necessidade de digitar o ".txt"

    p_leitura = fopen(nome_arq,"r");
    if(!p_leitura){
    	printf("\nERROR 404 NOT FOUND\n\nPROGRAMA ABORTADO !!!\n");
    	exit(0);
    }
    
    printf("\nOla, estamos criando o arquivo 'registros.txt', por favor, aguarde...\n");
    sleep(3);//coloquei um delay de 3 segundos...
    p_escrita = fopen("registros.txt", "wb+"); 
    fwrite(&cabeca, sizeof(cabeca), 1, p_escrita);
	tam = read_registry(p_leitura, campo, 0);
    while (tam > 0){
		fwrite(&tam, sizeof(tam), 1, p_escrita);
    	fwrite(campo, tam, 1, p_escrita);
		tam = read_registry(p_leitura, campo, 0);
    }
    printf("\nO arquivo 'registros.txt' foi criado com exito!\n");
	fclose(p_leitura);
    fclose(p_escrita);
}

int check_reg(){
	FILE *p_leitura = fopen("registros.txt","r");
	if(!p_leitura){ 
    	printf("O arquivo 'dados-inline.txt' ainda nao foi importado!\n");
    	sleep(1); //delay de 1 segundo...
		printf("\nImporte-o e tente novamente mais tarde...\n");
		fclose(p_leitura);
    	return 0;
    }
    fclose(p_leitura);
    return 1;
}

void remove_reg(char *cod_insc, FILE *p_leitura, int lenght, int led_cabeca){
	int byte_offset;
	
	//led apos remover
	fseek(p_leitura, (- lenght - 4), SEEK_CUR);
    byte_offset = ftell(p_leitura);
    fseek(p_leitura, 4, SEEK_CUR);
	fprintf(p_leitura, "*|");
	fprintf(p_leitura, "%d|", led_cabeca);
    fseek(p_leitura, 0, SEEK_SET);
    fwrite(&byte_offset, sizeof(byte_offset), 1, p_leitura);
	
	printf("O registro com inscricao: %s, foi removido com SUCESSO.\n", cod_insc);
	printf("ByteOffset do registro %s e: %d.\n", cod_insc, byte_offset);
	printf("Cabeca Led ATUAL: %d.\n", byte_offset);
	fclose(p_leitura);
}

void search_reg(FILE *p_leitura, char cod_insc[]){
	char *field, buffer[256];
	int verifica = 0, led_cabeca, lenght = read_registry_tam_bytes(p_leitura, buffer);
    
	if(removeReg == 1){
		//led antes de remover
		fseek(p_leitura, 0, SEEK_SET);
		fread(&led_cabeca, sizeof(int), 1, p_leitura);
		printf("Cabeca Led: %d.\n", led_cabeca);
		fseek(p_leitura, 4, SEEK_SET);
	}
	
	while(lenght > 0){
		field = strtok(buffer, "|");
    	
    	if(strcmp(field, cod_insc) == 0){
    		//remove o registro solicitado.
			if(removeReg == 1){
    			remove_reg(cod_insc, p_leitura, lenght, led_cabeca);
				removeReg = 0;
				return;
			}
			//imprime os dados buscados.
			if(imprimeBusca == 1){
    			int i = 0;
    			verifica = 1;
    			printf("%s", prompt[i]);
            	printf("%s \n", field);
            	i++;
				field = strtok(NULL, "|");
			
				while(field != NULL){
 					printf("%s", prompt[i]);
            		printf("%s \n", field);
					field = strtok(NULL, "|");
    				i++;
    			}
    			imprimeBusca = 0;
    			return;
			}
    	}
		lenght = read_registry_tam_bytes(p_leitura, buffer);
	}
	printf("REGISTRO NAO ENCONTRADO...\n");
	fclose(p_leitura);
}

void get_insc(){
	char insc[256];
	
	printf("Insira o codigo de incricao: ");
	fflush(stdin);
    fgets(insc, 256, stdin);
   	insc[strlen(insc)-1]='\0';
	
	int i = 0;
	FILE *p_leitura = fopen("registros.txt","r+");
	fseek(p_leitura, 4, SEEK_SET); 
    if(p_leitura == NULL){
    	printf("Nao foi possivel abrir o arquivo 'registros.txt', tente novamente mais tarde.");
    	return;
    }
	for(i = 0; i < 120; i++) printf("-");
	search_reg(p_leitura, insc);
}

void get_new_reg(char *buffer){
	char temp[100];
	int i;
	
	for(i = 0; i < 4; i++){
 		printf("%s", prompt[i]);
      	fflush(stdin);
    	fgets(temp, 100, stdin);
    	temp[strlen(temp)-1]='\0';
    	strcat(buffer, temp);
    	strcat(buffer, "|");
    }
}

void make_insertion(){
	p_escrita = fopen("registros.txt", "r+");
	fseek(p_escrita, 4, SEEK_SET);
    char caracter, buffer[256];
    buffer[0] = '\0';
    
	caracter = fgetc(p_escrita);
    while(caracter != EOF){
 		caracter = fgetc(p_escrita);
		if(caracter == EOF){
			get_new_reg(buffer);	
			int Tam = strlen(buffer);
    		fwrite(&Tam, sizeof(Tam), 1, p_escrita);
    		fwrite(buffer, Tam, 1, p_escrita);
		}
    }
    fclose(p_escrita);
}

void main(void){
	int opcao;
    
    opcao = showMenu(opcao);    
	system("cls");
    while(opcao != 0){
        switch(opcao){
            case 1 : {
                import();
                break;
            }
            case 2 : {
                int ver = check_reg();
                if(ver == 1)
                	make_insertion();
                break;
            }
            case 3 : {
                int ver = check_reg();
                if(ver == 1){
					removeReg = 1;
					get_insc();
				}
				break;
            }
            case 4 : {
                int ver = check_reg();
                if(ver == 1){
					imprimeBusca = 1;
					get_insc();
				}
				break;
            }
            default :{
            	system("cls");
				printf("Opcao invalida, tente novamente.\n");
				break;
			}
        }
        opcao = showMenu();
        system("cls");
    }
}
