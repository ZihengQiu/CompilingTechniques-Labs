#ifndef __PATTERNNUMBER_H
#define __PATTERNNUMBER_H

//operator

#define Plus 300
#define Minus Plus+1
#define Multiply Plus+2
#define Divide Plus+3
#define Modulo Plus+4
#define Less Plus+5
#define LessEq Plus+6
#define Great Plus+7
#define GreatEq Plus+8
#define Eq Plus+9
#define NotEq Plus+10
#define BitAnd Plus+11
#define BitOr Plus+12
#define LogicAnd Plus+13
#define LogicOr Plus+14
#define Assign Plus+15


//delimiter

#define LeftParenthese 314 
#define RightParenthese LeftParenthese+1
#define LeftBracket LeftParenthese+2
#define RightBracket LeftParenthese+3
#define LeftBrace LeftParenthese+4
#define RightBrace LeftParenthese+5
#define Comma LeftParenthese+6
#define Semicolon LeftParenthese+7

//keyword

#define Break 322
#define Main Break+1
#define Continue Break+2
#define Void Break+4
#define Int Break+5
#define Float Break+6
#define If Break+7
#define Else Break+8
#define Then Break+9
#define Switch Break+10
#define Case Break+11
#define Default Break+12
#define For Break+13
#define Do Break+14
#define While Break+15
#define Return Break+16
#define Auto Break+17
#define Extern Break+18
#define Register Break+19
#define Static Break+20
#define Const Break+21
#define Sizeof Break+22
#define Typedef Break+23
#define Volatile Break+24

//Constant

#define NumInt 347
#define NumFloat 348

//ID

#define Id 349

#endif
