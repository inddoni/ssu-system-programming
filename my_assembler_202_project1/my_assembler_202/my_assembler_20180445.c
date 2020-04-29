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

    
    make_symtab_output("symtab_20180445.txt");

    make_literaltab_output("literaltab_20180445.txt");

    
    if(assem_pass2() < 0 ){
        printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ;
        return -1 ;
    }

    ////object code ���
    make_objectcode_output("object_20180445.txt") ;
    

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
    if (strlen(ptr) <= 2 && strcmp(ptr," ") != 0) {
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
                if (strcmp(ptr, "CSECT\n") == 0 || strcmp(ptr, "LTORG\n") == 0)
                    strtok(ptr, "\n");
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
    char find_opStr[10] = { 0, };
    if(str)
        strcpy(find_opStr, str);

    //Opcodeã������ inst table�� name�� token table�� operator �� �¾ƾ� ��
    //����1. Operator�� �� ���� "+" �� ���� ������ + ���� �ޱ��ں��� �̸����� ����
    //����2. Operator �� Assempler directives�� ��ȯ�� ������ ó��

    if (find_opStr == NULL)
        return 99;

    if (find_opStr[0] == 43) { //"+"�� ASCII code 43�� ��
       char temp[10] = { 0, };
       strcpy(temp, find_opStr); //+ �ޱ��ں��� ������ ����
       int i = 1;
       while (temp[i] != NULL) {
           temp[i-1] = temp[i];
           i++;
       }
       temp[i - 1] = NULL;
       strcpy(find_opStr, temp);
    }

    
    int count = 0;
    int result = 0;

    //opcode ã��
    for (int i = 0; i < inst_index; i++) {
        result = strcmp(find_opStr, inst_table[i]->name);
        if (result == 0) {
            return strtol(inst_table[i]->opcode, NULL, 16);
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

    
    //�ҽ��ڵ� ���κ��� Address �Ҵ�
    address_define();

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

 
    for(int i = 0; i < token_line; i ++) {


        char* write_data1 = token_table[i]->label;
        char* write_data2 = token_table[i]->operator;
        char* write_data3 = token_table[i]->operand;
        int write_data4;
        char* tab = "\t";
        
        //opcode ã�Ƽ� �־��ֱ�
        int find_opcode = search_opcode(write_data2); 
        if (find_opcode == 99) //��ã���� ���
            write_data4 = 99;
        else
            write_data4 = find_opcode;

        //NULL ���ڴ� tab���� ��ü�ؼ� ����ϵ��� ��
        if (write_data2 == NULL)
            write_data2 = "\t";
        if (write_data3 == NULL)
            write_data3 = "\t";
        
        if (write_data4 == 99)
            fprintf(file, "%s\t%s\t%s\t\n", write_data1, write_data2, write_data3);
        else
            fprintf(file, "%s\t%s\t%s\t\t\t%02x\n", write_data1,write_data2,write_data3,write_data4);
    }

    fclose(file);

}

/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : 10������ ���ڸ� �Է¹޾� 16������ ��ȯ�ϴ� �Լ�
* �Ű� : (16������ ��ȯ��)10������ ����
* ��ȯ : 16������ ��ȯ�� �����
*  ----------------------------------------------------------------------------------
*/
int dtohex(int value) {

    int dec = value;
    char hex[10] = { 0, };
    char hex_result[10] = { 0, };
    int result = 0;
    int index = 0;
    
    while (1) {
        int mod = dec % 16;
        if (mod < 10)
            hex[index] = 48 + mod; //���� 0�� ASCII�ڵ尪 48 + ������
        else
            hex[index] = 65 + (mod - 10); //�빮�� A�� ASCII�ڵ尪 65 + ������

        dec = dec / 16;
        index++;

        if (dec == 0)
            break;
    }

    int j = 0;
    for (int i = index - 1; i >= 0; i--) {
        hex_result[j] = hex[i];
        j++;
    }

    result = atoi(hex_result);

    return result;
}

/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : symbol name�� �Ű��� ȣ���ϸ� �ش��ϴ� address�� ã�� ��ȯ���ִ� �Լ�
* �Ű� : symbol name
* ��ȯ : �ش� symbol�� address
*  ----------------------------------------------------------------------------------
*/
int search_symbol(char* name) {

    for (int i = 0; i < token_line; i++) {
        if (strcmp(sym_table[i].symbol, "") != 0) { //NULL�� �ƴϸ�
            if (strcmp(name, sym_table[i].symbol) == 0) {
                return address[i]; //ã���� �ּҰ� ��ȯ
            }
        }
    }

    return 99999; //��ã������ 99999��ȯ
}
/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : ���α׷� ��ȣ�� symbol name�� �Ű��� ȣ���ϸ� �ش��ϴ� address�� ã�� 
         �ش� ���α׷��� ���� symbol�� address�� ��ȯ���ִ� �Լ�
* �Ű� : symbol name, program no
* ��ȯ : �ش� symbol�� address
*  ----------------------------------------------------------------------------------
*/
int search_symbol_p(char* name, int p) {

    if (p == 0) {
        for (int i = 0; i < 23; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL�� �ƴϸ�
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //ã���� �ּҰ� ��ȯ
                }
            }
        }
    }
    else if (p == 1) {
        for (int i = 24; i < 44; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL�� �ƴϸ�
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //ã���� �ּҰ� ��ȯ
                }
            }
        }
    }
    else { //p == 3�̸�
        for (int i = 47; i < token_line; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL�� �ƴϸ�
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //ã���� �ּҰ� ��ȯ
                }
            }
        }
    }

    return 99999; //��ã������ 99999��ȯ
}

/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : symbol name�� �Ű��� ȣ���ϸ� �ش��ϴ� address�� ã�� ��ȯ���ִ� �Լ�
* �Ű� : symbol name
* ��ȯ : �ش� symbol�� address
*  ----------------------------------------------------------------------------------
*/
int search_literal(char* name) {
    for (int i = 0; i < token_line; i++){
        if (literal_table[i].literal != NULL) {
            if (strcmp(name, literal_table[i].literal) == 0) {
                return literal_table[i].addr; //ã���� �ּҰ� ��ȯ
            }
        }
    }

    return 99999; //��ã������ 99999��ȯ
}

/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : ��ɾ� ���ο� �´� Address ���� token_table�� sourceAddr ������ �����ϴ� 
*        �Լ��̴�. inst_table�� format�� token_table�� operator�� Ȱ���Ͽ� ����Ѵ�.
         address[MAX_LINE]�� ���� �ּ� ���� �����Ѵ�.
         sys_table�� ���� ���Ѵ�.
* �Ű� : ����
* ��ȯ : ����
*  ----------------------------------------------------------------------------------
*/
void address_define(void) {

    
    locctr = 0; //������ ���θ��� �Էµ� �ּҰ�
    int litCount = 0;
    int proIndex = 0; //������ ���� ���α׷��� �ε���

    /* operator start�� 0
    * operator�� CSECT�� �ּҰ� 0���� �ʱ�ȭ
    * operator�� EXTDEF�� EXTREF�� END�� �ּҰ� NULL
    * operator�� �� ���� "+"�̸� +4
    * operator�� �� ���� "="�̸� operand �� ���� C���� X���� Ȯ���ؼ� +3 Ȥ�� +1
    * operator�� RESW�̰ų� WORD�̸� �ش� operand�� �� * 3 �ؼ� �����ֱ�
    * operator�� BYTE�� operand�� �� * 1 �ؼ� �����ֱ�
    * operator�� LTORG�� �ּҰ� ���ϱ� 0 (���߿� ��¾ȵǰ� �ϱ�)
    * =============================================================================
    * ���� ��쿡 �ش����� ������ inst_table�� oprator�� �ش��ϴ� format�� ã�Ƽ� +3 Ȥ�� +2 ���ֱ� */

    for (int i = 0; i < token_line; i++) { //�ҽ��ڵ� ��� ���ο� �ݺ�
        

        char* oper = token_table[i]->operator;
        char* lab = token_table[i]->label;

        if (oper != NULL || strcmp(lab, "END") == 0) {
            
            address[i] = locctr;
           

            //operator�� START�̸� �ּҰ��� 0���� �ʱ�ȭ
            if (strcmp(oper, "START") == 0) {
                locctr = 0;
            }
            //operator�� EXTDEF�� EXTREF�� �ּҰ� NULL
            else if (strcmp(oper, "EXTDEF") == 0 || strcmp(oper, "EXTREF") == 0) {
                locctr = 0;
            }
            //operator�� �� ���� "+"�̸� +4
            else if (oper[0] == '+') {
                locctr += 4;
                //printf("%s\t%s\t%x\t \n", lab, oper, address[i]);
                //continue;
            }
            //operator�� �� ���� "="�̸� operand �� ���� C���� X���� Ȯ���ؼ� +3 Ȥ�� +1
            else if (oper[0] == '=') {
                if (oper[1] == "C") 
                    locctr += 3;
                else 
                    locctr += 1;
            }
            //operator�� RESW�̰ų� WORD�̸� �ش� operand�� �� * 3 �ؼ� �����ֱ�
            else if (strcmp(oper, "RESW") == 0 || strcmp(oper, "WORD") == 0) {
                if ((strcmp(oper, "RESW") == 0)) {
                    int num = atoi(token_table[i]->operand);
                    locctr += (3 * num);
                }
                else { //WORD�϶��� �׳� +3�� ���ֱ�
                    locctr += 3;
                }
            }
            //operator�� BYTE�� operand�� �� * 1 �ؼ� �����ֱ�
            else if (strcmp(oper, "RESB") == 0 || strcmp(oper, "BYTE") == 0) {
                if (strcmp(oper, "RESB") == 0) {
                    int num = atoi(token_table[i]->operand);
                    locctr += num;
                }
                else { //BYTE�϶��� �׳� +1�� ���ֱ�
                    locctr += 1;
                }
            }
            //operator�� LTORG�� �ּҰ� ���ϱ� 0 
            else if (strcmp(oper, "LTORG") == 0 || strcmp(lab, ".") == 0) {
                if (strcmp(oper, "LTORG") == 0) {
                    locctr += litCount;
                    litCount = 0; //�׵��� ���ͷ��� ���ͼ� ī��Ʈ �س��� ���� �־��ְ� 0���� �ʱ�ȭ
                }
            }
            //EQU�̸� operand ���� ��� �� ���簪(index-1) �� ���� ���ֱ� (operand�� * �̸� ���簪 ���� ����)
            else if (strcmp(oper, "EQU") == 0) {
                //bufend-buffer ��� ����� �־��ֱ�
                int value1 = 0;
                int value2 = 0;

                if (strcmp(token_table[i]->operand, "*") != 0) {
                    char* ptr = token_table[i]->operand;
                    ptr = strtok(ptr, "-");
                    value1 = search_symbol(ptr);
                                        
                    if (ptr != NULL) {
                        ptr = strtok(NULL, "-");
                        value2 = search_symbol(ptr);
                    }

                    //�ΰ� �� ���� address[i] ������ �ֱ�
                    address[i] = abs(value1 - value2);
                }
            }
            //���� �����ų� ���α׷��� ������ ���� ���
            else if (strcmp(oper, "CSECT") == 0 || strcmp(lab, "END") == 0) {
                programLength[proIndex] = locctr;
                //printf("program Length : %x\n", programLength[proIndex]);
                proIndex++;
                locctr = 0;
            }

            //operator�� ���� inst_table���� format�� ã�Ƽ� +3 �Ǵ� +2 ���ֱ�
            else {
                if(search_format(oper) != 99)
                    locctr += search_format(oper);       
            }

            //operand���� ���ͷ��� ������ count���ֱ�
            if (token_table[i]->operand && token_table[i]->operand[0] == '=') {
                if (token_table[i]->operand[1] == 'C')
                    litCount += 3;
                else
                    litCount += 1;
            }
            //printf("%s\t%s\t%x\t \n", lab, oper, address[i]);
        }
        
        //label�� ������ symbol table�� ����
        if (strcmp(lab, " ") != 0 && strcmp(lab, ".") != 0 && strcmp(lab, "END") != 0) {
            strcpy(sym_table[i].symbol, lab);

            if (strcmp(lab, "RDREC") == 0 || strcmp(lab, "WRREC") == 0)
                sym_table[i].addr = 0;
            else
                sym_table[i].addr = address[i];
            //printf("%s\t%x\n", sym_table[i].symbol, sym_table[i].addr);
        }

    }
}


/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : ��ɾ� ������ ���� opcode�� nixbpe�� �����ؼ� ���̺� �־���
* �Ű� : ����
* ��ȯ : ����
*  ----------------------------------------------------------------------------------
*/
void nixbpe_define(void) {
    // label�� +�ΰ�, #�ΰ�, @�ΰ� ���� �� �����ֱ�
    // operator�� RSUB�� ��
    // operend�� , �� �ִ� �� (+�ִ��� 2�� üũ)
    // �������� �⺻ SIC/XE nixbpe ����

    for (int i = 0; i < token_line; i++) {
        char* lab = token_table[i]->label;
        char* oper = token_table[i]->operator;
        char* operand = token_table[i]->operand;
        char* xCheck = NULL;
        int opcode = search_opcode(oper);

        if(operand)
            xCheck = strstr(operand, ",X");
       
        if (opcode != 99) {
            if (xCheck != NULL) {
                if (oper[0] == '+')
                    token_table[i]->nixbpe = 39; //16���� 39   2���� 111001 10����
                else
                    token_table[i]->nixbpe = 38; //16���� 38   2���� 111000
            }
            else if (oper[0] == '+') {
                token_table[i]->nixbpe = 31; //16���� 31   2���� 110001
            }
            else if (operand[0] == '#') {
                token_table[i]->nixbpe = 10; //16���� 10   2���� 010000
            }
            else if (operand[0] == '@') {
                token_table[i]->nixbpe = 22; //16���� 22   2���� 100010
            }
            else if (strcmp(oper, "RSUB") == 0) {
                token_table[i]->nixbpe = 30; //16���� 30   2���� 110000
            }
            else
                token_table[i]->nixbpe = 32; //16���� 32   2���� 110010
            
            printf("%d\n", token_table[i]->nixbpe);
        }
        

        //printf("%s\t%s\t%d\n", operators, operand, token_table[i]->nixbpe);
    }

}
/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : ���� ���α׷��� EXTREF�� �ش��ϴ��� �Ǻ����ִ� �Լ�
* �Ű� : ���α׷� ��ȣ(�� ��° ���α׷�����)��, operand name
* ��ȯ : ������ 1, �ƴϸ� 0 ��ȯ
*  ----------------------------------------------------------------------------------
*/
void setExtref(void) {

    int count = -1; //���α׷� ��ȣ
    char* ptr = { 0, };
    char temp[30] = { 0, };
    int k = 0;
    for (int i = 0; i < token_line; i++) {
        if (token_table[i]->operator) {
            if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                count++;
                if (token_table[i]->operand) {
                    switch (count) {
                    case 0:
                        strcpy(temp, token_table[i]->operand);
                        ptr = strtok(temp, ",");
                        while (ptr != NULL) {
                            first_extref[k] = malloc(sizeof(char) * 10);
                            strcpy(first_extref[k], ptr);
                            k++;

                            ptr = strtok(NULL, ",");
                        }
                        k = 0;
                        break;
                    case 1:
                        strcpy(temp, token_table[i]->operand);
                        ptr = strtok(temp, ",");
                        while (ptr != NULL) {
                            second_extref[k] = malloc(sizeof(char) * 10);
                            strcpy(second_extref[k], ptr);
                            k++;

                            ptr = strtok(NULL, ",");
                        }
                        k = 0;
                        break;
                    case 2:
                        strcpy(temp, token_table[i]->operand);
                        ptr = strtok(temp, ",");
                        while (ptr != NULL) {
                            third_extref[k] = malloc(sizeof(char) * 10);
                            strcpy(third_extref[k], ptr);
                            k++;

                            ptr = strtok(NULL, ",");
                        }
                        k = 0;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }

}
/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : ���� ���α׷��� EXTREF�� �ش��ϴ��� �Ǻ����ִ� �Լ�
* �Ű� : ���α׷� ��ȣ(�� ��° ���α׷�����)��, operand name
* ��ȯ : ������ 1, �ƴϸ� 0 ��ȯ
*  ----------------------------------------------------------------------------------
*/
int search_extref(int programNum, char* name) {
    
    int count = programNum; //���α׷� ��ȣ
    int k = 0;
    int result = 0;
    switch(count) {
        case 0 :
            while (first_extref[k]) {
                if (strcmp(first_extref[k], name) == 0) {
                    result = 1;
                    break;
                }
                k++;
            }
            break;
        case 1 :
            while (second_extref[k]) {
                if (strcmp(second_extref[k], name) == 0) {
                    result = 1;
                    break;
                }
                k++;
            }
            break;
        case 2 :
            while (third_extref[k]) {
                if (strcmp(third_extref[k], name) == 0) {
                    result = 1;
                    break;
                }
                k++;
            }
            break;
        default :
            break;
    }
    
    return result;
}

/* ----------------------------------------------------------------------------------
* ���� ������ �Լ�
* ���� : �ش� operator�� format�� ã�� ��ȯ���ִ� �Լ�
* �Ű� : format�� ã������ operaotor name
* ��ȯ : opertor�� format(3, 2 ��), �����ڵ� 99
*  ----------------------------------------------------------------------------------
*/
int search_format(char* name) {

    //�˻� �� operator name
    char find_name[10] = { 0, };
    if (name)
        strcpy(find_name, name);

    //����. Operator�� �� ���� "+" �� ���� ������ + ���� �ޱ��ں��� �̸����� ����

    if (find_name == NULL)
        return 99;

    if (find_name[0] == 43) { //"+"�� ASCII code 43�� ��
        char temp[10] = { 0, };
        strcpy(temp, find_name); //+ �ޱ��ں��� ������ ����
        int i = 1;
        while (temp[i] != NULL) {
            temp[i - 1] = temp[i];
            i++;
        }
        temp[i - 1] = NULL;
        strcpy(find_name, temp);
    }

    for (int j = 0; j < inst_index; j++) {
        if (strcmp(find_name, inst_table[j]->name) == 0) {
            if (strcmp(inst_table[j]->format, "3/4") == 0)
                return 3;
            else if (strcmp(inst_table[j]->format, "2") == 0)
                return 2;
            else
                return 1;
        }
    }

    return 99;
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
    /* Symbol Table �� 16������ ��� */

    FILE* file = fopen(file_name, "w");

    for (int i = 0; i < token_line; i++) {
        if (strcmp(sym_table[i].symbol, "") != 0) {
            fprintf(file, "%s\t%x\n", sym_table[i].symbol, sym_table[i].addr);
        }
    }

    fclose(file);
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
void make_literaltab_output(char* file_name)
{
    /* Literal Table �� 16������ ��� */

    //�ҽ��ڵ��� operand�� ��ĵ�ϸ鼭 [0]���� ���ڰ� = �̸� ���̺� �߰�
    //�ҽ��ڵ��� operator�� ��ĵ�ϸ鼭 LTORG�� ������ ���ͷ����̺� �ִ� =�� ���� �ּҰ��� �־������ => �ּ��Լ����� ����
   
    FILE* file = fopen(file_name, "w");
   
    int start = 0;
    int index = -1;
    for (int i = 0; i < token_line; i++) {
        
        char* operand = token_table[i]->operand;

        // 1. ���鼭 operand '='�� �� ���̺� �ֱ� 
        if (operand) {
            if (operand[0] == '=') {

                //������ ���� �̹� ���̺� ����ִ��� Ȯ�� (������ check = 1)
                int check = 0;
                for (int k = 0; k < i; k++) {
                    if (strcmp(literal_table[k].literal, operand) == 0) {
                        check = 1;
                        break;
                    }
                }

                //���ͷ� ���̺� �� �ֱ�
                if (check != 1) {
                    strcpy(literal_table[i].literal, operand);
                    continue;
                }
            }
        }
     
        if (token_table[i]->operator) {

            if (strcmp(token_table[i]->operator, "LTORG") == 0) {
                index++;
                for (index; index < i; index++) { //���� ���α��� ���� ���ͷ����� ����
                    if (strcmp(literal_table[index].literal,"") != 0) {
                        strcpy(literal_table[i].literal, literal_table[index].literal);
                        strcpy(literal_table[index].literal, "");
                        literal_table[i].addr = address[i];
                    }
                }
            }

            else if (strcmp(token_table[i]->label, "END") == 0) {
                index++;
                for (index; index < i; index++) { //���� ���α��� ���� ���ͷ����� ����
                    if (strcmp(literal_table[index].literal, "") != 0) {
                        strcpy(literal_table[i].literal, literal_table[index].literal);
                        strcpy(literal_table[index].literal, "");
                        literal_table[i].addr = address[i];
                    }
                }
                //END�϶� �־��ָ� ���α׷� ���̵� �÷��ֱ�
                if (strcmp(token_table[i]->label, "END") == 0) {
                    if (literal_table[i].literal[1] == 'X')
                        programLength[2] += 1;
                    else
                        programLength[2] += 3;
                }
            }
        }

        
    }

    //�ؽ�Ʈ���Ͽ� literal table ����
    for (int i = 0; i < token_line; i++) {

        if (strcmp(literal_table[i].literal, "") != 0) {
            //printf("%s\t%x\n", literal_table[i].literal, literal_table[i].addr);
            //���ͷ� ���̺� ���Ͽ� �� ���� ���ͷ��� �����Ѵ�. (while������)
            int index = 0; //lit�迭�ȿ� �־��� ���� �ε���
            int test = 3;
            char litPrint[10] = { 0, };
            while (literal_table[i].literal[test] != 39) { //��������ǥ�� �ƴϸ�
                litPrint[index] = literal_table[i].literal[test];
                test++; index++;
            }
            fprintf(file, "%s\t%x\n", litPrint, literal_table[i].addr);
        }
        
    }


    fclose(file);
}


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
  
    nixbpe_define();
    setExtref(); //���������� ���

    int count = -1; // �� ��° ���α׷����� ����
    int opcodeF = 0;
    int operandAddr = 0;
    int upper = 0;
    int lower = 0;
    int nixbpe = 0;
    for (int i = 0; i < token_line; i++) {

        char* operand = token_table[i]->operand;

        if (strcmp(token_table[i]->label, ".") != 0) { //comment line�� �ƴϸ�

            if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTREF") == 0) { //���° ���α׷����� �������ֱ�
                count++; //count��° ���α׷��̴�.
            }

            // 1. opcode������ ã��
            opcodeF = search_opcode(token_table[i]->operator);
            if (opcodeF != 99) { //opcode�� ã������

                //format 2�ڸ��� ���� ó�����ְ� continue ��Ű��
                if (search_format(token_table[i]->operator) != 99) { //operator�� �´� ������ ������
                    if (search_format(token_table[i]->operator) == 2) { //format �� 2���� Ȯ���ϱ�
                        char* ptr = { 0, };
                        ptr = strtok(token_table[i]->operand, ",");
                        int mult = 16;
                        while (ptr != NULL) {
                            if (strcmp(ptr, "X") == 0)
                                operandAddr = 1;
                            else if (strcmp(ptr, "A") == 0)
                                operandAddr = 0;
                            else if (strcmp(ptr, "S") == 0)
                                operandAddr = 4;
                            else // T�� ���
                                operandAddr = 5;

                            lower = operandAddr * mult;
                            mult /= 16;
                            ptr = strtok(NULL, ",");
                        }

                        upper = opcodeF * 16 * 16;
                        objectCode[i] = upper + lower;
                        printf("object code : %x\n", objectCode[i]); //���߿� �����!!!!!!!!!!!!!!!!!!!!!!
                        continue;
                    }
                }

                // 1. opcode�� ������ symbol table���� operand�� �ּ�ã�Ƽ� �������� (=TA)
                // ����)) operand�� extref�� �ִ� �̸��̸� 0���� ��������
                char temp[20] = { 0, };
                strcpy(temp, operand);
                char* ptr = strtok(temp, ",");
                if (search_extref(count, ptr) == 1) {
                    operandAddr = 0;
                    readCheck[i] = 1;
                }
                else {
                    if (operand[0] == '@') {
                        int idx = 1;
                        while (operand[idx] != NULL) { //@���� �ޱ��ں��� Operand �̸� �ٽ�����
                            operand[idx - 1] = operand[idx];
                            idx++;
                        }
                        operand[i - 1] = NULL;
                        operandAddr = search_symbol_p(operand, count);
                    }
                    else if (strstr(operand, ",") != NULL) {
                        char temp[30] = { 0, };
                        char* ptr = { 0, };
                        strcpy(temp, operand);
                        ptr = strtok(temp, ",");
                        if (search_extref(count, ptr) == 1)
                            operandAddr = 0; //���������� ������ �ּҴ� 0���� ó��
                        else
                            operandAddr = search_symbol_p(ptr, count);
                    }
                    else
                        operandAddr = search_symbol_p(operand, count);
                }
                
                // ������� ������ ��� opcode�� �ִ� ���ε��� �ɺ����̺� ��Ī�� �ּ� Ȥ�� 99999�� ���� ���� ��


                // '+' operator ó��
                if (token_table[i]->operator[0] == '+') {
                    opcodeF = opcodeF * 16; //480
                    nixbpe = token_table[i]->nixbpe;
                    upper = opcodeF + ((nixbpe / 10) * 16) + (nixbpe % 10);
                    upper = upper * 16 * 16 * 16 * 16 * 16; //5�ڸ� up
                    lower = operandAddr;
                    objectCode[i] = upper + lower;
                    printf("object code : %x\n", objectCode[i]); //���߿� �����!!!!!!!!!!!!!!!!!!!!!!
                    continue;
                }
                    


                // �ɺ����̺� ���� �� ó��
                if (operandAddr == 99999) { //operand�� #���� =������ ���� ���

                    if (operand[0] == '=') { //���ͷ�
                        //���ͷ� ���̺� search
                        operandAddr = search_literal(operand); //10������ȯ��
                        lower = operandAddr - address[i + 1];
                        //lower = lower % 1000; //TA - PC ������� �� 3�ڸ� ����
                    }

                    else if (operand[0] == '#') { //���
                        operand[0] = '0'; // #�� 0���� �ٲ��� #3 -> 03 �̶�� ���ڿ��� ��
                        lower = atoi(operand);
                    }

                    else {
                        lower = 0;
                    }


                }
                else { // �ɺ����̺��� ã������

                    lower = operandAddr - address[i + 1]; //TA- PC 
                    //lower = lower % 1000;
                }

                nixbpe = token_table[i]->nixbpe;
                nixbpe = ((nixbpe / 10) * 16) + (nixbpe % 10); //nixbpe 10������ ��ȯ
                upper = opcodeF * 16 + nixbpe;
                upper = upper * 16 * 16 * 16;
                if (lower < 0) { //lower�� �����̸�
                    objectCode[i] = upper + lower + 4096;
                }
                else
                    objectCode[i] = upper + lower;
            }

            // 2. opcode�� �� ã������ operator�� WORD�� BYTE���� Ȯ��
            else if (strcmp(token_table[i]->operator, "WORD") == 0) {
                objectCode[i] = 000000; //�ڸ��� �Ҵ�
                readCheck[i] = 1;
            }

            else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
                // operand�� ���� ����ǥ �ȿ� ���� �о����
                int num = 0;
                int mult = 16;
                char byteF[3] = { 0, };
                for (int k = 2; k < 4; k++) {
                    byteF[k - 2] = operand[k]; //'F' ���� => 15
                    if(byteF[k-2] >= 48 && byteF[k-2] <= 57)
                        num += (byteF[k - 2] - 48) * mult;
                    else
                        num += (byteF[k - 2] - 55) * mult;
                    mult = mult / 16;
                }
                objectCode[i] = num;
            }

            //3. ���ͷ� ó���ϱ�
            else if (strcmp(token_table[i]->operator, "LTORG") == 0 || strcmp(token_table[i]->operator, "FIRST") == 0) {
                //�ش� �ּҰ��� �´� ���ͷ��� ASCII�ڵ�� ��ȯ�ؼ� �ֱ�
                //=C'EOF' = 454f46 / =X'05' = 05
                objectCode[i] = 0; //�ϴ� object code 0���� ����
                int num = 0;
                int mult = 16*16*16*16;
                if (literal_table[i].literal[1] == 'C') { // =C �̸�
                    char byteF[4] = { 0, };
                    for (int k = 3; k < 6; k++) { //3���� ĳ���͸� �о object code�� ����
                        byteF[k - 3] = literal_table[i].literal[k];
                        num += byteF[k - 3] * mult;
                        mult = (mult / 16)/16;
                    }
                    objectCode[i] = num;
                }
                else { // =X �̸�
                    int num = 0;
                    int mult = 16 * 16;
                    int calc = 0;
                    char byteF[3] = { 0, };
                    for (int k = 3; k < 5; k++) {
                        byteF[k - 3] = literal_table[i].literal[k];
                        if (byteF[k - 3] >= 48 && byteF[k - 3] <= 57)
                            calc = byteF[k - 3] - 48;
                        else
                            calc = byteF[k - 3] - 55;
                        num += calc * mult;
                        mult = (mult / 16) / 16;
                    }
                    objectCode[i] = num;
                }
            }

            else { //������ operator�� (START, EXTEDF, CSECT, EQU ��) 
                objectCode[i] = NULL;
            }
            printf("object code : %x\n", objectCode[i]);
        }
        
    }
    


    return 0;
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
    FILE* file = fopen(file_name, "w");
    int count = -1; //���° ���α׷�����
    int i = 0;
    char ref_temp[30] = { 0, };
    int isStart = 1;
    int modeStart = 0;
    int zero = 0;
    while (i < token_line) {

        if (strcmp(token_table[i]->label, ".") != 0) { //comment line�� �ƴϸ�

            // Header
            if (isStart == 1) { //���α׷� ���ۺκ��϶��� isStart == 1 ��
                if (strcmp(token_table[i]->label, "END") == 0)
                    break;
                
                count++; //���° ���α׷��� ���Դ��� ����
                fprintf(file, "H%s\t%06X%06X\n", token_table[i]->label, zero, programLength[count]);
                isStart = 0;
                i++;
                continue; //�� �� �Է������� i ���� �� while�� ó������ �ٽ� �ö�
            }

            //D����
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                // "," �������� �Ľ��ϸ鼭 �ɺ����̺��� ���α׷���ȣ�� �̸����� �ּ�ã�ƿ���
                fprintf(file, "D");
                char temp[30] = { 0, };
                strcpy(temp, token_table[i]->operand);
                char* ptr = strtok(temp, ",");
                int addF = 0;
                while (ptr != NULL) {
                    addF = search_symbol_p(ptr, count); //�ɺ����̺��� �ּҰ� ã�ƿ�
                    fprintf(file, "%s%06X", ptr, addF);
                    ptr = strtok(NULL, ",");
                }
                fprintf(file, "\n");
                i++;
                continue;  //�� �� �Է������� i ���� �� while�� ó������ �ٽ� �ö�
            }

            //Rerence����
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTREF") == 0) {
                // "," �������� �Ľ��ϸ鼭 �̸��� ���
                fprintf(file, "R");
                char temp[30] = { 0, };
                strcpy(temp, token_table[i]->operand);
                strcpy(ref_temp, token_table[i]->operand); //�ش� ���α׷��� M ���ο��� �� ���� �̸� ����
                char* ptr = strtok(temp, ",");
                while (ptr != NULL) {
                    fprintf(file, "%s ", ptr);
                    ptr = strtok(NULL, ",");
                }
                fprintf(file, "\n");
                i++;
                continue;  //�� �� �Է������� i ���� �� while�� ó������ �ٽ� �ö�
            }

            // Text ó�� �� ������Ʈ �ڵ带 ����� �� �ʿ���� ���۷���� �ѱ��
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "RESB") == 0 || strcmp(token_table[i]->operator, "EQU") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) {
                i++;
                continue;
            }

            

            //���� �ش���� �����鼭, operator�� �ִ� �ֵ��� text ���ο� �� ��
            else if (token_table[i]->operator) { 
                while (strcmp(token_table[i]->operator,"CSECT") != 0 && strcmp(token_table[i]->operator,"FIRST") != 0) { //�ش� ���α׷��� ���� �ƴϸ� ��� �ݺ�
                    int line_length = 0;
                    char pr[10] = { 0, };
                    char prTotal[150] = { 0, };
                    int startAddr = address[i];

                    while (line_length < 29 && strcmp(token_table[i]->operator,"CSECT") != 0) { // �� ���� �ִ� ���� ������ �ݺ�
                        if (token_table[i]->operator && strcmp(token_table[i]->operator, "RESB") == 0 || strcmp(token_table[i]->operator, "EQU") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) {
                            i++;
                            break;
                        }
                        else if (token_table[i]->operator[0] == '+') {
                            line_length += 4;
                            sprintf(pr, "%08X", objectCode[i]);
                            i++;
                        }
                        else if (search_format(token_table[i]->operator) == 2) {
                            line_length += 2;
                            sprintf(pr, "%04X", objectCode[i]);
                            i++;
                        }
                        else if (strcmp(token_table[i]->operator,"BYTE") == 0) {
                            line_length += 1;
                            sprintf(pr, "%02X", objectCode[i]);
                            i++;
                        }
                        else if (strcmp(token_table[i]->operator,"WORD") == 0) {
                            line_length += 3;
                            sprintf(pr, "%06X", objectCode[i]);
                            i++;
                        }
                        else if (strcmp(token_table[i]->operator, "FIRST") == 0) {
                            line_length += 1;
                            sprintf(pr, "%02X", objectCode[i]);
                            strcat(prTotal, pr); // �� ���ο� �� �ڵ� ���̱�
                            break;
                            //���α׷� ���� ���� ����
                        }
                        else {
                            line_length += 3;
                            sprintf(pr, "%06X", objectCode[i]);
                            i++;
                        }
                        strcat(prTotal, pr); // �� ���ο� �� �ڵ� ���̱�
                    }

                    // �� ������ �Է��� ������
                    if (line_length != 0) { // �Էµ� ���ڰ� �ִ��� Ȯ��
                        fprintf(file, "T%06X%02X%s\n", startAddr, line_length, prTotal); //�����ּ�, �ش� ���� �� ����
                    }

                }

                // Csect ������(���α׷��� ��) �ݺ��� �������� : M���� ó���ϰ�, i++�ϰ� while�� ó��(����)���� ���� �ؾ���
                // M ���� ó��
                for (int j = modeStart; j < i; j++) { //�� ���α׷� ���� �ȿ��� ���� �ݺ���
                    if (readCheck[j] == 1) {
                        int locat = 0; // ��ɾ� �ڵ� ������ �ڸ���
                        int addr = 0;
                        if (token_table[j]->operator[0] == '+') {
                            locat = 5;
                            addr = address[j] + 1;
                        }
                        else {
                            locat = 6;
                            addr = address[j]; //�״��
                        }

                        char printName[15] = { 0, };
                        char* ptr = strstr(token_table[j]->operand, "-");
                        if (ptr == NULL) {
                            ptr = strtok(token_table[j]->operand, ",");
                            fprintf(file, "M%06X%02X+%s\n", addr, locat, ptr);
                        }
                        else {
                            char* ptr2 = strtok(token_table[j]->operand, "-");
                            fprintf(file, "M%06X%02X+%s\n", addr, locat, ptr2);
                            while(ptr2 != NULL){
                                ptr2 = strtok(NULL, "-");
                                if(ptr2 != NULL)
                                    fprintf(file, "M%06X%02X-%s\n", addr, locat, ptr2);
                            }
                            
                        }
                    }
                    else
                        continue;
                }

                modeStart = i; //���� ���α׷����� ã�� ���� �� ����


                // m���� ó�� ���� �� E���� ����ϰ� i++�ϰ� while�� ó������!
                if (count == 0) {
                    fprintf(file, "E%06X\n\n", 0);
                }
                else
                    fprintf(file, "E\n\n");
                
                if (count < 3) 
                    isStart = 1;

                //i++; // 1 ���� �Ƚ�Ŵ!! ���α׷� �̸��κк��� �����Ϸ��� 
                continue;

            }

            else {
                i++;
                continue;
            }
            

        }

        else { //label == "." �̸� ���� ���� �е��� �ε��� ���������ֱ�
            i++;
        }
    }

    fclose(file);

}
