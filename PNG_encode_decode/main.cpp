#include "PNG_file.h"

void err(char *filename)
{
	fprintf(stderr,"%s [-e <input> -o <output_image> | -d <output>] <image>\n",filename);
	fprintf(stderr,"\t -e\t Conceal the input data\n");
	fprintf(stderr,"\t -d\t Reveal the encoded data\n");
}

int main(int argc, char *argv[])
{
	/*
	 * Command Options
	 * stego.exe [-e <input>] [-d <output>] <image>
	 * Encode : -e
	 * Decode : -d
	 * Output : -o
	 */

	if(argc < 4)
	{
		err(argv[0]);
		return -1;
	}

	/* steganograhpy�� �̿��ؼ� �޼����� ����*/
	if(argv[1][1] == 'e')
	{
		/* ����� ��� ���α׷� ����� argumnet�� �� 6�� �ʿ��� */
		if(argc != 6)
		{
			err(argv[0]);
			return -1;
		}

		PNG_file link = PNG_file(argv[5]);
		link.encode(argv[2]);
		link.outputPNG(argv[4]);
	}
	/* steganography�� �̿��ؼ� ������ �޼����� ���� */
	else if(argv[1][1] == 'd')
	{
		/* �����ϴ� ��� ���α׷� ����� argument�� �� 4�� �ʿ���*/
		if(argc != 4)
		{
			err(argv[0]);
			return -1;
		}

		PNG_file link = PNG_file(argv[3]);
		link.decode(argv[2]);
	}

	return 0;
}