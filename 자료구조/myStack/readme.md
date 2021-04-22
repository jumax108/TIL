# 기본적인 Stack을 직접 구현
기본적인 push, pop과 같은 간단한 기능만을 구현

## stack Class

최초 생성 시, 고정된 크기의 배열을 동적할당해서 사용하는 stack<br/>
기본적으로 쓰레드에서 사용하는 stack 같은 느낌으로 구현

* 변수

  * ```c++ 
    unsigned int capacity
    ```

    * 현재 스택에서 동적할당 받은 배열의 인덱스 수

  * ```c++
    int* value
    ```

    * 실제로 값이 들어갈 배열

  * ```c++
    alignas(64) unsigned int topIndex
    ```

    * push 하면 값이 들어갈 인덱스 위치
    * push pop에 따라 수시로 값이 변경될 것이기 때문에 alignas로 캐시라인 구별

* 함수

  * ```c++
    bool push(int in)
    ```

    * 배열의 마지막에 in 값을 추가
    * 성공하면 true, 실패하면 false를 반환

  * ```c++ 
    bool pop(int* const out)
    ```

    * 배열의 마지막에 있는 값을 out에 넣고 값을 배열에서 삭제
    * 성공하면 true, 실패하면 false를 반환

  * ```c++
    void getSize(unsigned int* const out)
    ```

    * 배열에 있는 현재 원소의 수를 out 변수에 대입

  * ```c++
    void getCapacity(unsigned int* const out)
    ```

    * 배열이 현재 할당받은 전체 인덱스 수를 out 변수에 대입

## variableLengthStack Class

스택이 가득차면 자동으로 스택 크기를 늘려주는 방식으로 구현<br/>
이전 크기의 2배로 스택을 다시 할당 받음

* 위 스택을 상속받음

* 추가 함수

  * ```c++
    bool resize(unsigned int capapcity)
    ```

    * 배열을 capacity 만큼 재할당 받음
    * 할당에 실패하면 false, 할당에 성공하면 true를 반환

  * ```c++
    bool push(int in)
    ```

    * 배열의 마지막에 in 값을 추가
    * 배열이 가득차면 배열 크기를 capacity * 2로 재할당
    * 성공하면 true를 리턴 

## STL stack과 push 속도 비교

push를 천만번 진행했을 때 시간 측정으로 비교

STL stack: 818ms

stack: 35ms (reserve 10000000)

variableLengthStack: 69ms (reserve 1)

확실히 stl stack의 속도가 느리긴하다. <br/>
하지만 stl 사용의 목적이 최고의 속도가 아니기 때문에 속도 때문에 사용하지 않을 이유는 없을 것 같다.<br/>
특정 환경에 특화된 자료구조를 만든다면 allocator를 만들어서 stl에 적용하는 것이 맞는 것 같다.<br/>
언젠가 allocator를 직접 구현해서 stl stack의 속도를 올려보고 싶다. 