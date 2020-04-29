/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
typedef enum { false, true } bool;

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
    // inst.txt 예시
    // ADDR r1,r2	ADDR	2	90	2	r1	r2
    char* mnenmonic;
    char* name; //operator
    char* format;
    char* opcode;
    char* operand_unit;
    char* operand1;
    char* operand2;

};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst* inst_table[MAX_INST];
int inst_index;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */


char* input_data[MAX_LINES];
static int line_num;

/*
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
    char* label;                //명령어 라인 중 label
    char* operator;             //명령어 라인 중 operator
    char* operand;              //명령어 라인 중 operand
    char* comment;              //명령어 라인 중 comment
    char nixbpe;                //명령어 라인에 해당하는 nixbpe 설정
};

typedef struct token_unit token;
token* token_table[MAX_LINES];
static int token_line;


/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * symbol에 해당하는 token의 정보를 기술하는 구조체 변수
 * symbol의 이름과 주소정보를 가지고 있다.
 */
struct symbol_unit
{
    char symbol[10]; //symbol의 이름
    int addr; //주소정보
};


//symbol token을 관리하는 테이블 생성
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

/*
* 리터럴(의 token)을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
*/
struct literal_unit
{
    char literal[10]; //literal의 이름
    int addr; //주소정보
};

//symbol token을 관리하는 테이블 생성
typedef struct literal_unit literal;
literal literal_table[MAX_LINES];

static int locctr;
int address[MAX_LINES];
int programLength[3]; //CSECT 기준으로 나뉜 프로그램 각각의 길이를 담는 배열
int objectCode[MAX_LINES];
char* first_extref[10];
char* second_extref[10];
char* third_extref[10];
//--------------

static char* input_file;
static char* output_file;
int init_my_assembler(void);
int init_inst_file(char* inst_file);
int init_input_file(char* input_file);
int token_parsing(char* str);
int search_opcode(char* str);
static int assem_pass1(void);
void make_opcode_output(char* file_name);

void make_symtab_output(char* file_name);
void make_literaltab_output(char* file_name);
static int assem_pass2(void);
void make_objectcode_output(char* file_name);



/* 직접 정의한 함수 목록 */

//10진수를 16진수로 변환하는 함수
int dtohex(int value);
//소스코드에 라인순서대로 주소값을 계산하여 할당하는 함수
//주소값을 계산하여 token_table에 sourceAddr변수에 저장한다.
void address_define(void);

//nixbpe를 할당하는 함수
void nixbpe_define(void);

//symbol name을 넣으면 주소값을 반환해주는 함수
int search_symbol(char* name);
//symbol name과 몇 번째 프로그램인지의 값을 넣으면 주소값을 반환해주는 함수
int search_symbol_p(char* name, int p);
//literal name을 넣으면 주소값을 반환해주는 함수
int search_literal(char* name);

//현재 프로그램의 EXTREF에 해당하는지 판별해주는 함수
void setExtref(void);

//현재 프로그램의 EXTREF에 해당하는지 판별해주는 함수
int search_extref(int programNum, char* name);

//해당 operator의 format을 찾아 반환해주는 함수
int search_format(char* name);

//reference 들 체크해놓기
int readCheck[MAX_LINES] = { 0, }; 

