- rvalue при передаче по значению9
![[Pasted image 20230211212348.png]]

- lvalue, rvalue
![[Pasted image 20230211215304.png]]
![[Pasted image 20230211215340.png]]
http://en.cppreference.com/w/cpp/language/value_category
![[Pasted image 20230211215539.png]]
![[Pasted image 20230211215833.png]]


# cringe
![[Pasted image 20230211220200.png]]

## 4 случая
1. T& x = lvalue
2. const T& x = rvalue(в VS можно и без const)
3. T&& x = lvalue (нельзя)
4. T&& x = rvalue
![[Pasted image 20230211220620.png]]

## Конструкторы копирования и пермещения
![[Pasted image 20230211220757.png]]

- move принимает тип T&& , т.е. работает как с lvalue, так и с rvalue.
![[Pasted image 20230211230211.png]]

![[Pasted image 20230211230325.png]]
