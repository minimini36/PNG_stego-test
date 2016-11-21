encode.cpp
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "encode.h"

#define PNG_SIG_LENGTH 8

//��� �� ���� ������ ����� ǥ�� ����
int ipow(int base, int exp) {
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}


//���� ũ�⸦ ����ϱ� ���� ��Ƽ�Լ�
unsigned long filesize(const char *filename)
{
    FILE *f = fopen(filename,"rb");  /* open the file in read only */

    unsigned long size = 0;
    if (fseek(f,0,SEEK_END)==0) /* seek was successful */
        size = ftell(f);
    fclose(f);
    return size;
}


int encode(const char *inputFileName, const char *encodeFileName, const char *outputFileName) {

    FILE * inputFile;

    unsigned char header[8];

    inputFile = fopen (inputFileName,"rb");

    //������ �����ִ��� Ȯ��
    if(!inputFile)
        return 1;
             
             //���� �б�
    fread(header, 1, PNG_SIG_LENGTH, inputFile);

    //PNG�������� Ȯ��
    if(png_sig_cmp(header, 0, PNG_SIG_LENGTH))
        return 1;


    //libPNG ������ ���� �� ���� ó�� ����
    png_structp read_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!read_ptr)
        return 1;

    png_infop info_ptr = png_create_info_struct(read_ptr);

    if (!info_ptr) {
        png_destroy_read_struct(&read_ptr,
            (png_infopp)NULL, (png_infopp)NULL);
        return 1;
    }

    png_infop end_info = png_create_info_struct(read_ptr);

    if (!end_info) {
        png_destroy_read_struct(&read_ptr, &info_ptr,
            (png_infopp)NULL);
        return 1;
    }
    //������ ���� / ���� ó�� ���� ������

    //PNG�� ���� IO �ʱ�ȭ
    png_init_io(read_ptr, inputFile);

    //�츮�� ������ �� ���� PNG_SIG_LENGTH�� libPNG�� ���� �˸�
    png_set_sig_bytes(read_ptr, PNG_SIG_LENGTH);

    png_read_png(read_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_bytep* row_pointers = png_get_rows(read_ptr, info_ptr);

    printf("Read complete
");

    if(read_ptr->bit_depth != 8)
        return 1;

    //���ڵ� ����

    FILE * fileToEncode;

    unsigned char buffer;

    fileToEncode = fopen (encodeFileName,"rb");

    //Check if the file opened
    if(!fileToEncode)
        return 1;

    //������ ����� ���� �߰�
    unsigned long size = filesize(encodeFileName);

    fread(&buffer, 1, 1, fileToEncode);

    //�� �ڵ� ������ �Է� ������ �׸����� ���ڵ�
    //�Է� ������ ��Ʈ ������ �ּ� �߿䵵�� ���ڵ�
    for(int y=0; y < 1; y++) {
        int x=0;
        if(y == 0)
            for(x; x < 16; x++) {
                if((size & ipow(2,x)))
                    *(row_pointers[0]+x) |= 1;
            }
        for(x; x < read_ptr->width*3; x++) {
            if(x != 0 && x%8 == 0)
                if(!fread(&buffer, 1, 1, fileToEncode))
                    break;
            if((buffer & ipow(2,x%8)))
                *(row_pointers[y]+x) |= 1;
        }
    }

    //���� ����

    FILE * outputFile;

    outputFile = fopen (outputFileName,"wb");//�Է¹��� ������ ���� ����

    //������ �����ִ��� Ȯ��
    if(!outputFile)
        return 1;

    //���� ���� PNG ������ �ʱ�ȭ
    png_structp write_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!write_ptr)
        return 1;

    png_init_io(write_ptr, outputFile);

    //PNG ������ �� ����
    png_set_rows(write_ptr, info_ptr, row_pointers);

    //���� ���Ͽ� ��
    png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(outputFile);
    fclose(inputFile);

    return 0;
}