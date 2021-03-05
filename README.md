# BigInt
Big integer library in C (WIP)

## Warning
This library is not finished and not suitable for a serious project usage. It's still pretty slow (few seconds) for heavy operations on really huge integers like the ones used in RSA operations.

## Documentation

### Structures
Big integers storage structure
```
struct big_int {
	bool sign; 			// 0 for positive integers, 1 for negative
	uint8_t* buffer;	// array of integers
	uint32_t size;		// size of the array
};
```

Euclidean division result storage structure
```
struct big_int_eucl {
	big_int* q;	// Quotient
	big_int* r; // Remainder
};
```

### Functions

| Category | Prototype | Description |
| -------- | --------- | ----------- |
| **Memory** | `big_int* big_int_alloc()` | Allocate a memory space to store big integers structure, initialize it to 0 |
| **Memory** | `big_int* big_int_create(int32_t value)` | Allocate and store a new 32bit integer into a big_int structure |
| **Memory** | `big_int* big_int_load(const char* s)` | Load a big_int struct from a string |
| **Memory** | `big_int* big_int_from_buff(const char* buff)` | Generate an integer from a byte-buffer |
| **Memory** | `void big_int_destroy(big_int* a)` | Un-allocate memory from big_int |
| **Memory** | `void big_int_eucl_destroy(big_int_eucl* res)` | |
| **Memory** | `void big_int_cpy(big_int* dst, big_int* src)` | Copy the properties of the big_int src into the big_int dst |
| **Display** | `void big_int_print(big_int* a)` | Print a big_int |
| **Display** | `void big_int_println(big_int* a)` | Print a big int and a newline |
| **Format** | `void big_int_reduce(big_int* a)` | Remove leading zeroes |
| **Format** | `void big_int_shift(big_int* a, uint32_t shift)` | Shift the big_int number equivalent of a = a * (10 ^ shift) |
| **Format** | `void big_int_concat(big_int* a, big_int* b)` | Concatenate two big integers |
| **Format** | `big_int* big_int_frame(big_int* a, uint32_t start, uint32_t end)` | Return a selected piece "frame" of a big int, ex: frame(18745, 0, 2) = 18 |
| **Format** | `char* big_int_to_hex(big_int* a)` | Convert the big int to an hex string |
| **Format** | `uint32_t big_int_len(big_int* a)` | Return the number of digits in the number a|
| **Math** | `void big_int_abs(big_int* a)` | Take the absolute value of a big_int |
| **Math** | `void big_int_neg(big_int* a)` | Return the negative value of a big_int |
| **Math** | `int32_t big_int_cmp(big_int* a, big_int* b)` | Compare two big ints (return -1 if a < b, 1 if a > b, 0 if a = b) |
| **Math** | `void big_int_add(big_int* a, big_int* b)` | Add two big ints a & b and store the result in a |
| **Math** | `void big_int_sub(big_int* a, big_int* b)` | Substract two big ints a & b and store the result in a |
| **Math** | `void big_int_mul(big_int* a, big_int* b)` | Multiply two big ints a & b and store the result in a |
| **Math** | `void big_int_div(big_int* a, big_int* b)` | Divide two big ints a & b and store the result in a |
| **Math** | `big_int_eucl* big_int_eucl_div(big_int* a, big_int* b)` | Compute euclidean division |
| **Math** | `void big_int_mod(big_int* a, big_int* b)` | Compute the modulo a % b and store it in a |
| **Math** | `void big_int_exp(big_int* a, uint32_t e)` | Pow the big_int a to the exponent e (32bit uint) |
| **Math** | `big_int* big_int_modexp(big_int* base, big_int* e, big_int* mod)` | Fast modular exponentiation |
