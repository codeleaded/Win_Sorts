// #include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"
// #include "C:/Wichtig/System/Static/Library/Victor2.h"
// #include "C:/Wichtig/System/Static/Library/Vdctor2.h"
// #include "C:/Wichtig/System/Static/Library/Complex.h"
// #include "C:/Wichtig/System/Static/Library/TransformedView.h"
// #include "C:\Wichtig\System\Static\Container\Vector.h"

#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Container/Vector.h"

#define DATA_SIZE	(100000)
#define TYPE		float
#define MODE_MAX	3

TYPE* data;

Timepoint start_tp;
Timepoint end_tp;

char mode;
char sorted;
Thread sorter;

void Sort_Swap(void* a,void* b){
	TYPE swap = *(TYPE*)a;
	*(TYPE*)a = *(TYPE*)b;
	*(TYPE*)b = swap;
}
char Sort_Cmp(void* a,void* b){
	TYPE a1 = *(TYPE*)a;
	TYPE b1 = *(TYPE*)b;
	return a1 > b1;
}
char Sort_CmpG(void* a,void* b){
	TYPE a1 = *(TYPE*)a;
	TYPE b1 = *(TYPE*)b;
	return a1 > b1 ? 1 : (a1 < b1 ? -1 : 0);
}

long Search_Linear(TYPE* array,long size,TYPE* searched){
	for(long i = 0;i<size;i++){
		if(Sort_Cmp(array + i,searched)) return i;
	}
	return -1;
}
long Search_Binary(TYPE* array,long size,TYPE* searched){
	for(int i = size / 2;;){
		if(size==0) return -1;
		
		char cmp = Sort_CmpG(array + i,searched);
		if(cmp==0) return i;
		else if(cmp<0){
			size = i;
			i = size / 2;
		}else if(cmp<0){
			array += i + 1;
			size = size - (i + 1);
			i += 1 + size / 2;
		}
	}
}

void Sort_Bubble(TYPE* array,long size){
	for(long i = 0;i<size;i++){
		for(long j = i;j<size;j++){
			if(i==j) continue;
			if(Sort_Cmp(array + i,array + j))
				Sort_Swap(array + i,array + j);

			//Thread_Sleep_U(50);
		}
	}
}
void Sort_Merge(TYPE* array,long size){
	if(size==1L) return;
	
	long size1 = size / 2L;
	long size2 = size - size1;

	//TYPE* subarray1 = array;
	TYPE* subarray1 = malloc(sizeof(TYPE) * size1);
	memcpy(subarray1,array,sizeof(TYPE) * size1);

	//TYPE* subarray2 = array + size1;
	TYPE* subarray2 = malloc(sizeof(TYPE) * size2);
	memcpy(subarray2,array + size1,sizeof(TYPE) * size2);

	Sort_Merge(subarray1,size1);
	Sort_Merge(subarray2,size2);

	long count1 = 0L;
	long count2 = 0L;

	for(long i = 0L;i<size;i++){
		if(count1==size1)											array[i] = subarray2[count2++];
		else if(count2==size2)										array[i] = subarray1[count1++];
		else if(Sort_Cmp(subarray1 + count1,subarray2 + count2)) 	array[i] = subarray2[count2++];
		else 														array[i] = subarray1[count1++];
	}

	free(subarray2);
	free(subarray1);
}
void Sort_Quick(TYPE* array,long size){
	if(size<=1) return;
	
	long pivot = size - 1;

	long i = -1;
	long j = 0;
	for(;j<size-1;j++){
		if(Sort_Cmp(array + pivot,array + j))
			if(Sort_Cmp(array + (++i),array + j))
				Sort_Swap(array + i,array + j);
	}
	Sort_Swap(array + (++i),array + pivot);

	Sort_Quick(array,i);
	Sort_Quick(array + (i + 1),size - (i + 1));
}

void* Sort_Executer(void* a){
	while(sorter.running){
		while(sorted){ Thread_Sleep_M(10); }
		
		start_tp = Time_Nano();
	
		if(mode==0) 		Sort_Bubble(data,DATA_SIZE);
		else if(mode==1) 	Sort_Merge(data,DATA_SIZE);
		else if(mode==2) 	Sort_Quick(data,DATA_SIZE);
		
		end_tp = Time_Nano();
		sorted = 1;
	}
	return NULL;
}

void Sort_Shuffle(){
	for(int i = 0;i<DATA_SIZE;i++){
		data[i] = (float)Random_f64_MinMax(0.0f,1.0f);
	}
}

void Setup(AlxWindow* w){
	data = malloc(sizeof(TYPE) * DATA_SIZE);
	memset(data,0,sizeof(TYPE) * DATA_SIZE);

	Sort_Shuffle();

	mode = 2;
	sorted = 0;
	start_tp = 0;
	end_tp = 0;

	ResizeAlxFont(16,16);

	sorter = Thread_New(NULL,Sort_Executer,NULL);
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
		Sort_Shuffle();
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
			RenderRect(x,y,F32_Ceil(dx),dy,(!sorted ? RED : GREEN));
			place++;
		}
	}

	String str = String_Format("Mode: %d, Size: %d, Sec Taken: %f",mode,DATA_SIZE,Time_Elapsed(start_tp,end_tp));
	RenderCStrSize(str.Memory,str.size,0,0,WHITE);
	String_Free(&str);
}
void Delete(AlxWindow* w){
	Thread_Stop(&sorter);
	//Thread_Term(&sorter,NULL);

    if(data) free(data);
	data = NULL;
}

int main(){
    if(Create("Sorts",1200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}