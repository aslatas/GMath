# GMath
A single header C++ vector/matrix/quaternion math library, based on the Handmade Math library: https://github.com/HandmadeMath/Handmade-Math

That library is well supported, more fully featured, and likely less buggy than this one, so I'd recommend it instead; GMath is my personal graphics math library, and it exists mostly as a platform for me to learn about SSE intrinsics. It's intended to be fast, but it's not optimized at the assembly level, and most functions haven't even been profiled thoroughly. If you're looking to squeeze as much performance out as possible, you're probably better off optimizing the math itself, without using custom vector/matrix types at all. 

GMath requires C++11, and compiles on MSVC, Clang, and GCC. To achieve this, I had to sacrifice a few operations, namely some helpful constructors - the different compilers have different rules about unnamed structs/unions.
