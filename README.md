# sdpa

This is an implementation of the the simple While Language (WL) along with some data-flow analyses from the course "Static and Dynamic Program Analysis" in the winter term 2023/24 at TU Darmstadt.

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

#### Todo
- shared pointer overhead? are there alternatives?

<!---
## TODO
- add a base class MonotoneFramework (formal frameworks for defining data-flow analyses) that has the abstract member variables and functions, then define the DFAs as subclasses and implement the abstract operations as concrete implementations. (look at SDPA module 4 slide 45)
- add constraints for each DFA, i.e. LV analysis needs isolated exits etc. Implement that if the input does not fullfull requirement, that an empty skip statement is added.
- maybe implement multi-threaded version of analyses, which was the primary reason to use tuple representation!
-->
