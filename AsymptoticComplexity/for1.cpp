void MultMatriz(int** m1, int** m2, int m, int n, int q, int** m3) {

  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < q; k++) {
        m3[i][k] += m1[i][j] * m2[j][k];
      }
    }
  }
}