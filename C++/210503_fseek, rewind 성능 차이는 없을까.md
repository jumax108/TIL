# fseek, rewind 성능 차이는 없을까
fseek(fp, 0, SEEK_CUR) 과 rewind(fp) 는 성능적인 차이가 존재할까

## 예제 코드 작성
단순히 무수한 fseek, rewind로 비교하도록 해보자
```C++
	for (int cycleCnt = 0; cycleCnt < cycle; cycleCnt++) {

		start = clock();

		for (int loopCnt = 0; loopCnt < loop; loopCnt++) {
			fseek(file, 0, SEEK_END);
			fseek(file, 0, SEEK_CUR);
		}

		end = clock();
		printf("%d\n", end - start);
		sum += end - start;
	}
```

```C++
	for (int cycleCnt = 0; cycleCnt < cycle; cycleCnt++) {
		start = clock();

		for (int loopCnt = 0; loopCnt < loop; loopCnt++) {
			fseek(file, 0, SEEK_END);
			rewind(file);
		}

		end = clock();
		printf("%d\n", end - start);
		sum += end - start;
	}
```

## 결과
rewind가 fseek보다 빠른 경향이 있다.<br/>
큰 차이는 없으나 수백만번 실행했을 때, 평균적으로 rewind가 더 빠르다.<br/>
하지만 개별적으로 봤을 때는 fseek가 더 빠른 경우도 있었다.<br/>
이 정도의 성능 차이는 무엇을 사용해도 무방해보인다.<br/>
