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


tk_tree* parser(int s_p, int f_p){
	int i,  j, k, bgng = 1;
	tk_tree* top = NULL;
	tk_tree* tt = NULL;
	
	// parse one number
	if(f_p - s_p == 0){
		top = new_tkt_cell();
		
		for(j = 0; j < tok[s_p]->len; j++){
			top->chars[j] = tok[s_p]->chars[j];
			
		}
		
		top->len = tok[s_p]->len;
		top->type = tok[s_p]->type;
		
		return top;
	}
	
	// parse an expression
	for(i = s_p; i < f_p; i++){
		
		if(tok[i]->chars[0] == ')' && i == f_p-1)
			continue;
		else if(tok[i]->chars[0] == '('){
			j = i+1;
			int counter = 1;
			while(j < f_p-1 && counter != 0){
				if(tok[j]->chars[0] == '(')
					counter++;
				if(tok[j]->chars[0] == ')')
					counter--;
				j++;
				
			}
			if(j != f_p - 1)
				j--;
				
			tk_tree* new_top;
			new_top = parser(i+1, j);
			
			i = j;
			if(top == NULL){
				
				top = new_top;
				tt = top;
			}
			else if(tt->right != NULL && tt->right->type == 2){
				tt->right->right = new_top;
			}
			else
				top->right = new_top;	
			
			continue;		
		}
		
		if(bgng){
			// constant
			if(tok[i]->type == 5){
				top = new_tkt_cell();
				tt = top;
				top->left = new_tkt_cell();
				
				for(k = 0; k<tok[i]->len; k++){
					top->left->chars[k] = tok[i]->chars[k];
				}
				
				top->left->len = tok[i]->len;
				top->left->type = tok[i]->type;
			}
			
			// number
			if(tok[i]->type == 4){
				top = new_tkt_cell();
				tt = top;
				top->left = new_tkt_cell();
				
				for(k = 0; k<tok[i]->len; k++){
					top->left->chars[k] = tok[i]->chars[k];
				}
				
				top->left->len = tok[i]->len;
				top->left->type = tok[i]->type;
			}
			// operator
			if(tok[i]->type == 3){
				
				if(top->type == 3 || top->type == 2){
					tk_tree* new_top = new_tkt_cell();
					new_top->left = top;
					top = new_top;
					tt = top;
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
				top = new_tkt_cell();
				tt = top;

				for(k = 0; k < tok[i]->len; k++)
					top->chars[k] = tok[i]->chars[k];
				
				top->len = tok[i]->len;
				top->type = tok[i]->type;
			}
			continue;
		}
		
		// number
		if(tok[i]->type == 4 || tok[i]->type == 5){
			if(tt->chars[0] == '+' || tt->chars[0] == '-'){
				top->right = new_tkt_cell();
				
				for(k = 0; k < tok[i]->len; k++){
					top->right->chars[k] = tok[i]->chars[k];
				}
				
				top->right->len = tok[i]->len;
				top->right->type = tok[i]->type;
			}
			if(tt->chars[0] == '*' || tt->chars[0] == '/' || tt->type == 2){
				tt->right = new_tkt_cell();
				
				for(k = 0; k < tok[i]->len; k++){
					tt->right->chars[k] = tok[i]->chars[k];
				}
				
				tt->right->len = tok[i]->len;
				tt->right->type = tok[i]->type;
			}
		}
		// operator
		if(tok[i]->type == 3){
			if(tok[i]->chars[0] == '+' || tok[i]->chars[0] == '-'){
				tk_tree* new_top = new_tkt_cell();
				new_top->chars[0] = tok[i]->chars[0];
				new_top->len = tok[i]->len;
				new_top->type = tok[i]->type;
				new_top->left = top; 
				top = new_top;
				tt = top;
			}
			
			if(tok[i]->chars[0] == '*' || tok[i]->chars[0] == '/'){
				tk_tree* new_left = tt->right;
				tt->right = new_tkt_cell();
				tt = tt->right;
				tt->left = new_left;
				
				tt->chars[0] = tok[i]->chars[0];
				tt->len = tok[i]->len;
				tt->type = tok[i]->type;
			}
		}
		
		// function
		if(tok[i]->type == 2){
			tt->right = new_tkt_cell();
			
			for(k = 0; k < tok[i]->len; k++)
				tt->right->chars[k] = tok[i]->chars[k];
			
			tt->right->len = tok[i]->len;
			tt->right->type = tok[i]->type;		
		}
		
	}
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
    		return l + r;
    		
    	if(node->chars[0] == '-')
    		return l - r;
    	
    	if(node->chars[0] == '*')
    		return l * r;
    		
    	if(node->chars[0] == '/')
    		return l / r;
	}
	
	if(node->type == 2){
		if(node->chars[0] == 's' && node->chars[1] == 'q' && node->chars[2] == 'r' && node->chars[3] == 't')
			return sqrt(r);
		else if(node->chars[0] == 's' && node->chars[1] == 'i' && node->chars[2] == 'n')
			return sin(r);
		else if(node->chars[0] == 'c' && node->chars[1] == 'o' && node->chars[2] == 's')
			return cos(r);
		else if(node->chars[0] == 'l' && node->chars[1] == 'o' && node->chars[2] == 'g')
			return log(r);
	}

	return l;
    
} 

void output_f(char* ofname, float result){
	FILE* ofp = fopen(ofname, "w");
	fprintf(ofp, "%f", result);
	fclose(ofp);
}

int main()
{	
	char* ifname = "ae.txt";
	lexer(ifname);
	
	tk_tree* top;
	top = parser(0, ti);
		
	float result = compute(top);
	
	
	
	char* ofname = "result.txt";
	output_f(ofname, result);
	
	exit(0);
}

