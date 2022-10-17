int main() {
  int fat, fat2;
  int i, j;
  int n, m = 100;
  int sum = 0;

  for (j = 1; j <= n; j++) {
    for (i = 1; i <= j; i++) {
      fat = fat * i;
    }

    for (i = 1; i <= m; i++) {
      fat2 = fat2 * i;
    }

    sum += fat + fat2;
  }

  return 0;
}