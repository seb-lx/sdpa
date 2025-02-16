# sdpa

This is an implementation of the simple While Language (WL) along with some data-flow analyses (currently only Live-Variables Analysis) from the course "Static and Dynamic Program Analysis" in the winter term 2023/24 at TU Darmstadt.

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

where
- 𝑛 ranges over the set of numerals
- 𝑥 ranges over the set of program variables
- 𝜋 ranges over the set of program points
- 𝑜𝑝𝑎 ranges over the set of arithmetic operations
- 𝑜𝑝𝑏 ranges over the set of boolean operations
- 𝑜𝑝𝑟 ranges over the set of relational operations
