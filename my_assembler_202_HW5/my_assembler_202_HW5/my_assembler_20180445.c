/*
 * ȭ�ϸ� : my_assembler_20180445.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

 /*
  *
  * ���α׷��� ����� �����Ѵ�.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20180445.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ����
 * ��ȯ : ���� = 0, ���� = < 0
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
 *		   ���� �߰������� �������� �ʴ´�.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[])
{
    //inst_tablel�� input_data(�ҽ��ڵ� ���̺�)�� �����.
    if (init_my_assembler() < 0)
    {
        printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
        return -1;
    }

    if (assem_pass1() < 0)
    {
        printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
        return -1;
    }
    
    make_opcode_output("output_20180445.txt");

    /*
    * ���� ������Ʈ���� ���Ǵ� �κ�

    make_symtab_output("symtab_20180445");
    if(assem_pass2() < 0 ){
        printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ;
        return -1 ;
    }

    make_objectcode_output("output_20180445") ;
    */

    while (inst_index) {
        free(inst_table[inst_index]);
        --inst_index;
    }
    while (line_num) {
        free(input_data[line_num]);
        --line_num;
    }
    while (token_line) {
        free(token_table[token_line]);
        --token_line;
    }
    
    /*free(inst_table);
    free(token_table);
    free(input_data);*/
    
    return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
    int result;

    // inst.txt�� �о� inst_table�� �����.
    if ((result = init_inst_file("inst.txt")) < 0)
        return -1;
 
    // input.txt ������ ���پ� �о� �ҽ��ڵ� ���̺�(input_table)�� �����.
    if ((result = init_input_file("input.txt")) < 0)
        return -1;

    return result;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
 *        �����ϴ� �Լ��̴�.
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */

int init_inst_file(char* inst_file)
{
    FILE* file;
    int errno;

    errno = 0;
    file = fopen(inst_file, "r"); //���� ��������

    if (file == NULL) { //���� ������ ����ó��
        printf("���� - ����\n");
    }
    char instRead[200] = { 0, }; //�� �پ� �о� ������ ����
    char* ptr = { 0, }; //�� �پ� �о�� ������ tab ������ �߶� ������ ������ ����
    inst_index = 0; //�� ��° ��ɾ����� ���� �Ű��� index ��ȣ 
    int count = 0; //�ش� �ٿ��� �� ��° token ���� ������ index

     // inst_table ���� �Ҵ�
    *inst_table = (inst*)calloc(100, sizeof(inst));


    while (!feof(file)) { //������ ������ ���� ������ �ݺ��Ͽ� �� �پ� �о������ ��
 
       // inst_table ���� ���� �Ҵ�
        inst_table[inst_index] = (char*)calloc(100, sizeof(char));
       
        fgets(instRead, 200, file); //��ɾ� ������ �� �پ� �о instRead ������ �����Ѵ�.
        ptr = strtok(instRead, "\t"); // �о�� ���� tab �������� �߶� ù ��° token�� ptr�� ����
        inst_table[inst_index]->mnenmonic = malloc(200 * sizeof(char));
        strcpy(inst_table[inst_index]->mnenmonic, ptr); //����ü �ش� index�� mnenmonic �����ȿ� ù ��° token�� ����
        count++; // table�� ù ��° token�� �־����� ī��Ʈ + 1 (token�� ���������� ī��Ʈ�� �����ؼ� �����ϵ��� ��)

        while (ptr != NULL) { //���� token�� NULL�� �ƴϸ� ��� �ݺ� (�� ���� �� == NULL)
           
            ptr = strtok(NULL, "\t"); // �о�� ���� tab �������� �߶� token�� ptr�� ����
            
            switch (count) { //�� ��° token���� count�� Ȯ��
            case 1:
                inst_table[inst_index]->name = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->name, ptr); //count == 1�̸� name�� ���� ����
                count++; //name�� �־����� �� ī���͸� 1 �������ֱ�
                break;
            case 2:
                inst_table[inst_index]->format = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->format, ptr); //count == 2�̸� format�� ���� ����
                count++; //format�� �־����� �� ī���͸� 1 �������ֱ�
                break;
            case 3:
                inst_table[inst_index]->opcode = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->opcode, ptr); // count == 3�̸� opcode�� ���� ����
                count++; //operator�� �־����� �� ī���͸� 1 �������ֱ�
                break;
            case 4:
                inst_table[inst_index]->operand_unit = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->operand_unit, ptr); // count == 4 �̸� operand_unit�� ���� ����
                count++; //operand_unit�� �־����� �� ī���͸� 1 �������ֱ�
                break;
            case 5: // count == 5 �� operand_unit�� ����� Ȯ���ؼ� �־��ֱ�
                if (strcmp(inst_table[inst_index]->operand_unit, "0"))
                    break; // 0�̸� ���� ���� �����ϱ� �ƹ��͵� ���ϰ� break
                else {
                    inst_table[inst_index]->operand1 = malloc(200 * sizeof(char));
                    strcpy(inst_table[inst_index]->operand1, ptr); //0�� �ƴϸ� �ϴ� �ϳ� �־��ֱ�
                    count++; //operand1�� �־����� ī���͸� 1 �������ֱ�
                    break;
                }
            case 6: // count == 6�̸� operand_unit�� 2������ Ȯ���ϱ�
                if ((inst_table[inst_index]->operand_unit) == "2") {
                    inst_table[inst_index]->operand2 = malloc(200 * sizeof(char));
                    strcpy(inst_table[inst_index]->operand2, ptr); //2�̸� operand2�� �־��ֱ�
                    break;
                }
                else //2�� �ƴϸ� �ƹ��͵� ���� ���� �����ϱ� �ٷ� break
                    break;
            default:
                printf("count = %d default error", count); //default�϶� error �޼��� ����ϵ��� ��
                break;
            }

        }

        //�� ���� �� ����ü�� �Է������� 
        count = 0; //count �ٽ� 0���� �ʱ�ȭ
        ++inst_index; //inst_index 1 ���������� (���� ��ɾ���� �о���� ��)

    }

    fclose(file);
    return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
    FILE* file;
    int errno;

    errno = 0;
    file = fopen(input_file, "r");

    line_num = 0; //���δ����� ������ index
    char inputRead[200] = { 0, }; //���پ� �о�� ���� �ӽ÷� ������ ����
    *input_data = (char**)calloc(MAX_INST, sizeof(char*)); //input_data ���� �Ҵ� �� �ʱ�ȭ
    
    while (!feof(file)) { //������ ������ ���� ������ �ݺ�
       
        //input_data�� ���� input_data[line_num] �����Ҵ� �� �ʱ�ȭ
        input_data[line_num] = (char*)calloc(200, sizeof(char));

        fgets(inputRead, 200, file); //���پ� �о�ͼ� �ӽ÷� inputRead�� ����
       
        strcpy(input_data[line_num],inputRead); //�ش� index�� ����ü�� input_line_content������ �о�� ���� �־���
        line_num++; //index �ϳ� ���� (���� ���� ���)

    }

    
    fclose(file);
    return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str)
{
    char* read_line = str; //parsing�� ���
    char* ptr = { 0, }; //tab������ �߶� ������ ����, 0���� �ʱ�ȭ 
    int count = 0; //�� ��° ��ū���� �־����� üũ
  
    // ���� �Ҵ� token_table ���� ������ ���� �Ҵ�
    token_table[token_line] = (char*)calloc(MAX_INST, sizeof(char));
    
    ptr = strtok(read_line, "\t"); //tab �������� �߶� token�� ptr������ ����
    if (strlen(ptr) <= 3 && strcmp(ptr," ") != 0) {
        char* temp = ptr;
        strtok(temp, "\n");
        token_table[token_line]->label = temp; //ù token�� Label�̴�.
    }
    else
        token_table[token_line]->label = ptr; //ù token�� Label�̴�.
    count++; //ù token�� �־������� count�� 1 ������
   
    while (ptr != NULL) { //�ش� ��ū�� NULL�� �ƴϸ� ��� �ݺ� (NULL�̸� ���� �� �ǹ�)
        
        
        ptr = strtok(NULL, "\t");

        if (ptr != NULL) {

            switch (count) {
            case 1:
                token_table[token_line]->operator = ptr; //count == 1�̸� operator ���� ����
                count++;
                break;
            case 2:
                strtok(ptr, "\n");
                token_table[token_line]->operand = ptr; //count == 2�̸� operand ���� ����
                count++;
                break;
            case 3:
                token_table[token_line]->comment = ptr; //coujnt == 3�̸� comment ���� ����
                count++;
                break;
            default:
                break;
            }
        }
    }

    //�� �� ��ū �Ľ� �������� line index + 1
    token_line++;


    return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
 * �Ű� : ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
 * ���� :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
    //�˻� �� operator name
    char* find_opStr = str;

    //Opcodeã������ inst table�� name�� token table�� operator �� �¾ƾ� ��
    //����1. Operator�� �� ���� "+" �� ���� ������ + ���� �ޱ��ں��� �̸����� ����
    //����2. Operator �� Assempler directives�� ��ȯ�� ������ ó��

    if (find_opStr == NULL)
        return 99;

    if (find_opStr[0] == 43) { //"+"�� ASCII code 43�� ��
       char* temp = find_opStr; //+ �ޱ��ں��� ������ ����
       int i = 1;
       while (temp[i] != NULL) {
           temp[i-1] = temp[i];
           i++;
       }
       temp[i - 1] = NULL;
       find_opStr = temp;
    }

    
    int count = 0;
    int result = 0;

    //opcode ã��
    for (int i = 0; i < inst_index; i++) {
        result = strcmp(find_opStr, inst_table[i]->name);
        if (result == 0) {
            return inst_table[i]->opcode;
        }
    }

    //�� ���Ƶ� �� ã�� ��� 99��ȯ
    return 99;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
    /* input_table�� ���ڿ��� ���پ� �Է� �޾Ƽ� */
    /* token_parsing()�� ȣ���Ͽ� token_unit�� ���� */

    token_line = 0; // �ʱ�ȭ
    
    // token_table ���� �Ҵ�
    *token_table = (token*)calloc(MAX_INST, sizeof(token));

    while (input_data[token_line] != NULL) {
        
        //������ �ҽ��ڵ� ������ parsing �Ͽ� token_table�� �ۼ�
        token_parsing(input_data[token_line]);

    }


    return 0;
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char* file_name)
{
    FILE* file = fopen(file_name, "w");

    int index = 0; // �ʱ�ȭ
    int count = token_line;
 
    while (count) {


        char* write_data1 = token_table[index]->label;
        char* write_data2 = token_table[index]->operator;
        char* write_data3 = token_table[index]->operand;
        char* write_data4;
        char* tab = "\t";
        
        //opcode ã�Ƽ� �־��ֱ�
        int find_opcode = search_opcode(write_data2); 
        if (find_opcode == 99) //��ã���� ���
            write_data4 = "\t";
        else
            write_data4 = find_opcode;

        //NULL ���ڴ� tab���� ��ü�ؼ� ����ϵ��� ��
        if (write_data2 == NULL)
            write_data2 = "\t";
        if (write_data3 == NULL)
            write_data3 = "\t";
        if (write_data4 == NULL)
            write_data4 = "\t";

        fprintf(file, "%s\t%s\t%s\t\t\t%s\n", write_data1,write_data2,write_data3,write_data4);
        index++; //�ҽ��ڵ� ������ �� ��������� index + 1�ؼ� ���� �ҽ��ڵ��
        count--;
    }

    fclose(file);

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
    /* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char* filen_ame)
{
    /* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    return 0;
    /* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
    /* add your code here */
}
