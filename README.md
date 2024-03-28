# sdpa

This is an implementation of the simple While Language (WL) along with some data-flow analyses from the course "Static and Dynamic Program Analysis" in the winter term 2023/24 at TU Darmstadt.

## While Language (WL)
WL is a simple imperative language that supports computation on integer values, where data is stored in variables, and state is changed by assignments.

###  Grammar (BNF)
Programs (𝑆𝑡𝑚𝑡)\
𝑆 ::= [𝑥 := 𝑎]^𝜋\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| [𝑠𝑘𝑖𝑝]^𝜋\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 𝑖𝑓 [𝑏]^𝜋 𝑡ℎ𝑒𝑛 𝑆 𝑒𝑙𝑠𝑒 𝑆 𝑓𝑖\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 𝑤ℎ𝑖𝑙𝑒 [𝑏]^𝜋 𝑑𝑜 𝑆 𝑜𝑑\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 𝑆; 𝑆

Arithmetic expressions (𝐴𝐸𝑥𝑝)\
𝑎 ∶: = 𝑥\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 𝑛\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| (𝑎 𝑜𝑝𝑎 𝑎)

Boolean expressions (𝐵𝐸𝑥𝑝)\
𝑏 ∶: = 𝑡𝑟𝑢𝑒\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| 𝑓𝑎𝑙𝑠𝑒\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| (𝑎 𝑜𝑝𝑟 𝑎)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| (𝑛𝑜𝑡 𝑏)\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;| (𝑏 𝑜𝑝𝑏 𝑏)

#### Notes
Some implementations or design decisions might not be optimal and not that efficient or elegant, and could very likely be improved.\
For example, since no pattern matching exists, most of the utility functions that make a structural case distinction over Stmt are very cluttered, verbose, and not that readable but that seems to be the only way to do this. (?)\
Also, using smart pointers in the AST does not help making it less verbose.\
Most of the functions are implemented according to and in the mathematical style of the lecture, therefore, some operations might be redundant or and could be done more efficiently. However, the idea was just to implement it as in the lecture.


##### TODO
- AE-analysis, RD-analysis, VB-analysis
- maybe try out monotone frameworks (formal frameworks for defining data-flow analyses)
<!---
- MonotoneFramework (look at SDPA module 4 slide 45)
-->
