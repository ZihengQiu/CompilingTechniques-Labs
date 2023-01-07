int main()	//This is a comment
{
	int IntA = 1;
	const int ConstIntA = 2;
	int a = 1, b = 2, c = 3;

	int array[1] = {1};

	if(a<b || a<c && b<c || a==b || a==c)
	{
		a = 4;
	}else{
		b=  5;
	}

	while(a > b)
	{
		a = 6;
		break;
	}
	
	return 0;
}