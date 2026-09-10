#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/Algorithms.h"
#include "/home/codeleaded/System/Static/Container/Vector.h"

#define DATA_SIZE	(10000000)
#define TYPE		float
#define MODE_MAX	3

TYPE* data;

Timepoint start_tp;
Timepoint end_tp;

char mode;
char sorted;
Thread sorter;

void Type_Sort_Swap(void* a,void* b){
	TYPE swap = *(TYPE*)a;
	*(TYPE*)a = *(TYPE*)b;
	*(TYPE*)b = swap;
}
int Type_Sort_Cmp(const void* const a,const void* const b){
	TYPE a1 = *(TYPE*)a;
	TYPE b1 = *(TYPE*)b;
	return a1 > b1 ? 1 : (a1 < b1 ? -1 : 0);
}

void* Type_Sort_Executer(void* a){
	while(sorter.running){
		while(sorted){
			if(!sorter.running) return NULL;
			Thread_Sleep_M(10);
		}
		
		start_tp = Time_Nano();
	
		if(mode==0) 		Sort_Bubble(data,sizeof(TYPE),DATA_SIZE,Type_Sort_Cmp,Type_Sort_Swap);
		else if(mode==1) 	Sort_Merge(data,sizeof(TYPE),DATA_SIZE,Type_Sort_Cmp,Type_Sort_Swap);
		else if(mode==2) 	Sort_Quick(data,sizeof(TYPE),DATA_SIZE,Type_Sort_Cmp,Type_Sort_Swap);
		
		end_tp = Time_Nano();
		sorted = 1;
	}
	return NULL;
}
void Type_Sort_Shuffle(){
	for(int i = 0;i<DATA_SIZE;i++){
		data[i] = (float)Random_f64_MinMax(0.0f,1.0f);
	}
}

void Setup(AlxWindow* w){
	data = malloc(sizeof(TYPE) * DATA_SIZE);
	memset(data,0,sizeof(TYPE) * DATA_SIZE);

	Type_Sort_Shuffle();

	mode = 2;
	sorted = 0;
	start_tp = 0;
	end_tp = 0;

	ResizeAlxFont(16,16);

	sorter = Thread_New(NULL,Type_Sort_Executer,NULL);
	Thread_Start(&sorter);
}
void Update(AlxWindow* w){
	if(Stroke(ALX_KEY_UP).PRESSED){
		mode++;
		mode = mode % MODE_MAX;
	}
	if(Stroke(ALX_KEY_DOWN).PRESSED){
		mode--;
		mode = mode % MODE_MAX;
	}

	if(Stroke(ALX_MOUSE_L).PRESSED){
		Type_Sort_Shuffle();
		sorted = 0;
	}

	Clear(BLACK);

	int place = 0;
	for(int i = 0;i<DATA_SIZE;i++){
		TYPE t = data[i];
		
		float x = (float)i * ((float)GetWidth() / (float)DATA_SIZE);
		float y = (float)GetHeight() * (1.0f - t);
		float dx = (float)GetWidth() / (float)DATA_SIZE;
		float dy = (float)GetHeight() * t;
		
		if((int)x >= place){
			RenderRect(x,y,F32_Ceil(dx),dy,(!sorted ? BLUE : GREEN));
			place++;
		}
	}

	String str = String_Format("Mode: %d, Size: %d, Sec Taken: %f",mode,DATA_SIZE,Time_Elapsed(start_tp,end_tp));
	RenderCStrSize(str.Memory,str.size,0,0,WHITE);
	String_Free(&str);
}
void Delete(AlxWindow* w){
	sorter.running = 0;
	Thread_Join(&sorter);
	Thread_Free(&sorter);

    if(data) free(data);
	data = NULL;
}

int main(){
    if(Create("Sorts",1200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}