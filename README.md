# software-and-command
Code and Documentation for USST’s first Canadian CubeSat Project: The RADSAT-SK Satellite 

## Table of Contents
1. [Setting Up Your Repo](#Setting-Up-Your-Repo)
2. [Coding Standard](#Coding-Standard)
    1. [Indentation](#Indentation)
    2. [Variable Naming](#Variable-Naming)
    3. [Files](#Files)
    4. [Whitespace](#Whitespace)
    5. [Curly Braces](#Curly-Braces)
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
5. [Directory Structure](#Directory-Structure)

## Setting Up Your Repository
1. Get WSL (Windows Subsystem for Linux) or Git Bash for your computer
2. Using one of the aforementioned programs, navigate to where you'd like the repository to exist
3. Run ```git clone https://github.com/USST-RADSAT-SK/software-and-command.git``` (downloads the repository to your computer)
4. Navigate to the repo: ```cd software-and-command```
5. Run ```git config core.hooksPath .githooks``` (sets where git looks for the githooks)
6. Run ```chmod +x .githooks/pre-commit``` (makes the githook script executable)

Now your repo should be all set up! Check out our branching strategy below and coordinate with the Software and Command Team Lead(s) for further guidance.

## Coding Standard
Our coding standard is loosely based on the Qt coding style found [here](https://wiki.qt.io/Qt_Coding_Style).

We have a coding standard so that everyone's code looks the same and is easily readable. Commits made to the project not adhering to these standards may not be allowed to be pushed. Source code from a third party will not be expected to follow these standards.

Like all rules, some exceptions can be allowed. The most important takeaway is that your code should be consistent and easy to read.

### Indentation
Tabs or 4 spaces are allowed.

### Variable Naming
Variable names should be descriptive and abbreviations should almost always be avoided. Exemptions may apply to loop variables:
``` c
int sum = 0;
for (int i = 0; i < maxCount; ++i) {
	sum += i;
}
```
All variable and function names are in camel case (first word lowercase, follwing words capitalized):
``` c
int myNewVariable;
```
This is true for most variables and constants. However, for macros (and some enumerable types), the name is in all caps with underscores in between words:
``` c
#define ARRAY_SIZE 8
enum booleanValues {
	FALSE	= 0,
	TRUE	= 1,
};
```
Most enums will have "global" scope, so you'll usually want to prepend their enumeration names with the name of the enum itself: 
``` c
enum colours {
	colourRed 	= 0,
	colourGreen = 1,
	colourBlue 	= 2,
};
```
ALso note that the enumeration values are all explictly defined; this is highly recommend for readability and to prevent mistakes.

In functions, most variables that will be used throughout the function should be declared at the top of the function. Exceptions may include variable declarations within the scope of an if or for loop.

### Files
#### File Naming
To prevent namespace collisions and to make it extra obvious what code is "local" (rather than imported), all locally created files **MUST** be prepended with the R character. After that, they follow the CapitalCase convention (each word starts with a capital, everything else is lowercase). Absolutely no underscores or hyphens in file names. 

Names should also be short and sweet. Acronyms are fine, but are still subject to CapitalCase conventions. 

Some good examples:
- RProtobuf.h
- RPayloadCollectionTask.c
- RUart.h
- RDosimeter.h

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
                                            DEFINITIONS                                             
***************************************************************************************************/

/***************************************************************************************************
                                       PRIVATE FUNCTION STUBS                                       
***************************************************************************************************/

/***************************************************************************************************
                                             PUBLIC API                                             
***************************************************************************************************/

/***************************************************************************************************
                                          PRIVATE FUNCTIONS                                          
***************************************************************************************************/
```
Each line ends after exactly 100 characters, and the words are centered.
These are not strictly enforced, but are recommended. Consistency is the most important thing.

### Whitespace
#### Around Brackets
In function definitions and function calls, no additional whitespace is needed.
``` c
int myFunction(int arg1, int arg2) {
	myOtherFunction(arg1, arg2);
}
```
If, switch, for, and while statements have a similar style. But make sure to leave a space around the "for" keyword and the curly brace. Additional whitespace can be used when necessary, but it usually isn't.
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
int myFunction(int arg1, int arg2) {
	return myOtherFunction(arg1, arg2);
}


int myOtherFunction(int arg1, int arg2) {
	return (arg1 + arg2);
}
```
Within a function, one line of whitespace should separate functional "chunks" of code. Two lines *may* be used when things get crowded, however if you feel the need to partition your function like this, it may be time to split it into multiple functions, or use inline comment blocks to separate them:
``` c
int myFunction(int arg1, int arg2) {
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
int main(int argc, char* argv[]) {
	int* myArray = (int*)pvPortMalloc(ARRAY_SIZE * sizeof(int));
}
```

#### Operators
When using operators with a single operand (like address-of), there is no space between the variable and the operator. When using operators with 2 (or 3) operands, the operator is wrapped with spaces:
``` c
int myInt = 0;
int* myIntPtr = &myInt;
myIntPtr = myIntPtr + 4;
*myIntPtr = *myIntPtr * 2;
```

### Curly Braces
For all conditional statements, loops, and function definitions, the opening curly brace goes on the *same* line as the conditional logic/function header.
``` c
int myFunction(void) {
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
int function(int n) {
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
 * The detailed description is often not necessary, and should be used sparingly.
 * It can be multiple lines long. Try not to get too technical; keep the description
 * high-level, in case the inner-workings of the function are ever changed.
 *
 * @note give a notice to anyone using this function (if any; usually not)
 * @pre describe the pre condition (if any; usually not)
 * @post describe the post condition (if any; usually not)
 * @param c short description of the parameter
 * @return describe the return value
 */
int function (char c) {
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
typedef struct _myStruct {
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
#define functionMacro(x, y) (x + y)
```

## Directory Structure
We have chosen to follow a layered approach to code organization, partitioning our project into six *Layers*. From the top-down:
1. Application (source)	-> Performs specific functions required by the mission. Contains ```main()``` function (program entry point)
2. Operation			-> Provides generic operations that support the mission
3. Framework			-> Interfaces with the OS & HAL, to support the mission operations
4. OS					-> Wraps the Hardware to provide kernel-level support (task scheduling, semaphores, etc.)
5. HAL					-> Abstracts (i.e. simplifies) access to OBC hardware and peripherals
6. Hardware				-> The actual On-Board Computer's hardware and peripherals

Each layer provides an API to the layer directly above it, and thus each layer only interfaces with the layer directly beneath it. For example, the code within the Operation layer *only* uses the functionality provided by the Framework layer. This allows for nice encapsulation and de-coupling between the layers.

This has multiple benefits. One is that it allows for easier testing, since the top two layers should be completely hardware-independent, meaning they can (for the most part) be unit tested on a desktop PC. The decoupling also reduces complexities and debugging time, since each layer can only interface with the one directly beneath it. This can also be used to assist with deadlock prevention and other concurrency issues, since the lower layers (framework, OS) can be responsible for that.

It is important to note that the RADSAT-SK team is only developing the top three layers; the bottom three have already been provided to us.

If you're unsure of where to place some new code, talk to the current Software and Command Team Lead(s).

## Branching

### Strategy
We have 5 "levels" of branching used:
1. master	-> This is reserved for FLIGHT READY code, i.e. very well tested. Should only be merged into once or twice ever
2. beta		-> This is reserved for FLATSAT code, i.e. moderately well tested.
3. alpha	-> Working development branch, reserved for code that has been at least partially reviewed / tested.
4. other	-> These branches are the only places where development happens. Each of these is based off of alpha
5. hotfix	-> These are quick, one-off branches intended for quick fixes that are found on alpha or beta branches

Anyone can create a branch off of alpha and start developing. However, 2 people are required to review a PR (Pull Request) before the code can be accepted into the alpha branch. One of these people must be a team lead or project manager.

alpha can be merged into beta, and beta can be merged into master. These are done very seldom throughout the project as the codebase matures, and may ONLY be done by the Software and Command Team Lead(s) or CubeSat Project Managers.  

### Procedure
1. In your local repo run ```git checkout alpha``` (you may have to commit, stash, or throw away uncommitted changes on your current branch)
2. Run ```git pull``` (makes sure you have the latest code)
2. run ```git checkout -b "<your branch name>"``` (creates a new branch)
3. The first time that you try to push on the branch it will throw an error. Just follow the instructions to set the upstream branch.

### Branch Naming
All branches **MUST** follow the few branch naming rules. Those rules are:
- No captials
- No underscores
- Use hyphens instead of spaces
- Must prepend new branch into one of six directories (see below)

GitHub (and most other Git platforms) allow you to use branch folders, simply by uses forward slashes. Some examples of *good* branch names:
- admin/restructure-directories
- test/write-unit-test-framework
- application/implement-payload-collection-task
- operation/import-nanopb
- framework/implement-uart-wrapper
- hotfix/fix-i2c-bug

Notice that all six of the directories used are based off of the Project names for the RADSAT-SK GitHub repo (minus hotfix, which is for quick fixes on alpha or beta branches).
