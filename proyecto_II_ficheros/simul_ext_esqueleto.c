#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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
            } else{
                Grabarinodosydirectorio(directorio, &ext_blq_inodos, fent);
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
            argumento2[contadorArgumanto - 1] = '\0';
        }else{
			argumento1[contadorArgumanto - 1] = '\0';
			argumento2[0] = '\0';
		}
    }
    else{
        orden[i - 1] = '\0';
		argumento1[0] = '\0';
		argumento2[0] = '\0';
	}

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
    //imprime en pantalla los bloques
    printf("Bloques de %u Bytes \n", psup->s_block_size);
    printf("Inodos particion = %u\n", psup->s_inodes_count);
    printf("Inodos libres = %u\n", psup->s_free_inodes_count);
    printf("Bloques particion = %u\n", psup->s_blocks_count);
    printf("Bloques libres = %u\n", psup->s_free_blocks_count);
    printf("Primer bloque de datos = %u\n", psup->s_first_data_block);
}
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre)
{
	for(int i = 0; i < MAX_INODOS; i++)
		if (strcmp(directorio[i].dir_nfich, nombre) == 0){
			return i;
			
		}
	return -1;
}
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
    //recorre el directorio y muestra los archivos que tengan un size mayor que 0 y cuyo inodo sea distinto a 0xFFFF
    for (int i = 0; i < MAX_INODOS; i++) {
        if (directorio[i].dir_inodo != 0xFFFF && inodos->blq_inodos[directorio[i].dir_inodo].size_fichero > 0 && directorio[i].dir_inodo > 0) { 
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
	int encontrado = 0;
	int i;
	/*Se comprueba que haya el numero de argumentos necesario*/
	if(nombreantiguo[0] == '\0' || nombrenuevo[0] == '\0')
	{
		printf("Faltan argumentos.\n");
		return 0;
	}
	
	/*Se busca el fichero al que queremos cambiarle el nombre*/
	if((i = BuscaFich(directorio, inodos, nombreantiguo)) == -1){
		/*En caso de no encontrarlo, se retorna un mensaje con el error y se vuelve al main*/
		printf("No se ha encontrado el fichero.\n");
		return 0;
	}
	
	/*Se copia el nombre nuevo en el lugar donde se alojaba el antiguo*/
	memcpy(directorio[i].dir_nfich, nombrenuevo, LEN_NFICH);
	return 1;
}
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre)
{
	int encontrado = 0;
    int inodo_actual, directorio_actual;
	int *numerosBloques = (int*)malloc(0);
	int numeroBloquesEncontrados = 0;
	
	/*Comprobamos que se haya pasado un nombre despues del comando*/
	if(nombre[0] == '\0')
	{
		printf("Faltan argumentos.\n");
		return 0;
	}
    /*Buscar el archivo en el directorio, llamando a la funcion BuscaFich*/
    if ((directorio_actual = BuscaFich(directorio, inodos, nombre)) == -1) {
        printf("El archivo %s no existe.\n", nombre);
        return 0;
    }else{
		/*Asignamos el inodo en el que nos encontramos según el directorio que nos ha devuelto*/
		inodo_actual = directorio[directorio_actual].dir_inodo;
	}
	
	/*Hacemos un bucle para ir recopilando todos los indices de los bloques que ocupa el fichero*/
	while (inodo_actual != NULL_INODO) {
        for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
            if (inodos->blq_inodos[inodo_actual].i_nbloque[i] != NULL_BLOQUE) {
				numeroBloquesEncontrados++;
                numerosBloques = (int*)realloc(numerosBloques, sizeof(int) * numeroBloquesEncontrados);
				numerosBloques[numeroBloquesEncontrados - 1] = inodos->blq_inodos[inodo_actual].i_nbloque[i] - 4;
            }
        }
        inodo_actual = inodos->blq_inodos[inodo_actual].i_nbloque[MAX_NUMS_BLOQUE_INODO - 1];
    }
	
	
	int nBloqueCopia[numeroBloquesEncontrados];
	
	int posicion = 0;
	/*Los ordenamos para así poderlos mostrar por pantalla de forma correcta.*/
	for(int i = 0; i < numeroBloquesEncontrados; i++){
		for(int j = 0; j < numeroBloquesEncontrados; j++){
			
			if(numerosBloques[i] > numerosBloques[j]){
				
				posicion++;
			}
		}
		nBloqueCopia[posicion] = numerosBloques[i];
		
		posicion = 0;
	}
	
    /*Imprimimos los datos almacenados de forma ordenada usando el array ordenado y la variable 
	numBloquesEncontrados como limite para que no se rompa nada*/
    for(int i = 0; i < numeroBloquesEncontrados; i++)
	{
        printf("%s", memdatos[nBloqueCopia[i]].dato);
	}
	
	printf("\n");
    return 1;
}
void LiberarBloque(unsigned int num_bloque, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock) {
    //libera un bloque declarando su valor a 0
    ext_bytemaps->bmap_bloques[num_bloque] = 0;
    ext_superblock->s_free_blocks_count++;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombreCompleto, FILE *fich) {
    //borra un fichero, para ello recorre inodos y pone el valor 0xFFFF, ademas en bytemaps declara el valor a 0. luego sobreescribe el binario.
    for (int i = 0; i < MAX_INODOS; i++) {
        if (directorio[i].dir_inodo != 0xFFFF && strcmp(directorio[i].dir_nfich, nombreCompleto) == 0) {
            unsigned int inodo_id = directorio[i].dir_inodo;
            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
				if (inodos->blq_inodos[inodo_id].i_nbloque[j] != 0xFFFF){
					ext_bytemaps->bmap_bloques[inodos->blq_inodos[inodo_id].i_nbloque[j]] = 0;
					inodos->blq_inodos[inodo_id].i_nbloque[j] = 0xFFFF;
				}
            }
			
            ext_bytemaps->bmap_inodos[inodo_id] = 0;
            ext_superblock->s_free_inodes_count++; 

            memset(&directorio[i], 0, sizeof(EXT_ENTRADA_DIR));
            directorio[i].dir_inodo = 0xFFFF;
            GrabarSuperBloque(ext_superblock, fich);
            GrabarByteMaps(ext_bytemaps, fich);
            Grabarinodosydirectorio(directorio, inodos, fich);
			
            return 1; 
        }
    }
	
    return 0; 
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich)
{
	int i, encontrado = 0;
	int numDirectorioOriginal, numDirectorio = 0;
	/*Comprobamos que se haya pasado por comandos algun argumento que haga referencia al fichero de origen*/
	if(nombreorigen[0] == '\0' || nombredestino[0] == '\0')
	{
		printf("Faltan argumentos.\n");
		return 0;
	}
	/*Comprobamos si existe el fichero, sino se muestra el tipo de error y vuelve al main*/
	if((numDirectorioOriginal = BuscaFich(directorio, inodos, nombreorigen)) == -1){
		printf("No se ha encontrado el archivo.\n");
		return 0;
	}
	/*Comprobamos que haya un segundo argumento que haga referencia al nuevo archivo*/
	
	/*Comprobamos que ese archivo no exista ya*/
	if((numDirectorio = BuscaFich(directorio, inodos, nombredestino)) == -1){
		/*En caso de no existir, se le busca un directorio que no esté ocupado*/
		for(int i = 1; i < MAX_INODOS; i++)
		if (directorio[i].dir_inodo == 0xFFFF)
		{
			numDirectorio = i;
			break;
		}
		
	}else{
		/*En caso de que existe, se borra para así poder empezar de 0*/
		Borrar(directorio, inodos, ext_bytemaps, ext_superblock, nombredestino, fich);
	}
	
	/*Se copia el nombre deseado*/
	memcpy(directorio[numDirectorio].dir_nfich, nombredestino, LEN_NFICH);
	
	
	/*Se busca el i-nodo que va a ocupar*/
	encontrado = 0;
	for(int i = 0; i < MAX_INODOS; i++){
		if(ext_bytemaps->bmap_inodos[i] == 0){
			encontrado = 1;
			ext_bytemaps->bmap_inodos[i] = 1;
			directorio[numDirectorio].dir_inodo = i;
			break;
		}
	}
	/*En caso de que no lo pueda encontrar, retorna al main, donde se mostrará que no se ha podido copiar*/
	if(!encontrado){
		return 0;
	}
	
	/*Se buscan tanto el numero de bloques como donde alojarlos y se instancian donde se encuentre vacio*/
	int bloquesEncontrados = 0;
    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        if (inodos->blq_inodos[directorio[numDirectorioOriginal].dir_inodo].i_nbloque[i] != NULL_BLOQUE) {
				
			for(int j = 0; j < MAX_BLOQUES_DATOS; j++){
				if(ext_bytemaps->bmap_bloques[j] == 0){
					ext_bytemaps->bmap_bloques[j] = 1;
					
					inodos->blq_inodos[directorio[numDirectorio].dir_inodo].i_nbloque[bloquesEncontrados] = j;
					
					memdatos[j - 4] = memdatos[inodos->blq_inodos[directorio[numDirectorioOriginal].dir_inodo].i_nbloque[i] - 4];
					
					bloquesEncontrados++;
					break;
				}
			}
        }
    }
	
	/*Se copia el tamaño del fichero*/
	inodos->blq_inodos[directorio[numDirectorio].dir_inodo].size_fichero = inodos->blq_inodos[directorio[numDirectorioOriginal].dir_inodo].size_fichero;
	
	/*Se actualiza el fichero*/
	GrabarSuperBloque(ext_superblock, fich);
    GrabarByteMaps(ext_bytemaps, fich);
    Grabarinodosydirectorio(directorio, inodos, fich);
    GrabarDatos(memdatos, fich);
	return 1;
}
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich)
{
    fseek(fich, 2 * SIZE_BLOQUE, SEEK_SET);
    fwrite(inodos, sizeof(EXT_BLQ_INODOS), 1, fich);
    fseek(fich, 3 * SIZE_BLOQUE, SEEK_SET);
    fwrite(directorio, sizeof(EXT_ENTRADA_DIR) * MAX_FICHEROS, 1, fich);
    fflush(fich);
}
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich)
{
    fseek(fich, SIZE_BLOQUE, SEEK_SET);
    fwrite(ext_bytemaps, sizeof(EXT_BYTE_MAPS), 1, fich);
    fflush(fich);
}
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich)
{
    fseek(fich, 0, SEEK_SET);
    fwrite(ext_superblock, sizeof(EXT_SIMPLE_SUPERBLOCK), 1, fich);
    fflush(fich);
}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich)
{
    fseek(fich, 4 * SIZE_BLOQUE, SEEK_SET);
    fwrite(memdatos, SIZE_BLOQUE, MAX_BLOQUES_DATOS, fich);
    fflush(fich);
}
