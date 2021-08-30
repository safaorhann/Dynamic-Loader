int glob_var = 1;
const int aa = 11;

int sum(int a, int b){
  return a+b;
}

int max(int num1, int num2)
{
  static int static_int = 19;
  return num1 > num2 ? num1 : num2;  
}

int min(int num1, int num2)
{
    const int a = 11;
    return (num1 < num2) ? num1 : num2;
}

int globvar(){
  return glob_var;
}