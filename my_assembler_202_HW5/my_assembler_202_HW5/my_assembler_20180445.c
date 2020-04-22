/*
 * 화일명 : my_assembler_20180445.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

 /*
  *
  * 프로그램의 헤더를 정의한다.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

  // 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20180445.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일
 * 반환 : 성공 = 0, 실패 = < 0
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
 *		   또한 중간파일을 생성하지 않는다.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[])
{
    //inst_tablel과 input_data(소스코드 테이블)을 만든다.
    if (init_my_assembler() < 0)
    {
        printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
        return -1;
    }

    if (assem_pass1() < 0)
    {
        printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
        return -1;
    }
    
    make_opcode_output("output_20180445.txt");

    /*
    * 추후 프로젝트에서 사용되는 부분

    make_symtab_output("symtab_20180445");
    if(assem_pass2() < 0 ){
        printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ;
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
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
    int result;

    // inst.txt를 읽어 inst_table을 만든다.
    if ((result = init_inst_file("inst.txt")) < 0)
        return -1;
 
    // input.txt 파일을 한줄씩 읽어 소스코드 테이블(input_table)을 만든다.
    if ((result = init_input_file("input.txt")) < 0)
        return -1;

    return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
 *        생성하는 함수이다.
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */

int init_inst_file(char* inst_file)
{
    FILE* file;
    int errno;

    errno = 0;
    file = fopen(inst_file, "r"); //파일 가져오기

    if (file == NULL) { //파일 포인터 예외처리
        printf("실패 - 종료\n");
    }
    char instRead[200] = { 0, }; //한 줄씩 읽어 저장할 변수
    char* ptr = { 0, }; //한 줄씩 읽어온 문장을 tab 단위로 잘라서 저장할 포인터 변수
    inst_index = 0; //몇 번째 명령어인지 순서 매겨줄 index 번호 
    int count = 0; //해당 줄에서 몇 번째 token 인지 구분할 index

     // inst_table 동적 할당
    *inst_table = (inst*)calloc(100, sizeof(inst));


    while (!feof(file)) { //파일의 내용이 끝날 때까지 반복하여 한 줄씩 읽어오도록 함
 
       // inst_table 내부 동작 할당
        inst_table[inst_index] = (char*)calloc(100, sizeof(char));
       
        fgets(instRead, 200, file); //명령어 파일을 한 줄씩 읽어서 instRead 변수에 저장한다.
        ptr = strtok(instRead, "\t"); // 읽어온 줄을 tab 기준으로 잘라서 첫 번째 token을 ptr에 저장
        inst_table[inst_index]->mnenmonic = malloc(200 * sizeof(char));
        strcpy(inst_table[inst_index]->mnenmonic, ptr); //구조체 해당 index에 mnenmonic 변수안에 첫 번째 token을 저장
        count++; // table에 첫 번째 token을 넣었으면 카운트 + 1 (token을 넣을때마다 카운트를 증가해서 구분하도록 함)

        while (ptr != NULL) { //다음 token이 NULL이 아니면 계속 반복 (한 줄의 끝 == NULL)
           
            ptr = strtok(NULL, "\t"); // 읽어온 줄을 tab 기준으로 잘라서 token을 ptr에 저장
            
            switch (count) { //몇 번째 token인지 count값 확인
            case 1:
                inst_table[inst_index]->name = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->name, ptr); //count == 1이면 name을 넣을 차례
                count++; //name을 넣었으면 또 카운터를 1 증가해주기
                break;
            case 2:
                inst_table[inst_index]->format = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->format, ptr); //count == 2이면 format을 넣을 차례
                count++; //format을 넣었으면 또 카운터를 1 증가해주기
                break;
            case 3:
                inst_table[inst_index]->opcode = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->opcode, ptr); // count == 3이면 opcode를 넣을 차례
                count++; //operator을 넣었으면 또 카운터를 1 증가해주기
                break;
            case 4:
                inst_table[inst_index]->operand_unit = malloc(200 * sizeof(char));
                strcpy(inst_table[inst_index]->operand_unit, ptr); // count == 4 이면 operand_unit을 넣을 차례
                count++; //operand_unit을 넣었으면 또 카운터를 1 증가해주기
                break;
            case 5: // count == 5 면 operand_unit이 몇개인지 확인해서 넣어주기
                if (strcmp(inst_table[inst_index]->operand_unit, "0"))
                    break; // 0이면 넣을 것이 없으니까 아무것도 안하고 break
                else {
                    inst_table[inst_index]->operand1 = malloc(200 * sizeof(char));
                    strcpy(inst_table[inst_index]->operand1, ptr); //0이 아니면 일단 하나 넣어주기
                    count++; //operand1을 넣었으면 카운터를 1 증가해주기
                    break;
                }
            case 6: // count == 6이면 operand_unit이 2개인지 확인하기
                if ((inst_table[inst_index]->operand_unit) == "2") {
                    inst_table[inst_index]->operand2 = malloc(200 * sizeof(char));
                    strcpy(inst_table[inst_index]->operand2, ptr); //2이면 operand2에 넣어주기
                    break;
                }
                else //2가 아니면 아무것도 넣을 것이 없으니까 바로 break
                    break;
            default:
                printf("count = %d default error", count); //default일때 error 메세지 출력하도록 함
                break;
            }

        }

        //한 줄을 다 구조체에 입력했으면 
        count = 0; //count 다시 0으로 초기화
        ++inst_index; //inst_index 1 증가시켜줌 (다음 명령어라인 읽어들일 것)

    }

    fclose(file);
    return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
    FILE* file;
    int errno;

    errno = 0;
    file = fopen(input_file, "r");

    line_num = 0; //라인단위로 저장할 index
    char inputRead[200] = { 0, }; //한줄씩 읽어온 것을 임시로 저장할 변수
    *input_data = (char**)calloc(MAX_INST, sizeof(char*)); //input_data 동적 할당 후 초기화
    
    while (!feof(file)) { //파일의 내용이 끝날 때까지 반복
       
        //input_data의 내부 input_data[line_num] 동적할당 후 초기화
        input_data[line_num] = (char*)calloc(200, sizeof(char));

        fgets(inputRead, 200, file); //한줄씩 읽어와서 임시로 inputRead에 저장
       
        strcpy(input_data[line_num],inputRead); //해당 index의 구조체에 input_line_content변수에 읽어온 줄을 넣어줌
        line_num++; //index 하나 증가 (다음 라인 기록)

    }

    
    fclose(file);
    return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str)
{
    char* read_line = str; //parsing할 대상
    char* ptr = { 0, }; //tab단위로 잘라서 저장할 변수, 0으로 초기화 
    int count = 0; //몇 번째 토큰까지 넣었는지 체크
  
    // 동작 할당 token_table 내부 포인터 동적 할당
    token_table[token_line] = (char*)calloc(MAX_INST, sizeof(char));
    
    ptr = strtok(read_line, "\t"); //tab 기준으로 잘라서 token을 ptr변수에 저장
    if (strlen(ptr) <= 3 && strcmp(ptr," ") != 0) {
        char* temp = ptr;
        strtok(temp, "\n");
        token_table[token_line]->label = temp; //첫 token은 Label이다.
    }
    else
        token_table[token_line]->label = ptr; //첫 token은 Label이다.
    count++; //첫 token을 넣어줬으면 count를 1 증가함
   
    while (ptr != NULL) { //해당 토큰이 NULL이 아니면 계속 반복 (NULL이면 라인 끝 의미)
        
        
        ptr = strtok(NULL, "\t");

        if (ptr != NULL) {

            switch (count) {
            case 1:
                token_table[token_line]->operator = ptr; //count == 1이면 operator 넣을 차례
                count++;
                break;
            case 2:
                strtok(ptr, "\n");
                token_table[token_line]->operand = ptr; //count == 2이면 operand 넣을 차례
                count++;
                break;
            case 3:
                token_table[token_line]->comment = ptr; //coujnt == 3이면 comment 넣을 차례
                count++;
                break;
            default:
                break;
            }
        }
    }

    //한 줄 토큰 파싱 끝났으면 line index + 1
    token_line++;


    return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
 * 매계 : 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
 * 주의 :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
    //검색 할 operator name
    char* find_opStr = str;

    //Opcode찾으려면 inst table의 name과 token table의 operator 가 맞아야 함
    //예외1. Operator의 맨 앞이 "+" 인 것이 있으면 + 빼고 뒷글자부터 이름으로 지정
    //예외2. Operator 중 Assempler directives는 변환값 없도록 처리

    if (find_opStr == NULL)
        return 99;

    if (find_opStr[0] == 43) { //"+"의 ASCII code 43과 비교
       char* temp = find_opStr; //+ 뒷글자부터 저장할 변수
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

    //opcode 찾기
    for (int i = 0; i < inst_index; i++) {
        result = strcmp(find_opStr, inst_table[i]->name);
        if (result == 0) {
            return inst_table[i]->opcode;
        }
    }

    //다 돌아도 못 찾은 경우 99반환
    return 99;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
    /* input_table의 문자열을 한줄씩 입력 받아서 */
    /* token_parsing()을 호출하여 token_unit에 저장 */

    token_line = 0; // 초기화
    
    // token_table 동적 할당
    *token_table = (token*)calloc(MAX_INST, sizeof(token));

    while (input_data[token_line] != NULL) {
        
        //가져온 소스코드 한줄을 parsing 하여 token_table에 작성
        token_parsing(input_data[token_line]);

    }


    return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char* file_name)
{
    FILE* file = fopen(file_name, "w");

    int index = 0; // 초기화
    int count = token_line;
 
    while (count) {


        char* write_data1 = token_table[index]->label;
        char* write_data2 = token_table[index]->operator;
        char* write_data3 = token_table[index]->operand;
        char* write_data4;
        char* tab = "\t";
        
        //opcode 찾아서 넣어주기
        int find_opcode = search_opcode(write_data2); 
        if (find_opcode == 99) //못찾았을 경우
            write_data4 = "\t";
        else
            write_data4 = find_opcode;

        //NULL 문자는 tab으로 대체해서 출력하도록 함
        if (write_data2 == NULL)
            write_data2 = "\t";
        if (write_data3 == NULL)
            write_data3 = "\t";
        if (write_data4 == NULL)
            write_data4 = "\t";

        fprintf(file, "%s\t%s\t%s\t\t\t%s\n", write_data1,write_data2,write_data3,write_data4);
        index++; //소스코드 한줄을 다 출력했으면 index + 1해서 다음 소스코드로
        count--;
    }

    fclose(file);

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
    /* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char* filen_ame)
{
    /* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    return 0;
    /* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
    /* add your code here */
}
