# Hamsa
Kernels are nests of loops that process multidimensional arrays. **Hamsa** is a tool to extract kernels from programs and infer their asymptotic complexity. Figure 1 illustrates this:

<p align="center">
  <img alt="kernels" src="./assets/kernels.png" width="90%" height="80%"/></br>
</p>

For linear loop bounds, inferring the complexity is trivial. However, for non-linear bounds this task is more challenging. We intend to use the approach proposed by [Gulwani et al.](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/12/sas11-bound.pdf) to implement it.