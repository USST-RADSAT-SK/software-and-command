# software-and-command
Code and Documentation for USST’s RADSAT-SK in their first Canadian CubeSat Project

## Table of Contents
1. [Setting Up Your Repo](#Setting-Up_Your-Repo)
2. [Coding Standard](#Coding-Standard)
    1. [Indentation](#Indentation)
    2. [Variable Naming](#Variable-Naming)
    3. [Whitespace](#Whitespace)
    4. [Braces](#Braces)
    5. [Parentheses in Expressions](#Parentheses-in-Expressions)
    6. [Switch Statements](#Switch-Statements)
    7. [Line Lengths](#Line-Lengths)
3. [Code Documentation](#Code-Documentation)
    1. [Functions](#Functions)
    2. [Global Variables](#Global-Variables)
    3. [Typedefs](#Typedefs)
    4. [Structs](#Structs)
    5. [Enums](#Enums)
    6. [Macros](#Macros)
4. [Branching](#Branching)
    1. [Procedure](#Procedure)
    2. [Naming](#Naming)

## Setting Up Your Repo
1. Run ```git config core.hooksPath .githooks``` (sets where git looks for the githooks)
2. Run ```chmod +x .githooks/pre-commit``` (makes the githook script executable)

## Coding Standard
Our coding standard is very loosely based on the Qt coding style found [here](https://wiki.qt.io/Qt_Coding_Style).

We have a coding standard so that everyone's code looks the same and is easily readable. Commits made to the project not adhering to these standards may not be allowed to be pushed. Source code from a third party will not be expected to follow these standards.

### Indentation
Tabs or 4 spaces are allowed.

### Variable Naming
Variable names should be descriptive and abbreviations should be avoided. This doesn't apply to loop variables:
``` c
int sum = 0;
for ( int i = 0; i < maxCount; ++i ) {
	sum += i;
}
```
All variable and function names are in camel case (first word lowercase, follwing words capitalized):
``` c
int myNewVariable
```
This is true for most variables and constants. However, for macros and enumerable types, the name is in all caps with underscores in between words:
``` c
#define ARRAY_SIZE 8
enum booleanValues {
	FALSE = 0,
	TRUE = 1
};
```
In functions, all variables that will be used throughout the function should be declared at the top of the function.

### Whitespace
#### Around Brackets
In function definitions and function calls, arguments and parameters are wrapped in spaces, which looks like this:
``` c
int myFunction( int arg1, int arg2 ) {
	myFunction( arg1, arg2 );
}
```
If, switch, for, and while statements have a similar style:
``` c
for ( int i = 0; i < maxCount; ++i ) {
	if ( i == 0 ) {
		i = maxCount;
	}
}
```
#### Asterisks
When declaring a pointer variable, the asterisk goes right after the variable type, then a space is left between the asterisk and the variable name:
``` c
int main( int argc, char* argv[]) {
	int* myArray = (int*)malloc( ARRAY_SIZE * sizeof( int ) );
}
```
Note that the cast to int* does not have spaces wrapping the type.
#### Operators
When using operators with a single operand (like address-of), there is no space between the variable and the operator. When using operators with 2 (or 3) operands, the operator is wrapped with spaces:
``` c
int myInt = 0;
int* myIntPtr = &myInt;
myIntPtr = myIntPtr + 4;
*myIntPtr = *myIntPtr * 2;
```

### Braces
For all conditional statements and function definition, the opening curly brace goes on the same line as the conditional logic/function header:
``` c
int myFunction() {
	int i = 0;
	while ( i++ < maxCount ) {
		printf( "%d", i );
	}
}
```
Note that if a conditional statement has only a single line of code as a body, curly braces are still required and the line of code still goes on the next line for maintainability and readability.
If statements with else-if and else blocks have a closing curly brace, then the else keyword goes on the next line:
``` c
if ( counter == 0 ) {
	return 1;
}
else if ( counter == 1 ) {
	return 0;
}
else {
	return counter;
}
```

### Parentheses in expressions
If there is any ambiguity to the order of operations in your expression, use parentheses to make the order of operations explicit:
``` c
int errorResult = ( ( i & 1 ) * 4 ) + ( i & 3 );
```

### Switch Statements
How switch statements work if you're unfamiliar: if you pass in a variable to a switch statement, a 'case' of that switch statement is that that variable is equal to some constant. If the exact value of the variable has a case associated with it, code execution immediately jumps to that case (if it is not found, it goes to the 'default' case). Once the end of the case is reached, the code does not stop, so a break statement must be used to forcibly stop the execution of the switch code.

On this project, if you have two cases that do the exact same thing, you should simply group the cases together as shown in the example below. If you are writing code where you do not want to exit the switch statement after the correct case has finished executing, you should leave a comment stating that this was an intentional choice, since people unfamiliar with case statements will often forget to use break to exit.

Here is an example where a switch statement is used to implement a recursive Fibonacci function, where the series if assumed to start from 0 and the nth Fibonacci number is to be calculated, with some liberties taken to make a better example (recall that the 0th Fibonacci number is 0 and the 1st Fibonacci is 1):
``` c
int fibonacci( int n ) {
	int returnValue;
	switch ( n ) {
	case 0:
	case 1:
		returnValue = n;
		break;
	default:
		return fibonacci( n - 1 ) + fibonacci( n - 2 );
		break;
	}
	return returnValue;
}
```
Here is an example where fallthrough is used (and break statements are not). This is a function that converts an int (from 0 to 9) to the corresponding ASCII character. Notice the use of comments to indicate that fallthrough is intentional:
``` c
char toChar( int digit ) {
	char returnVal = '0';
	switch ( digit ) {
	case 9:
		++returnVal;
		// fallthrough
	case 8:
		++returnVal;
		// fallthrough
	case 7:
		++returnVal;
		// fallthrough
	case 6:
		++returnVal;
		// fallthrough
	case 5:
		++returnVal;
		// fallthrough
	case 4:
		++returnVal;
		// fallthrough
	case 3:
		++returnVal;
		// fallthrough
	case 2:
		++returnVal;
		// fallthrough
	case 1:
		++returnVal;
		// fallthrough
	case 0:
		break;
	default:
		returnVal = '\0';
		break;
	}
	return returnVal;
}
```

### Line Lengths
Lines should be 80 characters or less long since that is the default size of a terminal window. The maximum accepted line length will be 100 since no one really uses terminals anyways. Your IDE should tell you at the bottom of your screen what character/column you are on, so you shouldn't have to count this yourself.

## Code Documentation
Our code is documented using [doxygen](http://www.doxygen.nl/). All comments
used for documentation need to be in comment blocks starting with /** and
ending with */ otherwise Doxygen will not recognize them.

### Functions
The documentation for functions should be put in the .c file that
the function is defined in.

``` c
/**
 * @brief a short one line description
 *
 * the detailed description can be
 * multiple lines
 * @header "the/include/path.h"
 * @param c short description of the parameter
 * @pre describe the pre condition
 * @post describe the post condition
 * @return describe the return value
 */
int function ( char c ){
    // code
}
```

### Global Variables
Documentation for global variables should go inside the .c file that they
are defined in.

``` c
/**
 * short description of the variable
 */
int variable;
```

### Typedefs
Documentation for typedefs should go inside the .h file that they are
defined in.

``` c
/**
 * short description of the typedef
 */
typedef char CHARACTER;
```

### Structs
Documentation for structs should go inside the .h file that they are
defined in.

``` c
/**
 * @brief short description of the struct
 */
typedef struct aStruct{
    int a; /**< short description of the member */
    char b; /**< short description of the member*/
} MY_STRUCT;
```

### Enums
Documentation for enums should go inside the .h file that they are
defined in.
``` c
/**
 * short description of the enum
 */
typedef enum {
    TRUE, /**< short description of the member */
    FALSE /**< short description of the member */
} BOOL;
```

### Macros
Documentation for macros should go inside the .h file that they are
defined in.

``` c
/**
 * short description of the macro
 */
#define myMacro 1

/**
 * short description of the macro
 * @param x short description of x
 * @param y short description of y
 */
#define functionMacro( x, y ) ( x + y )
```
## Branching

### Procedure
1. In your local repo run ```git checkout alpha```
2. run ```git checkout -b "<your branch name>"```
3. The first time you push the branch there will an error. Just follow the instructions to set the upstream branch.

### Naming
All branches **MUST** follow the branch naming scheme. Any branch that does not follow the structure will not be allowed to be pushed.
The structure is as follows:
- ft-YYYY-branch_name

Where YYYY is one of the 4 letter module codes:

- MNGT = Management. This is just a general tag for any team management task like restructuring the code base or formatting doxygen code docs
- BOOT = Bootloader module
- MAIN = Main Application module
- COMM = Comms processing module
- IMAG = Image processing module
- PAYL = Payload processing module
- HOUS = Housekeeping module

The branch_name after the 4 letter code is **all** lowercase with words separated by underscores.

Ex:\
Correct: ft-MAIN-aic_function_stubs\
Incorrect: ft-main-AIC-function-stubs
