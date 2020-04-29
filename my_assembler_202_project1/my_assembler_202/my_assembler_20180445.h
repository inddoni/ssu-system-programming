/*
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
typedef enum { false, true } bool;

/*
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
    // inst.txt ����
    // ADDR r1,r2	ADDR	2	90	2	r1	r2
    char* mnenmonic;
    char* name; //operator
    char* format;
    char* opcode;
    char* operand_unit;
    char* operand1;
    char* operand2;

};

// instruction�� ������ ���� ����ü�� �����ϴ� ���̺� ����
typedef struct inst_unit inst;
inst* inst_table[MAX_INST];
int inst_index;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */


char* input_data[MAX_LINES];
static int line_num;

/*
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit
{
    char* label;                //��ɾ� ���� �� label
    char* operator;             //��ɾ� ���� �� operator
    char* operand;              //��ɾ� ���� �� operand
    char* comment;              //��ɾ� ���� �� comment
    char nixbpe;                //��ɾ� ���ο� �ش��ϴ� nixbpe ����
};

typedef struct token_unit token;
token* token_table[MAX_LINES];
static int token_line;


/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * symbol�� �ش��ϴ� token�� ������ ����ϴ� ����ü ����
 * symbol�� �̸��� �ּ������� ������ �ִ�.
 */
struct symbol_unit
{
    char symbol[10]; //symbol�� �̸�
    int addr; //�ּ�����
};


//symbol token�� �����ϴ� ���̺� ����
typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

/*
* ���ͷ�(�� token)�� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ���ȴ�.
*/
struct literal_unit
{
    char literal[10]; //literal�� �̸�
    int addr; //�ּ�����
};

//symbol token�� �����ϴ� ���̺� ����
typedef struct literal_unit literal;
literal literal_table[MAX_LINES];

static int locctr;
int address[MAX_LINES];
int programLength[3]; //CSECT �������� ���� ���α׷� ������ ���̸� ��� �迭
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



/* ���� ������ �Լ� ��� */

//10������ 16������ ��ȯ�ϴ� �Լ�
int dtohex(int value);
//�ҽ��ڵ忡 ���μ������ �ּҰ��� ����Ͽ� �Ҵ��ϴ� �Լ�
//�ּҰ��� ����Ͽ� token_table�� sourceAddr������ �����Ѵ�.
void address_define(void);

//nixbpe�� �Ҵ��ϴ� �Լ�
void nixbpe_define(void);

//symbol name�� ������ �ּҰ��� ��ȯ���ִ� �Լ�
int search_symbol(char* name);
//symbol name�� �� ��° ���α׷������� ���� ������ �ּҰ��� ��ȯ���ִ� �Լ�
int search_symbol_p(char* name, int p);
//literal name�� ������ �ּҰ��� ��ȯ���ִ� �Լ�
int search_literal(char* name);

//���� ���α׷��� EXTREF�� �ش��ϴ��� �Ǻ����ִ� �Լ�
void setExtref(void);

//���� ���α׷��� EXTREF�� �ش��ϴ��� �Ǻ����ִ� �Լ�
int search_extref(int programNum, char* name);

//�ش� operator�� format�� ã�� ��ȯ���ִ� �Լ�
int search_format(char* name);

//reference �� üũ�س���
int readCheck[MAX_LINES] = { 0, }; 

