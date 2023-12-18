#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre, FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
    char comando[LONGITUD_COMANDO];
    char orden[LONGITUD_COMANDO];
    char argumento1[LONGITUD_COMANDO];
    char argumento2[LONGITUD_COMANDO];

    int i, j;
    unsigned long int m;
    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    int entradadir;
    int grabardatos;
    FILE *fent;

    // Lectura del fichero completo de una sola vez
    //...

    fent = fopen("particion.bin", "r+b");
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);

    memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(&directorio, (EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
    memcpy(&ext_bytemaps, (EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(&memdatos, (EXT_DATOS *)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);

    // Bucle de tratamiento de comandos
    for (;;)
    {
        do
        {
            printf(">> ");
            fflush(stdin);
            fgets(comando, LONGITUD_COMANDO, stdin);
            // memcpy(orden, comando, LONGITUD_COMANDO);

        } while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);
        printf("Salido del bucle");
        if (strcmp(orden, "dir") == 1)
        {

            Directorio(directorio, &ext_blq_inodos);
            continue;
        }
        else if (strcmp(orden, "info") == 1)
        {
            LeeSuperBloque(&ext_superblock);
            //continue;
        }
        else if (strcmp(orden, "bytemaps") == 1)
        {
            Printbytemaps(&ext_bytemaps);
            continue;
        }
        else if (strcmp(orden, "imprimir") == 1)
        {
            if (!Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1)) {
                    printf("Error al imprimir el fichero\n");
                }
            continue;
        }
        else if (strcmp(orden, "rename") == 1)
        {
            if (!Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2)) {
                    printf("Error al renombrar el fichero\n");
                }
            continue;
        }
        else if (strcmp(orden, "remove") == 1)
        {
            if (!Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent)) {
                    printf("Error al eliminar el fichero\n");
                }
            continue;
        }
        else if (strcmp(orden, "copy") == 1)
        {
            if (!Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent)) {
                    printf("Error al copiar el fichero\n");
                }
            continue;
        }
        
        //...
        // Escritura de metadatos en comandos rename, remove, copy
        /*Grabarinodosydirectorio(directorio, &ext_blq_inodos, fent);
        GrabarByteMaps(&ext_bytemaps, fent);
        GrabarSuperBloque(&ext_superblock, fent);
        if (grabardatos)
            GrabarDatos(memdatos, fent);
        grabardatos = 0;*/
        // Si el comando es salir se habrán escrito todos los metadatos
        // faltan los datos y cerrar
        else if (strcmp(orden, "salir") == 1)
        {
            GrabarDatos(memdatos, fent);
            fclose(fent);
            return 0;
        }
        else{
            printf("ERROR: Comando ilegal [bytemaps, copy, dir, info, imprimir, rename, remove, salir]");
        }
    }
}
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps)
{
}
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2)
{
    const char *comandosPosibles[] = {"info\0", "bytemaps\0", "dir\0", "rename\0","imprimir\0", "remove\0", "copy\0"};
    char letra;
    int i = 0;
    int valorRetorno = 1;
    while(((letra = strcomando[i++]) != ' ') && (letra != '\0')){
        orden[i-1] = letra;
		printf("%c", orden[i-1]);
    }
    orden[i-1] = '\0';
	
    i = 0;
    for(int j = 0; j < 7 && valorRetorno == 1; j++){
		
		
        while(comandosPosibles[j][i] == orden[i] && comandosPosibles[j][i] != '\0' && orden[i] != '\0')
		{
			printf("%c - %c\n", comandosPosibles[j][i], orden[i]);
			i++;
		}
        if(orden[i+1] == '\0'){
            valorRetorno = 0;
        }
		i=0;
    }
	if(valorRetorno == 1){
		printf("ERROR: Comando ilegal [bytemaps, copy, dir, info, imprimir, rename, remove, salir]\n");
	}
    return valorRetorno;
}
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup)
{
    printf("Información del Superbloque:\n");
    printf("Total de inodos: %u\n", psup->s_inodes_count);
    printf("Total de bloques: %u\n", psup->s_blocks_count);
    printf("Bloques libres: %u\n", psup->s_free_blocks_count);
    printf("Inodos libres: %u\n", psup->s_free_inodes_count);
    printf("Primer bloque de datos: %u\n", psup->s_first_data_block);
    printf("Tamaño del bloque: %u bytes\n", psup->s_block_size);

}
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre)
{
}
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos)
{
}
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo)
{
}
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre)
{
}
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre, FILE *fich)
{
}
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich)
{
}
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich)
{
}
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich)
{
}
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich)
{
}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich)
{
}
