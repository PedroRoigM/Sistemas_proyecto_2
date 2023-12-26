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

        } while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);

        if (strcmp(orden, "dir") == 0)
        {
            Directorio(directorio, &ext_blq_inodos);
            continue;
        }
        else if (strcmp(orden, "info\0") == 0)
        {
            LeeSuperBloque(&ext_superblock);
            // continue;
        }
        else if (strcmp(orden, "bytemaps\0") == 0)
        {
            printf("Bytemaps:\n");
            Printbytemaps(&ext_bytemaps);
            continue;
        }
        else if (strcmp(orden, "imprimir\0") == 0)
        {
            if (!Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1))
            {
                printf("Error al imprimir el fichero\n");
            }
            continue;
        }
        else if (strcmp(orden, "rename\0") == 0)
        {
            if (!Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2))
            {
                printf("Error al renombrar el fichero\n");
            }
            continue;
        }
        else if (strcmp(orden, "remove\0") == 0)
        {
            if (!Borrar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent))
            {
                printf("Error al eliminar el fichero\n");
            }
            continue;
        }
        else if (strcmp(orden, "copy\0") == 0)
        {
            if (!Copiar(directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent))
            {
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
        else if (strcmp(orden, "salir") == 0)
        {
            GrabarDatos(memdatos, fent);
            fclose(fent);
            return 0;
        }
    }
}
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps)
{
    printf("Inodos: ");
    for (int i = 0; i < MAX_INODOS; i++)
    {
        printf("%d ", ext_bytemaps->bmap_inodos[i]);
    }
    printf("\nBloques [0 - 25]: ");
    for (int i = 0; i < 25; i++)
    {
        printf("%d ", ext_bytemaps->bmap_bloques[i]);
    }
    printf("\n");
}
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2)
{
    const char *comandosPosibles[] = {"info", "bytemaps", "dir", "rename", "imprimir", "remove", "copy", "salir"};
    int i = 0;
    int contadorArgumanto = 0;
    int valorRetorno = 1;
    while (strcomando[i] != ' ' && strcomando[i] != '\0')
    {
        orden[i] = strcomando[i];
        i++;
    }
    if (strcomando[i] == ' ')
    {
        orden[i] = '\0';
        i++;
        while (strcomando[i] != ' ' && strcomando[i] != '\0')
        {
            argumento1[contadorArgumanto] = strcomando[i];
            i++;
            contadorArgumanto++;
        }
        argumento1[contadorArgumanto] = '\0';
        if (strcomando[i] == ' ')
        {
            i++;
            contadorArgumanto = 0;
            while (strcomando[i] != ' ' && strcomando[i] != '\0')
            {

                argumento2[contadorArgumanto] = strcomando[i];
                i++;
                contadorArgumanto++;
            }
            argumento2[contadorArgumanto] = '\0';
        }else
			argumento1[contadorArgumanto - 1] = '\0';
    }
    else
        orden[i - 1] = '\0';

    for (int j = 0; j < 8 && valorRetorno == 1; j++)
    {

        if (strcmp(comandosPosibles[j], orden) == 0)
        {
            valorRetorno = 0;
        }
    }

    if (valorRetorno == 1)
    {
        printf("ERROR: Comando ilegal [bytemaps, copy, dir, info, imprimir, rename, remove, salir]\n");
    }

    return valorRetorno;
}
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup)
{
    printf("Bloques de %u Bytes \n", psup->s_block_size);
    printf("Inodos particion = %u\n", psup->s_inodes_count);
    printf("Inodos libres = %u\n", psup->s_free_inodes_count);
    printf("Bloques particion = %u\n", psup->s_blocks_count);
    printf("Bloques libres = %u\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos = %u\n", psup->s_first_data_block);
}
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre)
{
}
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
    for (int i = 0; i < MAX_INODOS; i++) {
        if (directorio[i].dir_inodo != 0xFFFF && inodos->blq_inodos[directorio[i].dir_inodo].size_fichero > 0 && directorio[i].dir_inodo > 0 && directorio[i].dir_inodo > 0) { 
            printf("%s\ttamano:%d\tinodo:%d\tbloques: ", 
                   directorio[i].dir_nfich,
                   inodos->blq_inodos[directorio[i].dir_inodo].size_fichero,
                   directorio[i].dir_inodo);
            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                if (inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[j] != 0xFFFF) {
                    printf("%d ", inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[j]);
                }
            }
            printf("\n");
        }
    }
}
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo)
{
}
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre)
{
	int encontrado = 0;
    int inodo_actual;
	int a;

    // Buscar el archivo en el directorio
    for (int i = 0; i < MAX_INODOS; i++) {
        if (strcmp(directorio[i].dir_nfich, nombre) == 0) {
            encontrado = 1; 
			a = i;
            inodo_actual = directorio[i].dir_inodo;
            break;
        }
    }
	
    if (!encontrado) {
        printf("El archivo %s no existe.\n", nombre);
        return 0;
    }

    // Imprimir el contenido del archivo
    while (inodo_actual != NULL_INODO) {
		
        for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
			
            if (inodos->blq_inodos[inodo_actual].i_nbloque[i] != NULL_BLOQUE) {
				
                printf("%s\n", memdatos[inodos->blq_inodos[inodo_actual].i_nbloque[i] - 4].dato);
            }
        }

        inodo_actual = inodos->blq_inodos[inodo_actual].i_nbloque[MAX_NUMS_BLOQUE_INODO - 1];
    }

    return 1;
}
void LiberarBloque(unsigned int num_bloque, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock) {
    ext_bytemaps->bmap_bloques[num_bloque] = 0;
    ext_superblock->s_free_blocks_count++;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombreCompleto, FILE *fich) {
    for (int i = 0; i < MAX_INODOS; i++) {
        if (directorio[i].dir_inodo != 0xFFFF && strcmp(directorio[i].dir_nfich, nombreCompleto) == 0) {
            unsigned int inodo_id = directorio[i].dir_inodo;
            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                unsigned int bloque = inodos->blq_inodos[inodo_id].i_nbloque[j];
                if (bloque != 0xFFFF) {
                    ext_bytemaps->bmap_bloques[bloque] = 0;
                    ext_superblock->s_free_blocks_count++; 
                }
            }

            ext_bytemaps->bmap_inodos[inodo_id] = 0;
            ext_superblock->s_free_inodes_count++; 

            memset(&directorio[i], 0, sizeof(EXT_ENTRADA_DIR));
            directorio[i].dir_inodo = 0xFFFF;
            return 1; 
        }
    }

    return 0; 
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich)
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
