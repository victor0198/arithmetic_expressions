#include <stdio.h>
#include <stdlib.h>

typedef struct tokens{
	char chars[10];
	int len;
	int type;
}token;

token* new_token(){
	token* tk = (token*) malloc(sizeof(token));
	
	tk->len = 0;
	tk->type = 0; 
	
	return tk;
}

token* tok[1000];
int ti;

void read_characters(char* ifname, char chs[1000], int* len){
	FILE* ifp = fopen(ifname, "r");
	
	char c;
	int i = 0;
	
	while((c = fgetc(ifp)) != EOF)
		chs[i++] = c;
	
	*len = i;
	
	fclose(ifp);
}

void split(char chs[1000], int len){
	/* TYPES OF TOKENS
	type 1: parantheses
	type 2: functions
	type 3: operators
	type 4: numbers
	type 5: constants
	*/
	int i, index = -1, chs_i = 0, type = 0, changed = 0;
	for(i = 0; i < len; i++){
		if(chs[i] == ' ')
			continue;
			
		changed = 0;
		
		// parantheses
		if(chs[i] == 40 || chs[i] == 41){
			type = 1;
			index++;
			tok[index] = new_token();
			chs_i = 0;

			tok[index]->chars[chs_i] = chs[i];
			tok[index]->len++;
			tok[index]->type = type;
			continue;	
		}
		// constant
		if(chs[i] == 'e' || (chs[i] == 'p' || (chs[i] == 'i' && type == 5))){
			if(type != 5){
				type = 5;
				changed = 1;
				printf("const\n");

			}
		}
		// function
		if(chs[i] >= 97 && chs[i] <=122)
			if(type != 2 && type != 5){
				type = 2;
				changed = 1;
			}
		// operator
		if(chs[i] == 42 || chs[i] == 43 || chs[i] == 45 || chs[i] == 47)
			if(type != 3){
				type = 3;
				changed = 1;
			}
		// numbers
		if(chs[i] >= 48 && chs[i] <=57 || chs[i] == 46)
			if(type != 4){
				type = 4;
				changed = 1;
			}
		
		if(changed){
			index++;
			tok[index] = new_token();
			chs_i = 0;
		}else
			chs_i++;

		tok[index]->chars[chs_i] = chs[i];
		tok[index]->len++;
		tok[index]->type = type;
	}

	ti = index + 1;
}

void lexer(char* ifname){
	char characters[1000];
	int chs_len;
	
	// read the function from the file
	read_characters(ifname, &characters, &chs_len);
		
	// split the function into tokens
	split(&characters, chs_len);
}

typedef struct tk_tree{
	char chars[10];
	int len;
	struct tk_tree* left;
	struct tk_tree* right;
	int type;
}tk_tree;

tk_tree* new_tkt_cell(){
	tk_tree* tkt = (tk_tree*) malloc(sizeof(tk_tree));
	
	tkt->len = 0;
	tkt->left = NULL; 
	tkt->right = NULL;
	
	return tkt;
}

void print_tree(struct tk_tree* node)
{ 
	if (node == NULL)
	{
		printf(".\n");
		return;
	}
		
	printf("L\n");
	print_tree(node->left); 
	printf("R\n");
	print_tree(node->right);
    
    int  i;
    for(i=0;i<node->len;i++)
    printf("%c\n", node->chars[i]);
	
} 

tk_tree* parser(int s_p, int f_p){
	int i,  j, k, bgng = 1;
	tk_tree* top = NULL;
	
	// parse one number
	if(f_p - s_p == 0){
		top = new_tkt_cell();
		
		printf("\n\ndifference=0\n");
		for(j = 0; j < tok[s_p]->len; j++){
			top->chars[j] = tok[s_p]->chars[j];
			printf("%c\n", top->chars[j]);
		}
		
		top->len = tok[s_p]->len;
		top->type = tok[s_p]->type;
		
		print_tree(top);
		printf("\n\n");
		return top;
	}
	
	// parse an expression
	for(i = s_p; i < f_p; i++){
		printf("   ");
		for(k = 0; k<tok[i]->len; k++){
			printf("%c", tok[i]->chars[k]);
		}
		printf("\n");
		
		if(tok[i]->chars[0] == ')' && i == f_p-1)
			continue;
		else if(tok[i]->chars[0] == '('){
			printf("1)yep\n");
			j = i+1;
			int counter = 1;
			while(j < f_p-1 && counter != 0){
				printf("counter = %d| %c\n", counter, tok[j]->chars[0]);
				if(tok[j]->chars[0] == '(')
					counter++;
				if(tok[j]->chars[0] == ')')
					counter--;
				j++;
				
			}
			if(j != f_p - 1)
				j--;
				
			tk_tree* new_top;
			printf("2)i %d j %d\n", i+1,j);
			
			printf("----P----\n");
			new_top = parser(i+1, j);
			print_tree(new_top);
			printf("-----Pe----\n");
			
			printf("3)O\n");
			i = j;
			printf("4)K\n");
			if(top == NULL){
				
				printf("5)NEW TOP\n");
				top = new_top;
				printf("6)NEW TOP - OK\n");
			}
			else if(top->right != NULL && top->right->right != NULL && top->right->right->type == 2){
				printf("5)TOP->right->right\n");
				top->right->right->right = new_top;
				printf("6)TOP->right->right - OK\n");
			}
			else if(top->right != NULL && top->right->type == 2){
				printf("5)TOP->right\n");
				top->right->right = new_top;
				printf("6)TOP->right - OK\n");
			}
			else
				top->right = new_top;	
			
			printf("7)ok\n");
//			bgng = 0;
			continue;		
		}
		
		if(bgng){
			// constant
			if(tok[i]->type == 5){
				printf("bgng const\n");
				top = new_tkt_cell();
				top->left = new_tkt_cell();
				
				for(k = 0; k<tok[i]->len; k++){
					top->left->chars[k] = tok[i]->chars[k];
				}
				
				top->left->len = tok[i]->len;
				top->left->type = tok[i]->type;
			}
			
			// number
			if(tok[i]->type == 4){
				printf("bgng nr\n");
				top = new_tkt_cell();
				top->left = new_tkt_cell();
				
				for(k = 0; k<tok[i]->len; k++){
					top->left->chars[k] = tok[i]->chars[k];
				}
				
				top->left->len = tok[i]->len;
				top->left->type = tok[i]->type;
			}
			// operator
			if(tok[i]->type == 3){
				printf("bgng op\n");
				
				if(top->type == 3 || top->type == 2){
					tk_tree* new_top = new_tkt_cell();
					new_top->left = top;
					top = new_top;
				}
				
				for(k = 0; k < tok[i]->len; k++){
					top->chars[k] = tok[i]->chars[k];
				}
				
				top->len = tok[i]->len;
				top->type = tok[i]->type;
				bgng = 0;
			}
			
			// function
			if(tok[i]->type == 2){
				printf("bgng fnc\n");
				top = new_tkt_cell();

				for(k = 0; k < tok[i]->len; k++)
					top->chars[k] = tok[i]->chars[k];
				
				top->len = tok[i]->len;
				top->type = tok[i]->type;
//				bgng = 0;		
			}
			continue;
		}
		
		// number
		if(tok[i]->type == 4 || tok[i]->type == 5){
			printf("---num---\n");
			if( (top->chars[0] == '+' || top->chars[0] == '-') && top->right != NULL &&(top->right->chars[0] == '*' || top->right->chars[0] == '/') ){
				printf("way 1\n");
				top->right->right = new_tkt_cell();
				
				for(k = 0; k < tok[i]->len; k++){
					top->right->right->chars[k] = tok[i]->chars[k];
				}
				
				top->right->right->len = tok[i]->len;
				top->right->right->type = tok[i]->type;
			}
			
			else if(top->chars[0] == '+' || top->chars[0] == '-' || top->chars[0] == '-' || top->chars[0] == '/'){
				printf("way 2\n");
				top->right = new_tkt_cell();
				
				for(k = 0; k < tok[i]->len; k++){
					top->right->chars[k] = tok[i]->chars[k];
				}
				
				top->right->len = tok[i]->len;
				top->right->type = tok[i]->type;
			}
			/*
			if(tt->chars[0] == '*' || tt->chars[0] == '/' || tt->type == 2){
				tt->right = new_tkt_cell();
				
				for(k = 0; k < tok[i]->len; k++){
					tt->right->chars[k] = tok[i]->chars[k];
				}
				
				tt->right->len = tok[i]->len;
				tt->right->type = tok[i]->type;
			}*/
			printf("---num ok---\n");
			printf("---------\n");
			print_tree(top);
			printf("---------\n");
		}
		// operator
		if(tok[i]->type == 3){
			printf("operator\n");
			
			if( (top->chars[0] == '+' || top->chars[0] == '-') && (tok[i]->chars[0] == '*' || tok[i]->chars[0] == '/') ){
				tk_tree* new_left = top->right;
				top->right = new_tkt_cell();
				top->right->left = new_left;
								
				top->right->chars[0] = tok[i]->chars[0];
				top->right->len = tok[i]->len;
				top->right->type = tok[i]->type;
			}
			else if(tok[i]->chars[0] == '+' || tok[i]->chars[0] == '-' || tok[i]->chars[0] == '*' || tok[i]->chars[0] == '/'){
				tk_tree* new_top = new_tkt_cell();
				new_top->chars[0] = tok[i]->chars[0];
				new_top->len = tok[i]->len;
				new_top->type = tok[i]->type;
				new_top->left = top; 
				top = new_top;
			}
			
			/*if(){
				tk_tree* new_left = tt->right;
				tt->right = new_tkt_cell();
				tt = tt->right;
				tt->left = new_left;
				
				tt->chars[0] = tok[i]->chars[0];
				tt->len = tok[i]->len;
				tt->type = tok[i]->type;
			}*/
			printf("operator - ok\n");  
			printf("---------\n");
			print_tree(top);
			printf("---------\n");
		}
		
		// function
		if(tok[i]->type == 2){
			printf("function\n");
			if( (top->chars[0] == '+' || top->chars[0] == '-') && top->right != NULL &&(top->right->chars[0] == '*' || top->right->chars[0] == '/') ){
				top->right->right = new_tkt_cell();
			
				for(k = 0; k < tok[i]->len; k++)
					top->right->right->chars[k] = tok[i]->chars[k];
				
				top->right->right->len = tok[i]->len;
				top->right->right->type = tok[i]->type;
			}
			else{
				top->right = new_tkt_cell();
			
				for(k = 0; k < tok[i]->len; k++)
					top->right->chars[k] = tok[i]->chars[k];
				
				top->right->len = tok[i]->len;
				top->right->type = tok[i]->type;
			}
			
					
		}
		
	}
	printf("FIN\n");
	return top;
}

float compute(struct tk_tree* node)
{ 
	if (node == NULL)
		return;
		
	if(node->type == 4)
		return atof(node->chars);
		
	if(node->type == 5){
		if(node->chars[0] == 'p' && node->chars[1] == 'i')
			return 3.14159265;
		else if(node->chars[0] == 'e')
			return 2.71828182;
	}	
	
	float l = 0; 
	l = compute(node->left); 
	
	float r = 0;
	r = compute(node->right);
    
    float result;
    if(node->type == 3){
    	if(node->chars[0] == '+')
    		result = l + r;
    		
    	if(node->chars[0] == '-')
    		result = l - r;
    	
    	if(node->chars[0] == '*')
    		result = l * r;
    		
    	if(node->chars[0] == '/')
    		result = l / r;
    	
//    	printf("3\n");
    	printf("RES: %f | l=%f, r=%f\n", result, l, r);
		return result;
	}
	
	if(node->type == 2){
		printf("type 2\n");
		if(node->chars[0] == 's' && node->chars[1] == 'q' && node->chars[2] == 'r' && node->chars[3] == 't')
			result = sqrt(r);
		else if(node->chars[0] == 's' && node->chars[1] == 'i' && node->chars[2] == 'n')
			result = sin(r);
		else if(node->chars[0] == 'c' && node->chars[1] == 'o' && node->chars[2] == 's')
			result = cos(r);
		else if(node->chars[0] == 'l' && node->chars[1] == 'o' && node->chars[2] == 'g')
			result = log(r);
		
		printf("RES: %f | l=%f, r=%f\n", result, l, r);
		return result;
	}

//	printf("5\n");
	printf("RES: %f | l=%f, r=%f\n", l, l, r);
	return l;
    
} 

void output_f(char* ofname, float result){
	FILE* ofp = fopen(ofname, "w");
	fprintf(ofp, "%f", result);
	fclose(ofp);
}

void print_t(){
	int i, j;
	for(i=0;i<ti;i++){
		for(j=0;j<tok[i]->len;j++)
			printf("%c", tok[i]->chars[j]); 
		printf("\n");
	}
}

int main()
{	
	char* ifname = "ae.txt";
	lexer(ifname);
	print_t();
	tk_tree* top;
	top = parser(0, ti);
	
	print_tree(top);	
	float result = compute(top);
	
	
	
	char* ofname = "result.txt";
	output_f(ofname, result);
	
	exit(0);
}

