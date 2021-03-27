# software-and-command
Code and Documentation for USST’s first Canadian CubeSat Project: The RADSAT-SK Satellite 

## Table of Contents
1. [Setting Up Your Repo](#Setting-Up-Your-Repo)
2. [Coding Standard](#Coding-Standard)
    1. [Indentation](#Indentation)
    2. [Variable Naming](#Variable-Naming)
    3. [Files](#Files)
    4. [Whitespace](#Whitespace)
    5. [Braces](#Braces)
    6. [Parentheses in Expressions](#Parentheses-in-Expressions)
    7. [Switch Statements](#Switch-Statements)
    8. [Line Lengths](#Line-Lengths)
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

## Setting Up Your Repository
1. Get WSL (Windows Subsystem for Linux) or Git Bash for your computer
2. Using one of the aforementioned programs, navigate to where you'd like the repository to exist
3. Run ```git clone https://github.com/USST-RADSAT-SK/software-and-command.git``` (downloads the repository to your computer)
4. Navigate to the repo: ```cd software-and-command```

## Coding Standard
Our coding standard is loosely based on the Qt coding style found [here](https://wiki.qt.io/Qt_Coding_Style).

We have a coding standard so that everyone's code looks the same and is easily readable. Commits made to the project not adhering to these standards may not be allowed to be pushed. Source code from a third party will not be expected to follow these standards.

Like all rules, some exceptions can be allowed. The most important takeaway is that your code should be consistent and easy to read.

### Indentation
Tabs or 4 spaces are allowed.

### Variable Naming
Variable names should be descriptive and abbreviations should be avoided. Exemptions may apply to loop variables:
``` c
int sum = 0;
for (int i = 0; i < maxCount; ++i) {
	sum += i;
}
```
All variable and function names are in camel case (first word lowercase, follwing words capitalized):
``` c
int myNewVariable
```
This is true for most variables and constants. However, for macros (and some enumerable types), the name is in all caps with underscores in between words:
``` c
#define ARRAY_SIZE 8
enum booleanValues {
	FALSE = 0,
	TRUE = 1
};
```
In functions, most variables that will be used throughout the function should be declared at the top of the function. Exceptions may include variable declarations within the scope of an if or for loop.

### Files
#### File Naming
To prevent namespace collisions and to make it extra obvious what code is "local" (rather than imported), all files **MUST** be prepended with the R character.
E.g., RProtobuf.h; RPayloadCollectionTask.c, etc.

#### Doxygen File Header
Every single source and header file written for the RADSAT-SK cubesat needs a Doxygen file header of the following style:
``` c
/**
 * @file RProtobuf.c
 * @date March 20 2021
 * @author Jim Lahey
 */  
```

#### File Section Separators
To increase readability (especially in larger files), multi-line function separators should be used. Ideally, these are used in all files. Do not use the separators to define a section if the section is empty, however. See the main examples of sections that are used:
``` c
/***************************************************************************************************
                                             PUBLIC API                                            
***************************************************************************************************/

/***************************************************************************************************
                                          PRIVATE FUNCTIONS                                          
***************************************************************************************************/

/***************************************************************************************************
                                            DEFINITIONS                                             
***************************************************************************************************/
```

These are not strictly enforced, but are recommended. Consistency is the most important thing.


### Whitespace
#### Around Brackets
In function definitions and function calls, no additional whitespace is needed.
``` c
int myFunction(int arg1, int arg2)
{
	myOtherFunction(arg1, arg2);
}
```
If, switch, for, and while statements have a similar style. But make sure to add space between the brackets and the "for" keyword, and the curly brace.
``` c
for (int i = 0; i < maxCount; ++i) {
	if (i == 0) {
		i = maxCount;
	}
}
```

#### Newline Whitespace
In between function definitions, exactly two lines of whitespace should be used. 
``` c
int myFunction(int arg1, int arg2)
{
	return myOtherFunction(arg1, arg2);
}


int myOtherFunction(int arg1, int arg2)
{
	return (arg1 + arg2);
}
```
Within a function, one line of whitespace should separate functional "chunks" of code. Two lines *may* be used when things get crowded, however if you feel the need to partition your function like this, it may be time to split it into multiple functions, or use inline comment blocks to separate them.
``` c
int myFunction(int arg1, int arg2)
{
	// init
	int newVariable = 0;
	int otherVariable = 0;

	// do thing
	newVariable = arg1 + 1;

	/* Now do the other thing */

	// bar bar
	newVariable += 1;
	otherVariable = myOtherFunction(newVariable);

	return otherVariable;
}
```

Whitespace can sometimes be helpful or even necessary to increase legibility. Feel free to use additional whitespace (within reason) where you see fit.

#### Asterisks
When declaring a pointer variable, the asterisk goes right after the variable type, then a space is left between the asterisk and the variable name:
``` c
int main(int argc, char* argv[])
{
	int* myArray = (int*)pvPortMalloc(ARRAY_SIZE * sizeof(int));
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
For all conditional statements, the opening curly brace goes on the *same* line as the conditional logic/function header. For all function definitions, the curly brace goes on the *next* line:
``` c
int myFunction(void)
{
	int i = 0;
	while (i++ < maxCount) {
		printf("%d", i);
	}
}
```
Note that if a conditional statement has only a single line of code as a body, curly braces are still required and the line of code still goes on the next line for maintainability and readability.
Else-if and else blocks have a closing curly brace, then the else keyword goes on the next line:
``` c
if (counter == 0) {
	return 1;
}
else if (counter == 1) {
	return 0;
}
else {
	return counter;
}
```

### Parentheses in expressions
If there is any ambiguity to the order of operations in your expression, use parentheses (and additional whitespace) to make the order of operations explicit:
``` c
int errorResult = ( ( i & 1 ) * 4 ) + ( i & 3 );
```

### Switch Statements
Cases that simply fall into the following case should be grouped together. Cases that do something and intentionally fall into the next case should explicitly say so with a comment. All case statements must end in either a break *or* return. Whitespace newline is left between each distinct set of cases. See example below.
``` c
int function(int n)
{
	int returnValue = 0;

	switch (n) {
	case (0):
	case (1):
	case (2):
		returnValue += doExtraThing(n);
		// FALLTHROUGH

	case (3):
		returnValue += n;
		break;
	
	default:
		return 0;
	}

	return returnValue;
}
```


### Line Lengths
Lines should aim to be 80 characters or less long, but the maximum accepted line length will be roughly 100 since no one really uses terminals anyways. Some exceptions may be made, but anything over 100 lines is starting to push the limits of readability.

## Code Documentation
Our code is documented using [doxygen](http://www.doxygen.nl/). All comments
used for documentation need to be in comment blocks starting with /** and
ending with */ otherwise Doxygen will not recognize them.

### Functions
The documentation for functions should be put in the source (.c) file that
the function is defined in.

``` c
/**
 * A short one line description
 *
 * The detailed description  is often not necessary, and should be used sparingly.
 * It can be multiple lines long. Try not to get too technical; keep the description
 * high-level.
 *
 * @note give a notice to anyone using this function (if any; usually not)
 * @pre describe the pre condition (if any; usually not)
 * @post describe the post condition (if any; usually not)
 * @param c short description of the parameter
 * @return describe the return value
 */
int function (char c)
{
    // code
}
```

### Global Variables
Documentation for global variables should go inside the source (.c) file that they
are defined in.

``` c
/** short description of the variable */
int variable;
```

### Typedefs
Documentation for typedefs should go inside the header (.h) file that they are defined in.

``` c
/** short description of the typedef */
typedef char CHARACTER;
```

### Structs
Documentation for structs should go inside the .h file that they are defined in.

``` c
/** short description of the struct */
typedef struct _myStruct{
    int a; 		/**< short description of the member */
    char b; 	/**< short description of the member */
} myStruct;
```

### Enums
Documentation for enums should go inside the .h file that they are
defined in.
``` c
/** short description of the enum */
typedef enum {
    TRUE, /**< short description of the member */
    FALSE /**< short description of the member */
} BOOL;
```

### Macros
Documentation for macros should go inside the .h file that they are
defined in.

``` c
/** short description of the macro */
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
1. In your local repo run ```git checkout alpha``` (you may have to commit, stash, or throw away uncommitted changes on your current branch)
2. Run ```git pull``` (makes sure you have the latest code)
2. run ```git checkout -b "<your branch name>"``` (creates a new branch)
3. The first time you push the branch there will an error. Just follow the instructions to set the upstream branch.

### Branch Naming
All branches **MUST** follow the few branch naming rules. Those rules are:
- No captials
- No underscores

GitHub (and most other Git tools) allow you to use branch folders, simply by including forward slashes in the names of branches. Branch folders are recommended when useful, but not always necessary. Some examples of *good* branch names:
- admin/restructure-directories
- test/write-unit-test-framework
- app/fix-payload-collection-task
- framework/implement-uart-wrapper
- operation/import-nanopb

Notice that all of the directories used are based off of the Project names for the RADSAT-SK GitHub repo. It would be wise to use that approach.