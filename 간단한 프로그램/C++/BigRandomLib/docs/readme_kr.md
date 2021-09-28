# Big Random

이 라이브러리로 큰 랜덤 값을 얻을 수 있습니다.

랜덤 값 범위: 0 ~ 0xFFFFFFFFFFFFFFFF - 1 (unsigned long long)

[[English]](../readme.md)

___ ___

## Union

```C++
union UValueRange {
	unsigned long long value;
	unsigned char byte[8];
};
```

* 몇 바이트를 사용하는지 확인하려는 용도입니다.

## 클래스 변수

```c++
UValueRange valueRange;
```

* 랜덤 값의 범위를 설정합니다.
* 만약 50000으로 설정하면, 랜덤 함수는 0에서 49999 사이의 값을 반환합니다.

```c++
int byteNum
```

* valueRange에서 사용하는 바이트 수
* GetRandomValue() 함수에서 반복횟수를 결정합니다.

___ ___

## 클래스 함수

```c++
BigRandom(unsigned long long valueRange)
```

* 생성자
* 랜덤 값의 범위를 설정합니다.

```c++
void SetSeed(unsigned int seed)
```

* 현재 스레드에서 의사 난수 생성을 위한 시드를 설정합니다.

* srand 함수를 래핑합니다.

``` c++
unsigned long long GetRandomValue()
```

* 0에서 ValueRange-1의 값을 리턴합니다.