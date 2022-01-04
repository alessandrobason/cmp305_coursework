#pragma once

/*
!   -> set width to W
F   -> move fwd by 1 and draw branch, if leaves is
       non-zero distribute according to leaf_r_rng 
       and leaf_d_ang
A|%	-> close end of branch, i.e. taper to 0 radius
+   -> turn left by a
-	-> turn right by a
&	-> pitch down by a
^	-> pitch up by a
/	-> roll right by a
\	-> roll left by a
L	-> create leaf according to d_ang and r_ang
[	-> start branch
]	-> end branch
$	-> reset to vertical
*/

class Tree {
public:

private:
};