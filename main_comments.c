main.cpp

#include "PNG_file.h"
void main() {
    PNG_file link = PNG_file("big.png");//PNG_file.h�� PNG_file������ big.png�� ������ link���� ����
    link.encode("1.txt");//�Ȱ��� encode�Լ��� 1.txt����
    link.outputPNG("bigen.png");//���� ������� bigen.png����
    PNG_file link = PNG_file("bigensimple.png");//���⵵~
//    link.decode("2.txt");//~~
}