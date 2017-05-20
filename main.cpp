#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// >	++ptr
// < 	--ptr
// + 	++*ptr
// - 	--*ptr
// .	putchar(*ptr)
// , 	*ptr=getchar()
// [	while(*ptr){
// ]	}	

#define TOKENS "><+-.,[]"

#define CODE_SEGMENT_SIZE 30000
#define STACK_SEGMENT_SEZE 1000
#define DATA_SEGMENT_SIZE 30000

typedef void(*Callback)(void); //函数指针

//虚拟机 
struct{
	char cs[CODE_SEGMENT_SIZE];
	long ip;
	char ss[STACK_SEGMENT_SEZE];
	long sp;
	char ds[DATA_SEGMENT_SIZE];
	long bp;
	Callback fn[128];
} vm;

void vm_forwad(){
	vm.bp=(vm.bp+1)%DATA_SEGMENT_SIZE;
}

void vm_backward(){
	vm.bp=(vm.bp-1)%DATA_SEGMENT_SIZE;
}

void vm_increment(){
	vm.ds[vm.bp]++;
}

void vm_decrement(){
	vm.ds[vm.bp]--;
}

void vm_input(){
	vm.ds[vm.bp]=getchar();
}

void vm_outpur(){
	putchar(vm.ds[vm.bp]);
}


//利用栈存储进入while时的地址 
//处理[ 如果指针指向的单元值为零，向后跳转到对应的]指令的次一指令处
void vm_while_entry(){	
	if(vm.ds[vm.bp]){
		//非0,存储进入while前ip的值
		vm.ss[vm.sp]=vm.ip-1;
		vm.sp++;	
	}else{
		//为0，向后跳转到对应的]指令的次一指令处 
		int c=1;//处理嵌套的[[[]]] ,初始为1层 
		vm.ip++; 
		for(vm.ip;vm.cs[vm.ip]&&c;vm.ip++){
			if(vm.cs[vm.ip]=='[')
		 		c++;
 			else if(vm.cs[vm.ip]==']')
 				c--;
		}
	}
}

//处理]  如果指针指向的单元值不为零，向前跳转到对应的[指令的次一指令处
void vm_while_exit(){
	if(vm.ds[vm.bp]){
		vm.sp--;
		vm.ip=vm.ss[vm.sp];
	}	
}

//安装code 
void setup(){
	memset(&vm,0,sizeof(vm));
	vm.fn['>']=vm_forwad;
	vm.fn['<']=vm_backward;
	vm.fn['+']=vm_increment;
	vm.fn['-']=vm_decrement;
	vm.fn['.']=vm_outpur;
	vm.fn[',']=vm_input;
	vm.fn['[']=vm_while_entry;
	vm.fn[']']=vm_while_exit;
	
	int i;
	int c;
	for(int i=0;(c=getchar())!=EOF;){
		if(strchr(TOKENS,c)){
			vm.cs[i]=c;
			i++;
		}
	}
}

void run(){
	while(vm.cs[vm.ip]){
		vm.fn[vm.cs[vm.ip]]();
		vm.ip++;
	}
}

int main(int argc,char* argv[]){
	if(argc>1){
		freopen(argv[1],"r",stdin);
	}
	setup();
	run();
	return 0;
}