# 파일입출력 시 FILE, stream 무엇이 더 빠를까
## 예제 코드 작성

FILE을 이용한 입출력 예제

```C++
FILE* file;
for (int loopCnt = 0; loopCnt < LOOP_NUM; ++loopCnt) {

    profiler.profileBegin("FILE read");
    _wfopen_s(&file, L"test.txt", L"rb");
    fread(buf, 1, 20971520, file);
    fclose(file);
    profiler.profileEnd("FILE read");
}
```

```C++
for (int loopCnt = 0; loopCnt < LOOP_NUM; ++loopCnt) {

    profiler.profileBegin("FILE write");
    _wfopen_s(&file, L"test.txt", L"wb");
    fwrite(buf, 1, 20971520, file);
    fclose(file);
    profiler.profileEnd("FILE write");
}
```

stream을 이용한 입출력 예제

```c++
ifstream stream;
for (int loopCnt = 0; loopCnt < LOOP_NUM; ++loopCnt) {

    profiler.profileBegin("stream read");
    stream.open(L"test.txt");
    stream.read(buf, 20971520);
    stream.close();
    profiler.profileEnd("stream read");
}
```
```c++
ofstream ostream;
for (int loopCnt = 0; loopCnt < LOOP_NUM; ++loopCnt) {

    profiler.profileBegin("stream write");
    ostream.open(L"test.txt");
    ostream.write(buf, 20971520);
    ostream.close();
    profiler.profileEnd("stream write");
}
```


## 결과

                Name |         Average |             Min |             Max |            Call
           FILE read |     5623.394 us |     5267.400 us |     7698.400 us |             100 
         stream read |    27361.939 us |    27431.700 us |    33358.600 us |             100 
          FILE write |    12035.806 us |    11382.900 us |   572817.600 us |             100 
        stream write |    65898.587 us |    54536.600 us |   104869.100 us |             100 
읽고 쓴 데이터 량은 18MB 정도였다.<br/>

전반적으로 FILE이 더 빠른 속도를 보여준다. 쓰기의 경우 max 값이 크긴 하지만 평균적으로는 압도하였다. 읽기에서는 항상 FILE이 좋았다. <br/>
횟수를 늘려도 결과에 의미있는 변화는 없었다.<br/>

쓰기 max 값이 5배 정도 크기 때문에 FILE을 통한 쓰기를 자주하지 않는 프로젝트에서는 stream을 통한 쓰기가 더 빠를 수도 있겠다.<br/>
하지만 기본적인 속도가 느려서 실제로 더 빠를지는 검증이 필요하다.<br/>
