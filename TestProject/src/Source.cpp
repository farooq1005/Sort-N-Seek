struct MyStruct {
  int myVar = 0;
  const char* name = "Farooq";
  char myChar = 'c';
  double myDouble = 0.2;
};

MyStruct func() {
  MyStruct obj;
  obj.myDouble = 5.5;

  return obj;
}

int main() {
  MyStruct obj = func();

  obj.myVar;

  return 0;
}