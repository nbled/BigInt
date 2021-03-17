# BigInt
Big integer library in C (WIP)

## Warning
This library is not faster that a proper implementation (cf. GMP), however it has "satisfying" results on RSA operation (\~1s for encryption/decryption)

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

