PNG_file.cpp

#include <stdio.h>
#include <stdlib.h>
#include "PNG_file.h"

#define PNG_SIG_LENGTH 8 //The signature length for PNG
#define BYTE_SIZE 8 //Size of a byte
#define SIZE_WIDTH 32 //The number of bits used for storing the length of a file


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


//���� ũ�⸦ ����ϱ����� ��Ƽ �Լ�
unsigned int filesize(const char *filename)
{
    FILE *f = fopen(filename,"rb");  /* open the file in read only */

    unsigned int size = 0;
    if (fseek(f,0,SEEK_END)==0) /* seek was successful */
        size = ftell(f);
    fclose(f);
    return size;
}

PNG_file::PNG_file(const char *inputFileName) {

    FILE * inputFile;

    unsigned char header[BYTE_SIZE];//ũ�Ⱑ 8�� ������ ���� ����

    inputFile = fopen (inputFileName,"rb");//ó���� �Է��� ���� ����

    //������ �����ִ��� Ȯ��
    if(!inputFile)
        exit(1);

    
               //�б� ����
    fread(header, 1, PNG_SIG_LENGTH, inputFile);

    //png�������� Ȯ��
    if(png_sig_cmp(header, 0, PNG_SIG_LENGTH))
        exit(1);


    //libPNG ������ ���� �� ���� ó�� ����
    read_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!read_ptr)
        exit(1);

    info_ptr = png_create_info_struct(read_ptr);

    if (!info_ptr) {
        png_destroy_read_struct(&read_ptr,
            (png_infopp)NULL, (png_infopp)NULL);
        exit(1);
    }

    png_infop end_info = png_create_info_struct(read_ptr);

    if (!end_info) {
        png_destroy_read_struct(&read_ptr, &info_ptr,
            (png_infopp)NULL);
        exit(1);
    }
    //End������ ���� �� ����ó�� ����

    //PNG�� ���� IO�ʱ�ȭ
    png_init_io(read_ptr, inputFile);

    //�츮�� ������ �� ���� PNG_SIG_LENGTH�� libPNG�� ���� �˸�
    png_set_sig_bytes(read_ptr, PNG_SIG_LENGTH);

    //��ü PNG������ �޸𸮷� �о��
    png_read_png(read_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    row_pointers = png_get_rows(read_ptr, info_ptr);

    //���� read_ptr�� ����ִ� bit_depth�� 8�� �ƴҰ�� ����
    if(read_ptr->bit_depth != BYTE_SIZE)
        exit(1);

    fclose(inputFile);
}

void PNG_file::encode(const char *fileToEncodeName) {
    //���ڵ� ����

    FILE * fileToEncode;

    unsigned char buffer = 0;

    fileToEncode = fopen (fileToEncodeName,"rb");//���Ե� �̸��� ���� ����

    //������ �����ִ��� Ȯ��
    if(!fileToEncode)
        exit(1);

    //���� ũ��� ���� ����
    unsigned long size = filesize(fileToEncodeName);

    //�� �ڵ� ������ �Է� ������ �׸����� ���ڵ�
    //�Է� ������ ��Ʈ ������ �ּ� �߿䵵�� ���ڵ�
    for(int y=0; y < read_ptr->height; y++) {
        int x=0;
        if(y == 0)
            for(x; x < SIZE_WIDTH; x++) {
                if((size & ipow(2,x)))
                    *(row_pointers[y]+x) |= 1;
                else
                    *(row_pointers[y]+x) &= 0xFE;
            }
        for(x; x < read_ptr->width*3; x++) {
            if(x%BYTE_SIZE == 0) {
                if(!fread(&buffer, 1, 1, fileToEncode)) 
                    goto loop_end;
            }    
            if((buffer & ipow(2,x%BYTE_SIZE)))
                *(row_pointers[y]+x) |= 1;
            else
                *(row_pointers[y]+x) &= 0xFE;
        }
        //���� ũ�⸦ �ʹ� ũ�� ������� �ʾҴ��� Ȯ��
        if(y >= read_ptr->height)
            exit(1);
    }

    //��Ƽ������ �����ϱ� ���Ͽ� �̰����� ����
    loop_end:

    fclose(fileToEncode);

}

void PNG_file::decode(const char *outputFileName) {
    //���ڵ� ����

    FILE * outputFile;

    unsigned char buffer = 0;

    outputFile = fopen (outputFileName,"wb");//�Է¹��� ������ ���� ����

    //������ �����ִ��� Ȯ��
    if(!outputFile)
        exit(1);

    unsigned int size = 0;
    for(int y=0; y < read_ptr->height; y++) {
        int x=0;
        if(y == 0)
            for(x; x < SIZE_WIDTH; x++) {
                size |= ((*(row_pointers[0]+x) & 1 ) << x);
            }
        for(x; x < read_ptr->width*3; x++) {
            if((x > SIZE_WIDTH || y > 0) && x%BYTE_SIZE == 0) {
                fwrite(&buffer, 1, 1, outputFile);
                buffer = 0;
            }
            if(((read_ptr->width*y)*3+x) == size*BYTE_SIZE+SIZE_WIDTH)
                goto loop_end;
            buffer |= ((*(row_pointers[y]+x) & 1) << x%BYTE_SIZE);
        }
    }

    //���� ������ ����� ���� ����� �̵�
    loop_end:

    fclose(outputFile);

}

void PNG_file::outputPNG(const char *outputFileName) {
    //���⼭ ���� ����

    FILE * outputFile;

    outputFile = fopen (outputFileName,"wb");
    if(!outputFile)
        exit(1);

    //���� ���ؼ� PNG������ �ʱ�ȭ
    write_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!write_ptr)
        exit(1);

    png_init_io(write_ptr, outputFile);

    //Set the rows in the PNG structure
    png_set_rows(write_ptr, info_ptr, row_pointers);

    //Write the rows to the file
    png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(outputFile);
}