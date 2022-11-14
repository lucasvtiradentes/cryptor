#include <stdio.h>   /* printf, sprintf */
#include <stdlib.h>  /* system, fgetc, fclose, fopen, fprintf, feof, remove */
#include <string.h>  /* strlen, strcmpi, strcmp */
#include <conio.h>   /* getch */
#include <locale.h>  /* setlocale */
#include <windows.h> /* SetConsoleTextAttribute, SetConsoleTitle */
#include <ctype.h>   /* isalnum, isspace */

#define limpa_tela() system("cls");
#define msg_simples(A) printf("%s", A);
#define pause() system("pause>nul");
#define sair() exit(0);

////////////////////////////////////////////////////////////////////////////////////////////////////

int num_senha = 0, caractere_n = 0, var_int, tem_senha, pri_vez, quer_senha;
char este_arquivo[100], nome[100], arquivo_entrada[100], extensao_encriptada[10], senha_arquivo[20];
char var_tmp[20], var_tmp2[20], arquivo_saida[100], extensao[10], extensao_saida[10], senha[100];
char caractere;
FILE *entrada, *saida;
HANDLE hStdout;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
WORD wOldColorAttrs;

/////////////////////////////////////////////////////////////////////////////////////////////////////

int animacao(char *frase, int cor) { // função: fazer uma animação com as letras
  int n, n_caracteres = strlen(frase);

  if (cor == 02) {
    SetConsoleTextAttribute(hStdout, 0x02);
  } else if (cor == 03) {
    SetConsoleTextAttribute(hStdout, 0x03);
  } else if (cor == 04) {
    SetConsoleTextAttribute(hStdout, 0x04);
  } else if (cor == 00) {
    SetConsoleTextAttribute(hStdout, wOldColorAttrs);
  } // padrao

  for (n = 0; n < n_caracteres; n++) {
    printf("%c", frase[n]);
    Sleep(60);
  }

  SetConsoleTextAttribute(hStdout, wOldColorAttrs); // isso aqui reseta a cor da caixa do console
}

int ajuda(char erro[], char d_erro[])
{ // Função: mostra a mensagem de erro e encerrar o programa
  limpa_tela();
  msg_simples("Erro ocorrido: ");
  animacao(erro, 04);
  msg_simples("\nDescrição do erro: ");
  animacao(d_erro, 04);
  msg_simples("\n\nModo de uso: ");
  animacao(este_arquivo, 02);
  animacao(" \"Arquivo\" \n", 02);
  pause();
  sair();
}

int define_senha() { // Função: Permite que o usuario digite uma senha
  limpa_tela();
  printf("Digite a sua senha (alfanumarica, apenas): ");
  do {                      // "faaa isso enquanto o usuario nao pressionar o enter"
    caractere = getch(); // obtam a tecla pressionada pelo usuario
    if ((isalnum(caractere) || isspace(caractere)) && caractere != 13) { // se o caractere for alfanumarico ou espaao
      printf("*");
      if (num_senha = 0) { // se for o primeiro digito da senha...
        sprintf(senha, "%c", ~caractere);
      } else { // se ja tiver um digito na senha...
        sprintf(senha, "%s%c", senha, ~caractere);
      }
      num_senha++; // equivale a num_senha = num_senha + 1
    } else if (caractere == 8 && num_senha > 0) { // se a tecla  for o "backspace" e a senha tiver mais de um digito...
      sprintf(senha, "%s%c", senha, ~'\b \b');
      printf("\b \b");
      num_senha--; // equivale a num_senha = num_senha - 1
    } else if (caractere == 13) { // se a tecla for "enter"...
      printf("\n");
    }
  } while (caractere != 13);
}

int protecao_senha() { // Função: Pergunta que o usuario gostaria de adcionar senha ao arquivo a ser encriptado

  while (1)  {
    limpa_tela();
    msg_simples("Voca gostaria de adcionar senha ao arquivo?\n\n");
    msg_simples("[01] Sim, eu gostaria\n");
    msg_simples("[02] Nao, nao a necessario\n");
    var_int = getch();
    if (var_int == 49)  { // se o usuario pressionar a tecla "1"...
      define_senha();
      quer_senha = 1; // sim
      break;
    }  else if (var_int == 50) {                 // se o usuario pressionar a tecla "2"...
      quer_senha = 2; // nao quer
      break;
    }
  }
}

char *acha_extensao(int mode_t) { // Função: Achar a extensao do arquivo
  char var_tmp[20];
  if (mode_t == 1)  { // Se o modo especificado for a "Codificação"...
    sprintf(var_tmp, "%s", "hme");
  } else if (mode_t == 2)  { // Se o modo especificado for a "Decodificação"...
    for (caractere_n = 0; caractere_n < 5; caractere_n++)    {
      caractere = fgetc(entrada);
      if (caractere != ~']') {
        if (caractere_n == 0) {
          sprintf(var_tmp, "%c", ~caractere);
        } else {
          sprintf(var_tmp, "%s%c", var_tmp, ~caractere);
        }
      } else {
        caractere_n += 2;
        break;
      }
    }
  }
  return (var_tmp);
}

char *acha_nome(char *arg1, int cond) { // Função: Achar o nome e a extensao do arquivo e defini-los em variaveis globais
  char nomedoarquivo[200];
  char *pch, *pch1;
  pch = strtok(arg1, "\\");
  while (pch != NULL)  {
    pch = strtok(NULL, "\\");
    if (pch != NULL) {
      strcpy(nomedoarquivo, pch);
    }
  }

  if (cond == 1) {
    char extensao_f[10];
    int vez = 1;
    pch1 = strtok(nomedoarquivo, ".");
    while (pch1 != NULL) {
      if (vez == 1) {
        strcpy(nome, pch1);
      } else if (vez == 2) {
        strcpy(extensao_f, pch1);
      }
      pch1 = strtok(NULL, ".");
      vez++;
    }
    sprintf(nome, "%s", nome);
    sprintf(extensao, "%s", extensao_f);
    sprintf(nomedoarquivo, "%s.%s", nome, extensao_f);
  }

  return (nomedoarquivo);
}

int decodifica() { // Função: Decodificar o arquivo
  if ((saida = fopen(arquivo_saida, "wb")) == NULL) ajuda("0x90", "Erro ao abrir o arquivo de saida"); // se houver erro...
  pri_vez = caractere_n;
  while (1)  {
    caractere_n++;
    if (caractere_n == pri_vez + 1){
      caractere = ~fgetc(entrada);
      if (caractere == '[') {
        for (var_int = 0; var_int < 20; var_int++) {
          caractere = ~fgetc(entrada);
          if (caractere != ']') {
            if (var_int == 0) {
              if (caractere == '-') {
                tem_senha = 2;
                caractere = ~fgetc(entrada);
                goto outx;
              } else {
                tem_senha = 1;
              }
              sprintf(senha_arquivo, "%c", ~caractere);
            } else {
              sprintf(senha_arquivo, "%s%c", senha_arquivo, ~caractere);
            }
          } else {
            goto outx;
          }
        }

      outx:
        if (tem_senha == 1) {
          limpa_tela();
          define_senha();
          if (strcmp(senha, senha_arquivo) == 0) {
            limpa_tela();
          } else {
            fclose(saida);
            remove(arquivo_saida);
            ajuda("0x80", "Senha incorreta");
          }
        }
      } else {
        fclose(saida);
        remove(arquivo_saida);
        ajuda("0x80", "Erro desconhecido");
      }
    } else {
      caractere = fgetc(entrada);
      if (feof(entrada))  break;
      sprintf(var_tmp, "%i", caractere);
      var_int = atoi(var_tmp);
      fprintf(saida, "%c", var_int - 5);
    }
    /* isso funciona assim: (caractere obtido = f) -> (f a igual a 102) -> (102 - 5 = 97) -> (97 = a) -> (entao 'f' = 'a') */
  }
}

int codifica(){ // Função: Codificar o arquivo

  if ((saida = fopen(arquivo_saida, "wb")) == NULL)  {
    ajuda("0x90", "Erro ao abrir o arquivo de saida");
  } else {
    pri_vez = caractere_n;
  }

  while (1) {
    caractere_n++;
    if (caractere_n == pri_vez + 1) {
      sprintf(var_tmp, "%c%s%c", ~'[', extensao_encriptada, ~']');
      if (quer_senha == 1) {
        sprintf(var_tmp2, "%c%s%c", ~'[', senha, ~']');
        fprintf(saida, "%s%s", var_tmp, var_tmp2);
      } else {
        fprintf(saida, "%s%c%c%c", var_tmp, ~'[', ~'-', ~']');
      }
    } else {
      caractere = fgetc(entrada);
      if (feof(entrada))
        break;
      sprintf(var_tmp, "%i", caractere);
      var_int = atoi(var_tmp);
      fprintf(saida, "%c", var_int + 5);
    }
  }
}

int define_variaveis(int mode_t)
{ // Função: Definir algumas variaveis globais

  sprintf(extensao_saida, "%s", acha_extensao(mode_t));
  sprintf(arquivo_saida, "%s.%s", nome, extensao_saida);
  if (mode_t == 1) {
    for (var_int = 0; var_int < strlen(extensao); var_int++) {
      if (var_int == 0) {
        sprintf(extensao_encriptada, "%c", ~extensao[var_int]);
      } else {
        sprintf(extensao_encriptada, "%s%c", extensao_encriptada, ~extensao[var_int]);
      }
    }
  }
}

int display(){                                          // Função: Puramente estatica
  system("mode 58, 10");                   // muda o tamanho do console
  system("title Encriptador de arquivos"); // muda o tatulo do console
  setlocale(LC_ALL, "Portuguese");         // define o idioma para portuguas
}

int main(int argc, char *argv[]){
  hStdout = GetStdHandle(STD_OUTPUT_HANDLE);      // define em hStdout o fluxo padrao
  GetConsoleScreenBufferInfo(hStdout, &csbiInfo); // pega a cor inicial da janela
  wOldColorAttrs = csbiInfo.wAttributes;          // define em "wOldColorAttrs" a cor inicial da janela

  display();
  sprintf(este_arquivo, "%s", acha_nome(argv[0], 2));

  if (argc > 2) {
    ajuda("0x50", "Namero de argumentos invalido");
  }
  else if (argc == 1) {
    msg_simples("\t\tPrograma criado por ");
    animacao("Lucas\n\n", 04);
    msg_simples("Modo de uso: ");
    animacao(este_arquivo, 03);
    animacao(" \"Arquivo\"\n\n", 03);
    pause();
    sair();
  } else {
    sprintf(arquivo_entrada, "%s", acha_nome(argv[1], 1));
    if ((entrada = fopen(arquivo_entrada, "rb")) == NULL) {
      ajuda("0x70", "Erro ao abrir o arquivo especificado");
    }
  }

  if (strcmpi(extensao, "hme") == 0) { // DECODIFICAçãO
    if ((caractere = fgetc(entrada)) != ~'[') { // Se o primeiro caractere nao for o "["...
      ajuda("0x60", "Erro desconhecido");
    } else { // Se o primeiro caractere do arquivo for "["...
      define_variaveis(2);
      decodifica();
    }
  }  else  { // CODIFICAçãO
    define_variaveis(1);
    protecao_senha();
    codifica();
  }
  return 0;
}
