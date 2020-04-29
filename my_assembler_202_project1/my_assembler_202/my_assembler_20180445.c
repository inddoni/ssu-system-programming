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

    
    make_symtab_output("symtab_20180445.txt");

    make_literaltab_output("literaltab_20180445.txt");

    
    if(assem_pass2() < 0 ){
        printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ;
        return -1 ;
    }

    ////object code 출력
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
    if (strlen(ptr) <= 2 && strcmp(ptr," ") != 0) {
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
                if (strcmp(ptr, "CSECT\n") == 0 || strcmp(ptr, "LTORG\n") == 0)
                    strtok(ptr, "\n");
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
    char find_opStr[10] = { 0, };
    if(str)
        strcpy(find_opStr, str);

    //Opcode찾으려면 inst table의 name과 token table의 operator 가 맞아야 함
    //예외1. Operator의 맨 앞이 "+" 인 것이 있으면 + 빼고 뒷글자부터 이름으로 지정
    //예외2. Operator 중 Assempler directives는 변환값 없도록 처리

    if (find_opStr == NULL)
        return 99;

    if (find_opStr[0] == 43) { //"+"의 ASCII code 43과 비교
       char temp[10] = { 0, };
       strcpy(temp, find_opStr); //+ 뒷글자부터 저장할 변수
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

    //opcode 찾기
    for (int i = 0; i < inst_index; i++) {
        result = strcmp(find_opStr, inst_table[i]->name);
        if (result == 0) {
            return strtol(inst_table[i]->opcode, NULL, 16);
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

    
    //소스코드 라인별로 Address 할당
    address_define();

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

 
    for(int i = 0; i < token_line; i ++) {


        char* write_data1 = token_table[i]->label;
        char* write_data2 = token_table[i]->operator;
        char* write_data3 = token_table[i]->operand;
        int write_data4;
        char* tab = "\t";
        
        //opcode 찾아서 넣어주기
        int find_opcode = search_opcode(write_data2); 
        if (find_opcode == 99) //못찾았을 경우
            write_data4 = 99;
        else
            write_data4 = find_opcode;

        //NULL 문자는 tab으로 대체해서 출력하도록 함
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
* 직접 정의한 함수
* 설명 : 10진수의 숫자를 입력받아 16진수로 변환하는 함수
* 매계 : (16진수로 변환할)10진수의 숫자
* 반환 : 16진수로 변환한 결과값
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
            hex[index] = 48 + mod; //숫자 0의 ASCII코드값 48 + 나머지
        else
            hex[index] = 65 + (mod - 10); //대문자 A의 ASCII코드값 65 + 나머지

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
* 직접 정의한 함수
* 설명 : symbol name을 매개로 호출하면 해당하는 address를 찾아 반환해주는 함수
* 매계 : symbol name
* 반환 : 해당 symbol의 address
*  ----------------------------------------------------------------------------------
*/
int search_symbol(char* name) {

    for (int i = 0; i < token_line; i++) {
        if (strcmp(sym_table[i].symbol, "") != 0) { //NULL이 아니면
            if (strcmp(name, sym_table[i].symbol) == 0) {
                return address[i]; //찾으면 주소값 반환
            }
        }
    }

    return 99999; //못찾았으면 99999반환
}
/* ----------------------------------------------------------------------------------
* 직접 정의한 함수
* 설명 : 프로그램 번호와 symbol name을 매개로 호출하면 해당하는 address를 찾아 
         해당 프로그램에 속한 symbol의 address를 반환해주는 함수
* 매계 : symbol name, program no
* 반환 : 해당 symbol의 address
*  ----------------------------------------------------------------------------------
*/
int search_symbol_p(char* name, int p) {

    if (p == 0) {
        for (int i = 0; i < 23; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL이 아니면
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //찾으면 주소값 반환
                }
            }
        }
    }
    else if (p == 1) {
        for (int i = 24; i < 44; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL이 아니면
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //찾으면 주소값 반환
                }
            }
        }
    }
    else { //p == 3이면
        for (int i = 47; i < token_line; i++) {
            if (strcmp(sym_table[i].symbol, "") != 0) { //NULL이 아니면
                if (strcmp(name, sym_table[i].symbol) == 0) {
                    return address[i]; //찾으면 주소값 반환
                }
            }
        }
    }

    return 99999; //못찾았으면 99999반환
}

/* ----------------------------------------------------------------------------------
* 직접 정의한 함수
* 설명 : symbol name을 매개로 호출하면 해당하는 address를 찾아 반환해주는 함수
* 매계 : symbol name
* 반환 : 해당 symbol의 address
*  ----------------------------------------------------------------------------------
*/
int search_literal(char* name) {
    for (int i = 0; i < token_line; i++){
        if (literal_table[i].literal != NULL) {
            if (strcmp(name, literal_table[i].literal) == 0) {
                return literal_table[i].addr; //찾으면 주소값 반환
            }
        }
    }

    return 99999; //못찾았으면 99999반환
}

/* ----------------------------------------------------------------------------------
* 직접 정의한 함수
* 설명 : 명령어 라인에 맞는 Address 값을 token_table의 sourceAddr 변수에 저장하는 
*        함수이다. inst_table의 format과 token_table의 operator를 활용하여 계산한다.
         address[MAX_LINE]에 구한 주소 값을 저장한다.
         sys_table의 값을 정한다.
* 매계 : 없음
* 반환 : 없음
*  ----------------------------------------------------------------------------------
*/
void address_define(void) {

    
    locctr = 0; //실제로 라인마다 입력될 주소값
    int litCount = 0;
    int proIndex = 0; //섹션이 나뉜 프로그램의 인덱스

    /* operator start면 0
    * operator가 CSECT면 주소값 0으로 초기화
    * operator가 EXTDEF나 EXTREF나 END면 주소값 NULL
    * operator의 맨 앞이 "+"이면 +4
    * operator의 맨 앞이 "="이면 operand 맨 앞이 C인지 X인지 확인해서 +3 혹은 +1
    * operator가 RESW이거나 WORD이면 해당 operand의 값 * 3 해서 더해주기
    * operator가 BYTE면 operand의 값 * 1 해서 더해주기
    * operator가 LTORG면 주소값 더하기 0 (나중에 출력안되게 하기)
    * =============================================================================
    * 위의 경우에 해당하지 않으면 inst_table에 oprator에 해당하는 format을 찾아서 +3 혹은 +2 해주기 */

    for (int i = 0; i < token_line; i++) { //소스코드 모든 라인에 반복
        

        char* oper = token_table[i]->operator;
        char* lab = token_table[i]->label;

        if (oper != NULL || strcmp(lab, "END") == 0) {
            
            address[i] = locctr;
           

            //operator가 START이면 주소값은 0으로 초기화
            if (strcmp(oper, "START") == 0) {
                locctr = 0;
            }
            //operator가 EXTDEF나 EXTREF면 주소값 NULL
            else if (strcmp(oper, "EXTDEF") == 0 || strcmp(oper, "EXTREF") == 0) {
                locctr = 0;
            }
            //operator의 맨 앞이 "+"이면 +4
            else if (oper[0] == '+') {
                locctr += 4;
                //printf("%s\t%s\t%x\t \n", lab, oper, address[i]);
                //continue;
            }
            //operator의 맨 앞이 "="이면 operand 맨 앞이 C인지 X인지 확인해서 +3 혹은 +1
            else if (oper[0] == '=') {
                if (oper[1] == "C") 
                    locctr += 3;
                else 
                    locctr += 1;
            }
            //operator가 RESW이거나 WORD이면 해당 operand의 값 * 3 해서 더해주기
            else if (strcmp(oper, "RESW") == 0 || strcmp(oper, "WORD") == 0) {
                if ((strcmp(oper, "RESW") == 0)) {
                    int num = atoi(token_table[i]->operand);
                    locctr += (3 * num);
                }
                else { //WORD일때는 그냥 +3만 해주기
                    locctr += 3;
                }
            }
            //operator가 BYTE면 operand의 값 * 1 해서 더해주기
            else if (strcmp(oper, "RESB") == 0 || strcmp(oper, "BYTE") == 0) {
                if (strcmp(oper, "RESB") == 0) {
                    int num = atoi(token_table[i]->operand);
                    locctr += num;
                }
                else { //BYTE일때는 그냥 +1만 해주기
                    locctr += 1;
                }
            }
            //operator가 LTORG면 주소값 더하기 0 
            else if (strcmp(oper, "LTORG") == 0 || strcmp(lab, ".") == 0) {
                if (strcmp(oper, "LTORG") == 0) {
                    locctr += litCount;
                    litCount = 0; //그동안 리터럴이 나와서 카운트 해놓은 값을 넣어주고 0으로 초기화
                }
            }
            //EQU이면 operand 보고 계산 후 현재값(index-1) 값 갱신 해주기 (operand가 * 이면 현재값 변경 없음)
            else if (strcmp(oper, "EQU") == 0) {
                //bufend-buffer 계산 결과값 넣어주기
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

                    //두개 뺀 값을 address[i] 값으로 주기
                    address[i] = abs(value1 - value2);
                }
            }
            //섹션 나뉘거나 프로그램이 끝날때 길이 계산
            else if (strcmp(oper, "CSECT") == 0 || strcmp(lab, "END") == 0) {
                programLength[proIndex] = locctr;
                //printf("program Length : %x\n", programLength[proIndex]);
                proIndex++;
                locctr = 0;
            }

            //operator에 대해 inst_table에서 format을 찾아서 +3 또는 +2 해주기
            else {
                if(search_format(oper) != 99)
                    locctr += search_format(oper);       
            }

            //operand에서 리터럴이 나오면 count해주기
            if (token_table[i]->operand && token_table[i]->operand[0] == '=') {
                if (token_table[i]->operand[1] == 'C')
                    litCount += 3;
                else
                    litCount += 1;
            }
            //printf("%s\t%s\t%x\t \n", lab, oper, address[i]);
        }
        
        //label이 있으면 symbol table에 저장
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
* 직접 정의한 함수
* 설명 : 명령어 라인을 돌며 opcode와 nixbpe를 결정해서 테이블에 넣어줌
* 매계 : 없음
* 반환 : 없음
*  ----------------------------------------------------------------------------------
*/
void nixbpe_define(void) {
    // label이 +인것, #인것, @인것 각각 값 정해주기
    // operator가 RSUB인 것
    // operend에 , 가 있는 것 (+있는지 2중 체크)
    // 나머지는 기본 SIC/XE nixbpe 설정

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
                    token_table[i]->nixbpe = 39; //16진수 39   2진수 111001 10진수
                else
                    token_table[i]->nixbpe = 38; //16진수 38   2진수 111000
            }
            else if (oper[0] == '+') {
                token_table[i]->nixbpe = 31; //16진수 31   2진수 110001
            }
            else if (operand[0] == '#') {
                token_table[i]->nixbpe = 10; //16진수 10   2진수 010000
            }
            else if (operand[0] == '@') {
                token_table[i]->nixbpe = 22; //16진수 22   2진수 100010
            }
            else if (strcmp(oper, "RSUB") == 0) {
                token_table[i]->nixbpe = 30; //16진수 30   2진수 110000
            }
            else
                token_table[i]->nixbpe = 32; //16진수 32   2진수 110010
            
            printf("%d\n", token_table[i]->nixbpe);
        }
        

        //printf("%s\t%s\t%d\n", operators, operand, token_table[i]->nixbpe);
    }

}
/* ----------------------------------------------------------------------------------
* 직접 정의한 함수
* 설명 : 현재 프로그램의 EXTREF에 해당하는지 판별해주는 함수
* 매계 : 프로그램 번호(몇 번째 프로그램인지)와, operand name
* 반환 : 맞으면 1, 아니면 0 반환
*  ----------------------------------------------------------------------------------
*/
void setExtref(void) {

    int count = -1; //프로그램 번호
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
* 직접 정의한 함수
* 설명 : 현재 프로그램의 EXTREF에 해당하는지 판별해주는 함수
* 매계 : 프로그램 번호(몇 번째 프로그램인지)와, operand name
* 반환 : 맞으면 1, 아니면 0 반환
*  ----------------------------------------------------------------------------------
*/
int search_extref(int programNum, char* name) {
    
    int count = programNum; //프로그램 번호
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
* 직접 정의한 함수
* 설명 : 해당 operator의 format을 찾아 반환해주는 함수
* 매계 : format을 찾으려는 operaotor name
* 반환 : opertor의 format(3, 2 등), 에러코드 99
*  ----------------------------------------------------------------------------------
*/
int search_format(char* name) {

    //검색 할 operator name
    char find_name[10] = { 0, };
    if (name)
        strcpy(find_name, name);

    //예외. Operator의 맨 앞이 "+" 인 것이 있으면 + 빼고 뒷글자부터 이름으로 지정

    if (find_name == NULL)
        return 99;

    if (find_name[0] == 43) { //"+"의 ASCII code 43과 비교
        char temp[10] = { 0, };
        strcpy(temp, find_name); //+ 뒷글자부터 저장할 변수
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
    /* Symbol Table 값 16진수로 출력 */

    FILE* file = fopen(file_name, "w");

    for (int i = 0; i < token_line; i++) {
        if (strcmp(sym_table[i].symbol, "") != 0) {
            fprintf(file, "%s\t%x\n", sym_table[i].symbol, sym_table[i].addr);
        }
    }

    fclose(file);
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
void make_literaltab_output(char* file_name)
{
    /* Literal Table 값 16진수로 출력 */

    //소스코드의 operand를 스캔하면서 [0]번재 글자가 = 이면 테이블에 추가
    //소스코드의 operator를 스캔하면서 LTORG가 나오면 리터럴테이블에 있는 =에 대해 주소값을 넣어줘야함 => 주소함수에서 하자
   
    FILE* file = fopen(file_name, "w");
   
    int start = 0;
    int index = -1;
    for (int i = 0; i < token_line; i++) {
        
        char* operand = token_table[i]->operand;

        // 1. 돌면서 operand '='인 것 테이블에 넣기 
        if (operand) {
            if (operand[0] == '=') {

                //동일한 값이 이미 테이블에 들어있는지 확인 (있으면 check = 1)
                int check = 0;
                for (int k = 0; k < i; k++) {
                    if (strcmp(literal_table[k].literal, operand) == 0) {
                        check = 1;
                        break;
                    }
                }

                //리터럴 테이블에 값 넣기
                if (check != 1) {
                    strcpy(literal_table[i].literal, operand);
                    continue;
                }
            }
        }
     
        if (token_table[i]->operator) {

            if (strcmp(token_table[i]->operator, "LTORG") == 0) {
                index++;
                for (index; index < i; index++) { //현재 라인까지 모은 리터럴들을 저장
                    if (strcmp(literal_table[index].literal,"") != 0) {
                        strcpy(literal_table[i].literal, literal_table[index].literal);
                        strcpy(literal_table[index].literal, "");
                        literal_table[i].addr = address[i];
                    }
                }
            }

            else if (strcmp(token_table[i]->label, "END") == 0) {
                index++;
                for (index; index < i; index++) { //현재 라인까지 모은 리터럴들을 저장
                    if (strcmp(literal_table[index].literal, "") != 0) {
                        strcpy(literal_table[i].literal, literal_table[index].literal);
                        strcpy(literal_table[index].literal, "");
                        literal_table[i].addr = address[i];
                    }
                }
                //END일때 넣어주면 프로그램 길이도 늘려주기
                if (strcmp(token_table[i]->label, "END") == 0) {
                    if (literal_table[i].literal[1] == 'X')
                        programLength[2] += 1;
                    else
                        programLength[2] += 3;
                }
            }
        }

        
    }

    //텍스트파일에 literal table 쓰기
    for (int i = 0; i < token_line; i++) {

        if (strcmp(literal_table[i].literal, "") != 0) {
            //printf("%s\t%x\n", literal_table[i].literal, literal_table[i].addr);
            //리터럴 테이블 파일에 쓸 넣을 리터럴을 추출한다. (while문으로)
            int index = 0; //lit배열안에 넣어줄 글자 인덱스
            int test = 3;
            char litPrint[10] = { 0, };
            while (literal_table[i].literal[test] != 39) { //작은따옴표가 아니면
                litPrint[index] = literal_table[i].literal[test];
                test++; index++;
            }
            fprintf(file, "%s\t%x\n", litPrint, literal_table[i].addr);
        }
        
    }


    fclose(file);
}


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
  
    nixbpe_define();
    setExtref(); //참조변수들 담기

    int count = -1; // 몇 번째 프로그램인지 구분
    int opcodeF = 0;
    int operandAddr = 0;
    int upper = 0;
    int lower = 0;
    int nixbpe = 0;
    for (int i = 0; i < token_line; i++) {

        char* operand = token_table[i]->operand;

        if (strcmp(token_table[i]->label, ".") != 0) { //comment line이 아니면

            if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTREF") == 0) { //몇번째 프로그램인지 설정해주기
                count++; //count번째 프로그램이다.
            }

            // 1. opcode있으면 찾기
            opcodeF = search_opcode(token_table[i]->operator);
            if (opcodeF != 99) { //opcode를 찾았으면

                //format 2자리수 따로 처리해주고 continue 시키기
                if (search_format(token_table[i]->operator) != 99) { //operator에 맞는 포멧이 있으면
                    if (search_format(token_table[i]->operator) == 2) { //format 이 2인지 확인하기
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
                            else // T인 경우
                                operandAddr = 5;

                            lower = operandAddr * mult;
                            mult /= 16;
                            ptr = strtok(NULL, ",");
                        }

                        upper = opcodeF * 16 * 16;
                        objectCode[i] = upper + lower;
                        printf("object code : %x\n", objectCode[i]); //나중에 지우기!!!!!!!!!!!!!!!!!!!!!!
                        continue;
                    }
                }

                // 1. opcode가 있으면 symbol table에서 operand의 주소찾아서 가져오기 (=TA)
                // 예외)) operand가 extref에 있는 이름이면 0으로 가져오기
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
                        while (operand[idx] != NULL) { //@빼고 뒷글자부터 Operand 이름 다시저장
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
                            operandAddr = 0; //참조변수가 맞으면 주소는 0으로 처리
                        else
                            operandAddr = search_symbol_p(ptr, count);
                    }
                    else
                        operandAddr = search_symbol_p(operand, count);
                }
                
                // 여기까지 실행후 모든 opcode가 있는 라인들은 심볼테이블에 매칭된 주소 혹은 99999를 갖고 있을 것


                // '+' operator 처리
                if (token_table[i]->operator[0] == '+') {
                    opcodeF = opcodeF * 16; //480
                    nixbpe = token_table[i]->nixbpe;
                    upper = opcodeF + ((nixbpe / 10) * 16) + (nixbpe % 10);
                    upper = upper * 16 * 16 * 16 * 16 * 16; //5자리 up
                    lower = operandAddr;
                    objectCode[i] = upper + lower;
                    printf("object code : %x\n", objectCode[i]); //나중에 지우기!!!!!!!!!!!!!!!!!!!!!!
                    continue;
                }
                    


                // 심볼테이블에 없는 값 처리
                if (operandAddr == 99999) { //operand가 #인지 =인지에 따라 계산

                    if (operand[0] == '=') { //리터럴
                        //리터럴 테이블 search
                        operandAddr = search_literal(operand); //10진수반환됨
                        lower = operandAddr - address[i + 1];
                        //lower = lower % 1000; //TA - PC 결과값의 끝 3자리 추출
                    }

                    else if (operand[0] == '#') { //상수
                        operand[0] = '0'; // #을 0으로 바꿔줌 #3 -> 03 이라는 문자열이 됨
                        lower = atoi(operand);
                    }

                    else {
                        lower = 0;
                    }


                }
                else { // 심볼테이블에서 찾았으면

                    lower = operandAddr - address[i + 1]; //TA- PC 
                    //lower = lower % 1000;
                }

                nixbpe = token_table[i]->nixbpe;
                nixbpe = ((nixbpe / 10) * 16) + (nixbpe % 10); //nixbpe 10진수로 변환
                upper = opcodeF * 16 + nixbpe;
                upper = upper * 16 * 16 * 16;
                if (lower < 0) { //lower가 음수이면
                    objectCode[i] = upper + lower + 4096;
                }
                else
                    objectCode[i] = upper + lower;
            }

            // 2. opcode를 못 찾았으면 operator가 WORD나 BYTE인지 확인
            else if (strcmp(token_table[i]->operator, "WORD") == 0) {
                objectCode[i] = 000000; //자리만 할당
                readCheck[i] = 1;
            }

            else if (strcmp(token_table[i]->operator, "BYTE") == 0) {
                // operand의 작은 따옴표 안에 값만 읽어오기
                int num = 0;
                int mult = 16;
                char byteF[3] = { 0, };
                for (int k = 2; k < 4; k++) {
                    byteF[k - 2] = operand[k]; //'F' 추출 => 15
                    if(byteF[k-2] >= 48 && byteF[k-2] <= 57)
                        num += (byteF[k - 2] - 48) * mult;
                    else
                        num += (byteF[k - 2] - 55) * mult;
                    mult = mult / 16;
                }
                objectCode[i] = num;
            }

            //3. 리터럴 처리하기
            else if (strcmp(token_table[i]->operator, "LTORG") == 0 || strcmp(token_table[i]->operator, "FIRST") == 0) {
                //해당 주소값에 맞는 리터럴을 ASCII코드로 변환해서 넣기
                //=C'EOF' = 454f46 / =X'05' = 05
                objectCode[i] = 0; //일단 object code 0으로 셋팅
                int num = 0;
                int mult = 16*16*16*16;
                if (literal_table[i].literal[1] == 'C') { // =C 이면
                    char byteF[4] = { 0, };
                    for (int k = 3; k < 6; k++) { //3개의 캐릭터를 읽어서 object code를 만듬
                        byteF[k - 3] = literal_table[i].literal[k];
                        num += byteF[k - 3] * mult;
                        mult = (mult / 16)/16;
                    }
                    objectCode[i] = num;
                }
                else { // =X 이면
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

            else { //나머지 operator들 (START, EXTEDF, CSECT, EQU 등) 
                objectCode[i] = NULL;
            }
            printf("object code : %x\n", objectCode[i]);
        }
        
    }
    


    return 0;
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
    FILE* file = fopen(file_name, "w");
    int count = -1; //몇번째 프로그램인지
    int i = 0;
    char ref_temp[30] = { 0, };
    int isStart = 1;
    int modeStart = 0;
    int zero = 0;
    while (i < token_line) {

        if (strcmp(token_table[i]->label, ".") != 0) { //comment line이 아니면

            // Header
            if (isStart == 1) { //프로그램 시작부분일때만 isStart == 1 임
                if (strcmp(token_table[i]->label, "END") == 0)
                    break;
                
                count++; //몇번째 프로그램에 들어왔는지 설정
                fprintf(file, "H%s\t%06X%06X\n", token_table[i]->label, zero, programLength[count]);
                isStart = 0;
                i++;
                continue; //한 줄 입력했으면 i 증가 후 while문 처음으로 다시 올라감
            }

            //D라인
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                // "," 기준으로 파싱하면서 심볼테이블에서 프로그램번호와 이름으로 주소찾아오기
                fprintf(file, "D");
                char temp[30] = { 0, };
                strcpy(temp, token_table[i]->operand);
                char* ptr = strtok(temp, ",");
                int addF = 0;
                while (ptr != NULL) {
                    addF = search_symbol_p(ptr, count); //심볼테이블에서 주소값 찾아옴
                    fprintf(file, "%s%06X", ptr, addF);
                    ptr = strtok(NULL, ",");
                }
                fprintf(file, "\n");
                i++;
                continue;  //한 줄 입력했으면 i 증가 후 while문 처음으로 다시 올라감
            }

            //Rerence라인
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "EXTREF") == 0) {
                // "," 기준으로 파싱하면서 이름만 기록
                fprintf(file, "R");
                char temp[30] = { 0, };
                strcpy(temp, token_table[i]->operand);
                strcpy(ref_temp, token_table[i]->operand); //해당 프로그램의 M 라인에서 쓸 정보 미리 저장
                char* ptr = strtok(temp, ",");
                while (ptr != NULL) {
                    fprintf(file, "%s ", ptr);
                    ptr = strtok(NULL, ",");
                }
                fprintf(file, "\n");
                i++;
                continue;  //한 줄 입력했으면 i 증가 후 while문 처음으로 다시 올라감
            }

            // Text 처리 전 오브젝트 코드를 만드는 데 필요없는 오퍼랜드는 넘기기
            else if (token_table[i]->operator && strcmp(token_table[i]->operator, "RESB") == 0 || strcmp(token_table[i]->operator, "EQU") == 0 || strcmp(token_table[i]->operator, "RESW") == 0) {
                i++;
                continue;
            }

            

            //위에 해당되지 않으면서, operator가 있는 애들은 text 라인에 들어갈 것
            else if (token_table[i]->operator) { 
                while (strcmp(token_table[i]->operator,"CSECT") != 0 && strcmp(token_table[i]->operator,"FIRST") != 0) { //해당 프로그램의 끝이 아니면 계속 반복
                    int line_length = 0;
                    char pr[10] = { 0, };
                    char prTotal[150] = { 0, };
                    int startAddr = address[i];

                    while (line_length < 29 && strcmp(token_table[i]->operator,"CSECT") != 0) { // 한 줄의 최대 길이 내에서 반복
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
                            strcat(prTotal, pr); // 한 라인에 들어갈 코드 붙이기
                            break;
                            //프로그램 라인 증가 안함
                        }
                        else {
                            line_length += 3;
                            sprintf(pr, "%06X", objectCode[i]);
                            i++;
                        }
                        strcat(prTotal, pr); // 한 라인에 들어갈 코드 붙이기
                    }

                    // 한 라인이 입력이 끝나면
                    if (line_length != 0) { // 입력된 문자가 있는지 확인
                        fprintf(file, "T%06X%02X%s\n", startAddr, line_length, prTotal); //시작주소, 해당 라인 총 길이
                    }

                }

                // Csect 만나면(프로그램의 끝) 반복문 빠져나옴 : M라인 처리하고, i++하고 while문 처음(맨위)으로 가게 해야함
                // M 라인 처리
                for (int j = modeStart; j < i; j++) { //한 프로그램 번위 안에서 도는 반복문
                    if (readCheck[j] == 1) {
                        int locat = 0; // 명령어 코드 내에서 자릿수
                        int addr = 0;
                        if (token_table[j]->operator[0] == '+') {
                            locat = 5;
                            addr = address[j] + 1;
                        }
                        else {
                            locat = 6;
                            addr = address[j]; //그대로
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

                modeStart = i; //다음 프로그램블럭때 찾을 시작 값 수정


                // m라인 처리 끝낸 후 E라인 출력하고 i++하고 while문 처음으로!
                if (count == 0) {
                    fprintf(file, "E%06X\n\n", 0);
                }
                else
                    fprintf(file, "E\n\n");
                
                if (count < 3) 
                    isStart = 1;

                //i++; // 1 증가 안시킴!! 프로그램 이름부분부터 시작하려고 
                continue;

            }

            else {
                i++;
                continue;
            }
            

        }

        else { //label == "." 이면 다음 라인 읽도록 인덱스 증가시켜주기
            i++;
        }
    }

    fclose(file);

}
