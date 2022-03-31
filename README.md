## Sogang University - CSE3013 (컴퓨터공학설계및실험I)
--------

### Openframe Work Project - Crazy Arcade 구현

#### 실행 전 
  
txt 파일과 mp3 파일 3개를 /bin/data 에 넣어준다.

--------

#### 실행 후

1. 'l' or 'L' 키 입력 후 map.txt 파일을 더블 클릭한다.

2. 'd' or 'D' 키 (누를때마다 1, 2가 반복) 
    1. 화면상에 Player, Enemy가 띄워짐    
    2. Player, Enemy의 위치 재세팅 

3. 'q' or 'Q' 키 => Memory Free 및 게임 종료
 
-------- 
 
#### < Player 동작에 관련된 키 소개 >

> Up Key(↑) => 윗쪽으로 이동		
> 
> Down Key(↓) => 아랫쪽으로 이동
> 
> Left Key(←) => 왼쪽으로 이동		
> 
> Right Key(→) => 오른쪽으로 이동
> 
> Space Key => 물풍선을 놓음	
> 
> Shift Key => 물풍선을 터뜨림

--------

#### < Game 에 대한 간단한 소개 >

> Player's life = 5, Enemy's life = 3
>
> Player가 Enemy 4마리를 모두 처치할 경우 게임 승리, 그 외엔 게임 패배
> 
> 물풍선을 여러개 놓을 수 있다. (Score 에 의존하므로 블록들을 제거하여 실행)
> 
> 또한 물풍선을 밀 수 있다.
