int main() {
	int sum = 10;
  int prod = 1;
	int n = 100;
	for (int i = 20; i < n; i++) {
    for (int j = 20; j < n*i; j++) {
      prod *= j;
    }
		sum += i;
	}
}