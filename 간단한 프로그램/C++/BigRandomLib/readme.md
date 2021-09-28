# Big Random

this library support getting big random number.

random number range: 0 ~ 0xFFFFFFFFFFFFFFFF - 1 (unsigned long long)

[[한국어]](./docs/readme_kr.md)

___ ___

## Union

```C++
union UValueRange {
	unsigned long long value;
	unsigned char byte[8];
};
```

* check for how many use byte

## Class Variable

```c++
UValueRange valueRange;
```

* Set range of random value
* if set 50000, random function must return value of 0 ~ 49999 

```c++
int byteNum
```

* num of byte used by valueRange
* set GetRandomValue() function's loop count

___ ___

## Class Function

```c++
BigRandom(unsigned long long valueRange)
```

* Constructor
* set range of random value

```c++
void SetSeed(unsigned int seed)
```

* set seed for pseudorandom number generation in current thread

* wrapping srand

``` c++
unsigned long long GetRandomValue()
```

* return random value (0 ~ valueRange - 1)

