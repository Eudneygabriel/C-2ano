#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

// Declaração antecipada da função
void remover_acentos(char *str);
void remover_aspas(char *str);

void traduzirLinha(FILE *saida, char *linha)
{
    if (strncmp(linha, "defina", 6) == 0)
    {
        char var[100];
        int valor;
        char op[100];
        if (sscanf(linha, "defina %s como %s", var, op) == 2)
        {
            if (sscanf(op, "%d", &valor) == 1)
                fprintf(saida, "int %s = %d;\n", var, valor);
            else
                fprintf(saida, "int %s = %s;\n", var, op);
        }
    }
    else if (strncmp(linha, "diga", 4) == 0)
    {
        char texto[256];
        sscanf(linha, "diga %[^\n]", texto);
        remover_aspas(texto);
        remover_acentos(texto);
        fprintf(saida, "printf(\"%s\\n\");\n", texto);
    }
    else if (strncmp(linha, "se", 2) == 0)
    {
        char var[100], op[20], msg[100];
        int valor;
        sscanf(linha, "se %s for %s %d, diga %[^\n]", var, op, &valor, msg);
        remover_aspas(msg);
        remover_acentos(msg);

        if (strcmp(op, "maior") == 0)
            fprintf(saida, "if (%s > %d) {\n", var, valor);
        else if (strcmp(op, "menor") == 0)
            fprintf(saida, "if (%s < %d) {\n", var, valor);
        else if (strcmp(op, "igual") == 0)
            fprintf(saida, "if (%s == %d) {\n", var, valor);

        fprintf(saida, "    printf(\"%s\\n\");\n", msg);
        fprintf(saida, "}\n");
    }
    else if (strncmp(linha, "enquanto", 8) == 0)
    {
        char var[100], op[20], msg[100];
        int valor;
        sscanf(linha, "enquanto %s for %s %d, diga %[^\n]", var, op, &valor, msg);
        remover_aspas(msg);

        char sinal[3];
        if (strcmp(op, "menor") == 0)
            strcpy(sinal, "<");
        else if (strcmp(op, "maior") == 0)
            strcpy(sinal, ">");
        else
            strcpy(sinal, "==");

        // Se a mensagem é uma variável, imprimimos com %d, senão como string
        if (strcmp(var, msg) == 0)
        {
            fprintf(saida, "while (%s %s %d) {\n", var, sinal, valor);
            fprintf(saida, "    printf(\"%%d\\n\", %s);\n", msg);
        }
        else
        {
            remover_acentos(msg);
            fprintf(saida, "while (%s %s %d) {\n", var, sinal, valor);
            fprintf(saida, "    printf(\"%s\\n\");\n", msg);
        }
        fprintf(saida, "    %s++;\n", var);
        fprintf(saida, "}\n");
    }
    else if (strncmp(linha, "funcao", 6) == 0 || strncmp(linha, "função", 7) == 0)
    {
        char nome[100];
        sscanf(linha, "funcao %s()", nome);
        fprintf(saida, "void %s() {\n", nome);
        fprintf(saida, "    printf(\"Funcao %s chamada!\\n\");\n", nome);
        fprintf(saida, "}\n");
    }
}

int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");

    FILE *entrada = fopen("teste.talk", "r");
    if (!entrada)
    {
        perror("Erro ao abrir teste.talk");
        return 1;
    }

    FILE *saida = fopen("gerado.c", "w");
    if (!saida)
    {
        perror("Erro ao criar gerado.c");
        fclose(entrada);
        return 1;
    }

    fprintf(saida, "#include <stdio.h>\nint main() {\n");

    char linha[256];
    while (fgets(linha, sizeof(linha), entrada))
    {
        traduzirLinha(saida, linha);
    }

    fprintf(saida, "return 0;\n}\n");

    fclose(entrada);
    fclose(saida);

    printf("Arquivo gerado: gerado.c\n");

    return 0;
}

// Remove acentos apenas dos textos (não dos comandos)
void remover_acentos(char *str)
{
    const char acentos[] = "áàãâäéèêëíìîïóòôõöúùûüçÁÀÃÂÄÉÈÊËÍÌÎÏÓÒÔÕÖÚÙÛÜÇ";
    const char sem_acentos[] = "aaaaaeeeeiiiiooooouuuucAAAAAEEEEIIIIOOOOOUUUUC";

    for (char *p = str; *p; p++)
    {
        for (int i = 0; acentos[i] != '\0'; i++)
        {
            if (*p == acentos[i])
            {
                *p = sem_acentos[i];
                break;
            }
        }
    }
}

// Remove aspas do início e fim de uma string, se houver
void remover_aspas(char *str)
{
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}
