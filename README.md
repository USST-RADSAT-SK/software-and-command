# software-and-command
Code and Documentation for USST’s RADSAT-SK in their first Canadian CubeSat Project

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
