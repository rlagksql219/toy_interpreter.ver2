#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>

#define BUFFER_SIZE 1024

using namespace std;

typedef struct TreeNode { /* 트리 노드 구조체 */
	string data;
	TreeNode* parent;
	TreeNode* left;
	TreeNode* right;
} Node;


void define_defun(); /* DEFUN을 사용하여 정의한 명령어를 defun.txt 파일에 저장 */
void print_defun(); /* defun.txt에 정의되어 있는 DEFUN 명령어 출력 */
void interpreter(); /* IF와 MINUS로만 이루어진 식으로 바꾸어 결과값 계산 */
void print_infix(vector<string> final); /* 실행 프로그램에서 읽은 문장 출력 */
void print_postfix(Node* root); /* 읽은 문장을 postfix로 바꾸어 출력 */
void print_node(Node* cur); /* 노드에 저장된 데이터를 출력 */

vector<string> change_defun(char read_line[]); /* DEFUN으로 정의한 명령어를 IF와 MINUS로 이루어진 식으로 대체 */
vector<string> change_defun2(vector<string> final); /* DEFUN으로 정의한 명령어를 IF와 MINUS로 이루어진 식으로 대체 */
vector<string> make_token(char* read_line, int n); /* 계산할 문장을 숫자와 연산자로 나누어 토큰 형태로 저장 */
Node* make_tree(vector<string> expr); /* 토큰으로 수식트리 생성 */
void InitNode(Node* node); /* 트리 노드 초기화 */
int Evaluate_expressionTree(Node* node); /* 수식트리를 postorder 방식으로 순회하여 결과값 계산 */
int value(string op, int op1, int op2); /* TOY Language 연산자에 따른 계산 */

int check_overlap(char read_line[]); /* DEFUN에 이미 정의되어있는 명령어인지 확인 */
int use_array_error_check(char* read_line); /* 문법에 맞게 문장이 입력되었는지 확인 */
int use_vector_error_check(char* read_line, vector<string> expr); /* 문법에 맞게 문장이 입력되었는지 확인 */


int main()
{
	int num;

	for (int i = 0; ; i++) {
		if (i == 0) {
			printf("==================================\n");
			printf("1. Define DEFUN\n");
			printf("2. Print DEFUN\n");
			printf("3. Interpreter\n");
			printf("4. Exit\n");
			printf("==================================\n");

			printf("메뉴를 선택하세요 >> ");
			scanf_s("%d", &num);
		}
		else {
			if (num != 1) {
				printf("==================================\n");
				printf("1. Define DEFUN\n");
				printf("2. Print DEFUN\n");
				printf("3. Interpreter\n");
				printf("4. Exit\n");
				printf("==================================\n");

				printf("메뉴를 선택하세요 >> ");
				scanf_s("%d", &num);
			}
			else {
				printf("\n");
				printf("메뉴를 선택하세요 >> ");
				scanf_s("%d", &num);
			}
		}


		if (num == 1) {
			define_defun();
		}

		if (num == 2) {
			print_defun();
		}

		if (num == 3) {
			interpreter();
		}

		if (num == 4) {
			printf("\n프로그램을 종료합니다.\n");
			return 0;
		}
	}
}


/**
함 수 : define_defun()
기 능 : 표준입력으로 DEFUN을 사용하여 명령어를 정의하고, defun.txt 파일에 해당 정의 저장
*/
void define_defun()
{
	char read_line[BUFFER_SIZE] = { 0, };
	int mark = 0;
	int overlap = 1;

	FILE* fp;


	for (int i = 0;; i++) {
		if (i == 0)
			getchar(); //메뉴 고를 때, 입력했던 개행문자 읽기

		printf("DEFUN ");
		memset(read_line, 0, BUFFER_SIZE);
		fgets(read_line, BUFFER_SIZE, stdin);


		/* 에러 처리 */
		for (int j = 0; j < BUFFER_SIZE; j++) {
			if (read_line[j] == 'x') {
				mark = 1;
				break;
			}
		}

		if (mark == 0) {
			printf("매개변수가 하나도 없습니다.\n");
			printf("다시 입력하세요.\n");
			printf("\n");
		}
		else {
			overlap = check_overlap(read_line);
			if (overlap == 1) {
				printf("함수가 이미 정의되어 있습니다.\n");
				printf("다시 입력하세요.\n");
				printf("\n");
			}
			else
				break;
		}
	}


	fopen_s(&fp, "defun.txt", "a");

	fputs(read_line, fp);

	fclose(fp);
}


/**
함 수 : print_defun()
기 능 : DEFUN을 사용하여 defun.txt 파일에 저장된 명령어 표준출력
*/
void print_defun()
{
	FILE* fp;
	char read_line[BUFFER_SIZE] = { 0, };


	fopen_s(&fp, "defun.txt", "r");

	while (!feof(fp))
	{
		fgets(read_line, BUFFER_SIZE, fp);
		printf("%s", read_line);

		memset(read_line, 0, BUFFER_SIZE);
	}
	printf("\n");

	fclose(fp);
}


/**
함 수 : interpreter()
기 능 : 결과값을 계산할 프로그램 파일을 입력받아 내용을 출력하고, DEFUN으로 정의한 명령어를 모두 IF와 MINUS로 바꾸어 결과값 계산
*/
void interpreter()
{
	char fname[200];
	FILE* fp;


	printf("실행 파일명을 입력하세요 >> ");
	scanf_s("%s", fname, sizeof(fname));

	fopen_s(&fp, fname, "r");

	if (fp == NULL)
	{
		perror("입력한 파일명이 폴더 내에 존재하지 않습니다\n");

		printf("실행 파일명을 입력하세요 >> ");
		scanf_s("%s", fname, sizeof(fname));

		fopen_s(&fp, fname, "r");
	}


	while (!feof(fp))
	{
		char read_line[BUFFER_SIZE] = { 0, };
		int eval = 0;
		int exist_error = 0;

		vector<string> expr;
		vector<string> final;
		Node* root;

		printf("\n************\n");
		fgets(read_line, BUFFER_SIZE, fp);

		for (int i = 0; i < BUFFER_SIZE; i++) {
			if (read_line[i] == '\n') {
				read_line[strlen(read_line) - 1] = '\0';
				break;
			}
		}

		printf("(%s) -> ", read_line);


		/* 에러 처리 */
		exist_error = use_array_error_check(read_line);
		if (exist_error == 1)
			continue;

		expr = make_token(read_line, sizeof(read_line));

		exist_error = use_vector_error_check(read_line, expr);
		if (exist_error == 1)
			continue;


		final = change_defun(read_line);

		for (int i = 0; i < final.size(); i++) {
			if (final[i] == "ADD" || final[i] == "NEG" || final[i] == "EQUAL" || final[i] == "POS" || final[i] == "ZERO" || final[i] == "IF/THEN/ELSE" || final[i] == "TIMES")
				final = change_defun2(final); //아직 DEFUN으로 정의한 명령어가 남아있는 경우
		}

		root = make_tree(final);
		eval = Evaluate_expressionTree(root);

		print_infix(final);
		print_postfix(root);
		printf("Result : %d\n", eval);

	}

	printf("\n");

	fclose(fp);

}


/**
함 수 : print_infix(vector<string> final)
기 능 : 프로그램 파일의 내용을 IF와 MINUS로만 이루어진 식으로 바꾸어 출력
*/
void print_infix(vector<string> final) {

	int index = 0;

	for (int j = final.size() - 1; j > 0; j--) {
		if (final[j] == "MINUS" || final[j] == "IF") {
			index = j;
			break;
		}
	}

	for (int i = 0; i < final.size(); i++) {
		char ch[BUFFER_SIZE] = { 0, };

		strcpy_s(ch, final[i].c_str());

		if (strcmp(ch, "MINUS") == 0 || strcmp(ch, "IF") == 0)
			printf("(%s ", ch);

		else if (i >= index + 2)
			printf("%s) ", ch);

		else
			printf("%s ", ch);
	}

	if (final[0] == "MINUS" && final.size() == 3)
		printf("\n");
	else if (final[0] == "IF" && final.size() == 3)
		printf("\n");
	else
		printf(")\n");
}


/**
함 수 : print_postfix(Node* root)
기 능 : 프로그램 파일에 저장된 내용의 결과값을 계산하기 위해 읽은 문장을 postfix로 바꾸어 출력
*/
void print_postfix(Node* root) {

	Node* cur;

	cur = root;

	printf("\n");
	printf("\nPrefix To Postfix : ");

	while (1) {
		if (cur->left == NULL && cur->right == NULL) {
			print_node(cur);
			break;
		}

		while (cur->left != NULL)
			cur = cur->left;
		cur = cur->parent;
		print_node(cur->left);

		cur = cur->right;
	}

	if (cur->parent != NULL)
		cur = cur->parent;
	if (cur->parent != NULL)
		cur = cur->parent;

	if (cur->left != root->left)
		print_node(cur->left);
	print_node(cur->right);
	print_node(cur);

	while (1) {
		if (cur->parent == root || cur == root) {
			print_node(root);
			break;
		}

		cur = cur->parent;
		print_node(cur->right);
		print_node(cur);
	}
	printf("\n");
}


/**
함 수 : print_node(Node* cur)
기 능 : 노드에 저장된 데이터 출력
*/
void print_node(Node* cur) {
	char ch[BUFFER_SIZE] = { 0, };

	strcpy_s(ch, cur->data.c_str());
	printf("%s ", ch);
}


/**
함 수 : change_defun(char read_line[])
기 능 : 프로그램에서 읽은 내용의 결과값을 계산하기 위해 DEFUN으로 정의한 명령어를 IF와 MINUS로 이루어진 식으로 대체
*/
vector<string> change_defun(char read_line[])
{
	char read_defun[BUFFER_SIZE] = { 0, };
	vector<string> test;
	vector<string> defun;

	FILE* fp;

	test = make_token(read_line, BUFFER_SIZE);

	fopen_s(&fp, "defun.txt", "r");

	while (!feof(fp))
	{
		int mark = 0;
		vector<string> final;

		fgets(read_defun, BUFFER_SIZE, fp);

		if (read_defun[0] == '\0' || read_defun[0] == '\n')
			break;

		for (int i = 0; i < BUFFER_SIZE; i++) {
			if (read_defun[i] == '\n') {
				read_defun[strlen(read_defun) - 1] = '\0';
				break;
			}
		}
		
		defun = make_token(read_defun, sizeof(read_defun));

		for (int i = 0; i < test.size(); i++) {
			if (test[i] == defun[0]) { //IF와 MINUS로 이루어진 식으로 대체
				mark = 1;
				if (defun[0] == "POS" || defun[0] == "NEG" || defun[0] == "ZERO") {
					int cnt = i;
					for (int j = 0; j < cnt; j++)
						final.push_back(test[j]);

					for (int j = 2; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 2; j < test.size(); j++)
						final.push_back(test[j]);
				}

				else if (defun[0] == "IF/THEN/ELSE") {
					int cnt = i;
					for (int j = 0; i < cnt; i++)
						final.push_back(test[j]);

					for (int j = 4; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						if (defun[j] == "y" && i < test.size())
							defun[j] = test[i + 2];

						if (defun[j] == "z" && i < test.size())
							defun[j] = test[i + 3];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 4; j < test.size(); j++)
						final.push_back(test[j]);
				}

				else {
					int cnt = i;
					for (int j = 0; i < cnt; i++)
						final.push_back(test[j]);

					for (int j = 3; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						if (defun[j] == "y" && i < test.size())
							defun[j] = test[i + 2];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 3; j < test.size(); j++)
						final.push_back(test[j]);
				}
			}
		}

		if (mark == 1) {
			test.clear();
			test.assign(final.begin(), final.end());
		}

		memset(read_defun, 0, BUFFER_SIZE);
	}

	fclose(fp);

	return test;
}


/**
함 수 : change_defun2(vector<string> final)
기 능 : 프로그램에서 읽은 내용의 결과값을 계산하기 위해 DEFUN으로 정의한 명령어를 IF와 MINUS로 이루어진 식으로 대체
*/
vector<string> change_defun2(vector<string> final) {

	char read_defun[BUFFER_SIZE] = { 0, };
	vector<string> test;
	vector<string> defun;

	FILE* fp;

	test.assign(final.begin(), final.end());

	fopen_s(&fp, "defun.txt", "r");

	while (!feof(fp))
	{
		int mark = 0;
		vector<string> final;

		fgets(read_defun, BUFFER_SIZE, fp);
		read_defun[strlen(read_defun) - 1] = '\0';
		defun = make_token(read_defun, sizeof(read_defun));

		for (int i = 0; i < test.size(); i++) {
			if (test[i] == defun[0]) { //IF와 MINUS로 이루어진 식으로 대체
				mark = 1;
				if (defun[0] == "POS" || defun[0] == "NEG" || defun[0] == "ZERO") {
					int cnt = i;
					for (int j = 0; j < cnt; j++)
						final.push_back(test[j]);

					for (int j = 2; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 2; j < test.size(); j++)
						final.push_back(test[j]);
				}

				else if (defun[0] == "IF/THEN/ELSE") {
					int cnt = i;
					for (int j = 0; i < cnt; i++)
						final.push_back(test[j]);

					for (int j = 4; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						if (defun[j] == "y" && i < test.size())
							defun[j] = test[i + 2];

						if (defun[j] == "z" && i < test.size())
							defun[j] = test[i + 3];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 4; j < test.size(); j++)
						final.push_back(test[j]);
				}

				else {
					int cnt = i;
					for (int j = 0; i < cnt; i++)
						final.push_back(test[j]);

					for (int j = 3; j < defun.size(); j++) {
						if (defun[j] == "x" && i < test.size())
							defun[j] = test[i + 1];

						if (defun[j] == "y" && i < test.size())
							defun[j] = test[i + 2];

						final.push_back(defun[j]);
					}

					for (int j = cnt + 3; j < test.size(); j++)
						final.push_back(test[j]);
				}
			}
		}

		if (mark == 1) {
			test.clear();
			test.assign(final.begin(), final.end());
		}

		memset(read_defun, 0, BUFFER_SIZE);
	}

	fclose(fp);

	return test;

}

/**
함 수 : make_token(char* read_line, int n)
기 능 : 계산할 문장을 저장한 배열에서 괄호를 제외하고, 숫자와 연산자로 나누어 토큰 형태로 저장
*/
vector<string> make_token(char* read_line, int n)
{

	vector<string> expr;
	vector<char> tmp;
	char rm_bracket[BUFFER_SIZE] = { 0 };

	for (int i = 0; i < n; i++) {
		if (read_line[i] == '(')
			continue;
		if (read_line[i] == ')')
			continue;
		if (read_line[i] == '\0')
			break;

		tmp.push_back(read_line[i]);
	}


	for (int i = 0; i < tmp.size(); i++)
		rm_bracket[i] = tmp[i];

	char* context = NULL;
	char* token = strtok_s(rm_bracket, " ", &context);

	while (token != NULL) {
		expr.push_back(token);
		token = strtok_s(NULL, " ", &context);
	}

	return expr;
}


/**
함 수 : make_tree(vector<string> expr)
기 능 : 숫자와 연산자로 나누어 저장된 토큰으로 수식트리 생성
*/
Node* make_tree(vector<string> expr)
{

	Node* root = new Node;
	Node* cur;
	Node* insert;

	InitNode(root);

	root->data = expr.front();

	cur = root;

	for (int i = 1; i < expr.size(); i++) {

		/* 이전 토큰이 연산자인 경우 */
		if (expr[i - 1] == "MINUS" || expr[i - 1] == "IF") {
			if (cur->right != NULL)
				cur = cur->right;


			if (cur->left == NULL) {
				insert = new Node;
				InitNode(insert);

				cur->left = insert;
				insert->parent = cur;
				cur = cur->left;
				cur->data = expr[i];
			}
			else {
				cur = cur->parent;
				insert = new Node;
				InitNode(insert);

				cur->right = insert;
				insert->parent = cur;
				cur = cur->right;
				cur->data = expr[i];
			}
		}

		/* 이전 토큰이 숫자인 경우 */
		else {
			cur = cur->parent;

			while (cur->right != NULL)
				cur = cur->parent;


			if (cur->right == NULL) {
				insert = new Node;
				InitNode(insert);

				cur->right = insert;
				insert->parent = cur;
				cur->right->data = expr[i];
			}
		}
	}

	while (cur->parent != NULL)
		cur = cur->parent;

	return cur;
}


/**
함 수 : InitNode(Node* node)
기 능 : 트리의 노드 초기화
*/
void InitNode(Node* node)
{
	node->data = '\0';
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
}


/**
함 수 : Evaluate_expressionTree(Node* node)
기 능 : 수식트리를 postorder 방식으로 순회하여 결과값 계산
*/
int Evaluate_expressionTree(Node* node)
{
	Node* left = node->left;
	Node* right = node->right;
	int answer;

	if (left == NULL && right == NULL) {
		return stoi(node->data);
	}

	int op1 = Evaluate_expressionTree(left);
	int op2 = Evaluate_expressionTree(right);
	string op = node->data;

	answer = value(op, op1, op2);
	return answer;
}


/**
함 수 : value(string op, int op1, int op2)
기 능 : TOY Language 연산자에 따른 계산
*/
int value(string op, int op1, int op2)
{

	if (op == "MINUS") {
		return op1 - op2;
	}


	if (op == "IF") {
		if (op1 > 0)
			return op2;

		else
			return 0;
	}
}


/**
함 수 : use_array_error_check(char* read_line)
기 능 : 문법에 맞게 문장이 입력되었는지 확인
*/
int use_array_error_check(char* read_line)
{
	int left_bracket = 0;
	int right_bracket = 0;

	/* 실수 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (read_line[i] == '.') {
			printf("숫자가 들어갈 자리에 정수가 아닌 실수가 입력되었습니다.\n");
			return 1;
		}
	}


	/* "(", ")"가 잘못 사용 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (read_line[i] == '(')
			left_bracket++;

		if (read_line[i] == ')')
			right_bracket++;
	}

	if (left_bracket != right_bracket) {
		printf("괄호가 잘못 사용되었습니다.\n");
		return 1;
	}


	/* "-" 여러번 연속으로 입력 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		char tmp1 = read_line[i];
		char tmp2 = read_line[i + 1];

		if (tmp1 == '-' && tmp2 == '-') {
			printf("숫자가 입력되어야 할 자리에 '-'가 여러 번 연속으로 입력되었습니다.\n");
			return 1;
		}
	}


	/* 알파벳과 "-"를 제외한 각종 기호 */
	for (int i = 0; i < BUFFER_SIZE; i++) {
		char tmp = read_line[i];
		char a[100] = "-&/()ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

		if (tmp == '\0')
			break;

		if (isdigit(tmp) == 0) { //숫자 제외

			if (strchr(a, tmp) == NULL) {
				printf("알파벳과 '-'를 제외한 각종 기호가 입력되었습니다.\n");
				return 1;
			}
		}
	}
}


/**
함 수 : use_vector_error_check(char* read_line, vector<string> expr)
기 능 : 문법에 맞게 문장이 입력되었는지 확인
*/
int use_vector_error_check(char* read_line, vector<string> expr)
{
	int mark = 0;
	double compare = 0;

	/* 연산자가 없거나 피연산자의 개수가 적은 경우 */
	for (int i = 0; i < expr.size(); i++) {
		if (expr[i] == "MINUS" || expr[i] == "IF" || expr[i] == "ADD" || expr[i] == "NEG" || expr[i] == "EQUAL" || expr[i] == "POS" || expr[i] == "ZERO" || expr[i] == "IF/THEN/ELSE" || expr[i] == "TIMES")
			mark = 1;

		if (expr[i] == "POS" || expr[i] == "NEG" || expr[i] == "ZERO") {
			if (expr.size() < 2) {
				printf("피연산자의 개수가 적습니다.\n");
				return 1;
			}
		}

		if (expr[i] == "MINUS" || expr[i] == "IF" || expr[i] == "ADD" || expr[i] == "EQUAL" || expr[i] == "TIMES") {
			if (expr.size() < 3) {
				printf("피연산자의 개수가 적습니다.\n");
				return 1;
			}
		}

		if (expr[i] == "IF/THEN/ELSE") {
			if (expr.size() < 4) {
				printf("피연산자의 개수가 적습니다.\n");
				return 1;
			}
		}
	}

	if (mark == 0) {
		printf("undefined\n");
		return 1;
	}
}


/**
함 수 : check_overlap(char read_line[])
기 능 : DEFUN에 이미 정의되어있는 명령어인지 확인
*/
int check_overlap(char read_line[])
{
	FILE* fp;
	vector<string> expr;
	vector<string> defun;

	expr = make_token(read_line, sizeof(read_line));


	fopen_s(&fp, "defun.txt", "r");

	while (!feof(fp))
	{
		char store_line[BUFFER_SIZE] = { 0, };

		fgets(store_line, BUFFER_SIZE, fp);

		if (store_line[0] == '\0')
			break;

		defun = make_token(store_line, sizeof(store_line));

		if (expr[0] == defun[0]) {
			fclose(fp);

			return 1;
		}
	}

	fclose(fp);

	return 0;
}