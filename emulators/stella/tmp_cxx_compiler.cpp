class Foo {
	int a;
};
int main(int argc, char **argv)
{
	Foo *a = new Foo();
	delete a;
	return 0;
}
