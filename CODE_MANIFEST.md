# NAMING CONVENTION:
thou shalt use pascalcase for **CLASS** and other type names
```cpp
class MokraWoda {}; // <= CORRECT
```
instead of
```cpp
class mokraWoda {}; // <= INCORRECT
class mokra_woda {}; // <= INCORRECT
```

**FUNCTIONS** use pascalcase, lest ye be deemed my own hounds victuals
```cpp
int OsuszWode() {} // <= CORRECT
```
and not 
```cpp
int osuszWode() {} // <= INCORRECT
int osusz_wode() {} // <= INCORRECT
```
if a sailor doth not use camelcase for **VARIABLES** AND **ARGUMENTS** he should be whipped with a cat o nine tails
```cpp
int Nawodnij(int argumentValue, char argument) {} // <= CORRECT
```
and 
```cpp
int Nawodnij(int argument_value, char Arguments) {} // <= INCORRECT
int Nawodnij(int ArgumentValue, char Arguments) {} // <= INCORRECT
```

each and every **CONSTANT** shall be named with caterwauling snake case 
```cpp
constexpr KALUZA = 1; // <= CORRECT  
#define KALUZA 1 // <= CORRECT
constexpr Kaluza = 1; // <= INCORRECT
constexpr kaluza = 1; // <= INCORRECT
```
