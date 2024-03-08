El binario compilado es "main". Para compilar: 
gcc main.c crc.c fileLock.c -w -o main

Las pruebas se han hecho con el archivo test.txt. Su crc computado sin modificar es test.txt.crc y el segundo archivo en el que generar y leer los bloques de crc es test2.txt.crc.
./main test.txt test2.txt.crc

Para computar el crc de un archivo de texto
./checksum [archivo] -generate