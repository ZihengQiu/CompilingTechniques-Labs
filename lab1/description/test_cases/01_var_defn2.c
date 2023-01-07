//test domain of global var define and local define
// int array[10];
// int array2[10][10];
// int array3[10][10][10];

// int MyAdd(){int z;return z;}
// // int MyAdd(int x, int y){int z = x + y;return z;}

// int MyAdd2(int x, int y, int z)
// {
//     int t = x + y + z;
// 	return t;
// }
// void MyAdd3()
// {
// 	int a = 1;
// }

// float MyAdd4(int a[][1])
// {
// 	float z = 1;
// 	return z;
// }
// float MyAdd4(int a[][1]){	int a; int b = 2; return z;}

int func(int aa) { int c = 666; return c;}

int main()
{
	// const int a = 1, b = 2;
	// int aa = 1, bb, cc = 3;
	// int c = 1, d = 2;
	// const int aa = 1;//, bb;
	// const int cc = 1, dd = 2;
	// const int ee[2] = {1,2};

	// int e[10];
	// e[1] = 1;
	// e[2] = 2;
	// int f[1] = {1};
	// int g[2] = {1, 2};
	// int h[2] = {3, 4};

	int a = 3, c = 7;
	int b = 5;
	int d = a + c * 10;

	// int e = -MyAdd(a+MyAdd(1,2), MyAdd(3,4));
	// int f = MyAdd2(1, 2, array[1]);
	// constint consta = 1;
	// const int consta2 = 2, consta3 = 4;
	// const int constarraya[2] = {0, 1};
	// int var;
	// int var2 = 6, var3 = 7;
	// int var4[2] = {8, 9};
	// int var5[1] = {10};
	// int var6[1] = {};

	// if(a <= a || b >=b || c >= c || d >= d)a = 1;else{	a = a + 2;}
	if(a <= b )a = 1;else{	a = a + 2;}
	// while (a >= 1){	a = a - 1;	a = a + 1;	break;}

	// {a = a + 2;{a = a + 3;}a = a + 4;{a = a + 5;}}
	// {
	// 	a = 2;
	// 	b = 3;
	// }
	// const int a6 = 10, a7 = 8;
	// int a8 = 0;
	// if(a<1) b=2; else c=3;
	// while(a+1<2) b=b+3*4;
	// {a=1;{b=2;}}
	// while(a<=2) {a=3;break;}
	// array[1] = a+1;
	return 0;
}
//int main(){int a = 3, c = 7;int b = 5;int d = a + c * 10; if(a <= a || b >=b || c >= c || d >= d)a = 1;else{	a = a + 2;}return 0;}

// int main(){if(a <= b )a = 1;else{	a = a + 2;}return 0;}

// int main(){return 0;}int chx(){return 0;}int mwt = 666;
// int chx(){return 0;}
// float MyAdd4(int a[][1]){int a; int b = 2; return z;}
// float MyAdd44(int A){int z; int b = 2; return z;}
