# switch case의 대한 분석

switch case와 if문의 차이점에 대한 분석

___ ___

## if 의 어셈블리 코드

val과 100을 비교하는 코드

```assembly
cmp dword ptr [val], 64h
jne main+18h
```

단순히 cmp를 해서 점프를 하는 코드이다.

흔히 생각하는 if의 구조 그 자체이다.



## switch case의 어셈블리 코드

val의 값에 대한 case 0, case 1, case 2, case 3 에 대한 코드

```assembly
mov         eax,dword ptr [val]  
mov         dword ptr [ebp-8],eax  
cmp         dword ptr [ebp-8],3  
ja          $LN7+7h (0611045h)  
mov         ecx,dword ptr [ebp-8]  
jmp         dword ptr [ecx*4+61104Ch]  
```

느낌이 좀 다르다.

0이면 case 0에 대한 코드를, 1이면 case 1에 대한 코드를 실행해야 하기에

cmp가 여러번 발생할 것 같지만 그렇지 않다

cmp는 단 한 번 발생한다.

___ ___

## 점프 테이블

컴파일러는 case 값이 일정하면 점프 테이블을 준비한다.

![switch_메모리뷰.png](../../images/switch_메모리뷰.png)

위 코드의 마지막 61104C 메모리 값이다.

그리고 저장된 값들은 각 case 가 시작되는 코드의 주소이다.

case 0의 해당하는 명령이 시작되는 주소가 0x0061023 인 것이다.

즉 컴파일러는 최초 cmp로 값이 case 범위에서 벗어나는지 체크하고 배열에 저장된 코드 주소로 jmp하는 것이 다이다.

case의 값이 많아져도 점프 테이블 크기가 증가하는 것이지 코드 량이 많아지지는 않는다.

즉 case가 많아질수록 if보다 매우 빠르게 작동한다.

___ ___

## 언제나 점프 테이블을 생성하는가

아니다.

case가 어느정도 일정할 때만 나온다.

case 0, case 1, case 3, case 4 처럼 중간에 몇 개 비는 것 정도는 문제가 안된다.

하지만 case 1, case 10000, case -8498, case 9999 처럼 값이 널뛰면 case 별로 cmp 코드가 생성된다.

즉 이 경우는 if를 적은 것과 다름이 없다.

___ ___

