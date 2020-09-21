# Yalta - Yet Another Lua TAgs
[![Build status](https://ci.appveyor.com/api/projects/status/2fpf7q0uf8643gvo?svg=true)](https://ci.appveyor.com/project/EugeneManushkin/yalta)

Yalta is a [ctags](https://en.wikipedia.org/wiki/Ctags)-like program that produces tags for source code files written in [Lua](https://www.lua.org/). This is an experimental program made as a PoC for pool request to [Universal Ctags](https://ctags.io/).

# Integrate with CtagsSourceNavigator
This utility may be integrated with [CtagsSourceNavigator](https://github.com/EugeneManushkin/CtagsSourceNavigator) plugin.
Download latest stable version from [releases](https://github.com/EugeneManushkin/Yalta/releases) page and unpack it. Open [Far Manager](https://www.farmanager.com/)
and then open plugin configuration page (**F11->Ctags Source Navigator->C Plugin configuration**). Uncheck "Use built in ctags utility" and put
full path to ctags_wrapper.bat script located in unpacked_release_folder\yalta\ctags_wrapper.bat.

# Install
### Windows
Download latest stable version from [releases](https://github.com/EugeneManushkin/Yalta/releases) page.
## Linux
Build it from source code. See Build section.

# Usage
## Windows
```batch
:: Index single file:
yalta.bat path/to/file.lua > tags
:: Index directory recursive:
yalta.bat path/to/directory > tags
```
## Linux
TODO: describe

# Build
Required CMake and compiler with C++11 support.
```bash
git clone https://github.com/EugeneManushkin/Yalta.git yalta
mkdir -p yalta/build && cd yalta/build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

# What is captured?
Unlike the Universal Ctags Yalta recognizes language constructions separated with spaces, line breaks and comments. 

## Functions (kind 'f')
Yalta recognizes dot and colon separated function scope and properly handles assignment of a first function in local variable list.
Yalta handles function assignment to a global variable list in same way as Universal Ctags which is wrong. In real code assignment
anonymous function to a global variable list is rare thow.
|                                 Language construction |                     Yalta |    Universal Ctags |
|:------------------------------------------------------|:--------------------------|:-------------------|
|```function aaa.bbb.ccc.foo(args)                   ```|foo with class:aaa.bbb.ccc |**aaa.bbb.ccc.name**|
|```function aaa.bbb.ccc:foo(args)                   ```|foo with class:aaa.bbb.ccc |**aaa.bbb.ccc:name**|
|```local function foo(args)                         ```|foo                        |foo                 |
|```local foo = function(args)                       ```|foo                        |foo                 |
|```local foo, aaa, bbb = function(args) ... end, ...```|foo                        |**bbb**             |
|```foo = function(args) ... end, ...                ```|foo                        |foo                 |
|```aaa, bbb, foo = function(args) ... end, ...      ```|**foo**                    |**foo**             |
|```{10, 20, foo = function(args) ... end, 30}       ```|foo                        |foo                 |

## Locals (kind 'l')
Yalta parses local variable lists. Variables of 'for' statement and function arguments are also parsed as local variables.
|                                 Language construction |                     Yalta |    Universal Ctags |
|:------------------------------------------------------|:--------------------------|:------------------:|
|```local aaa, bbb, ccc                              ```|aaa, bbb, ccc              |**not implemented** |
|```for key,value in pairs(t) do                     ```|key, value                 |**not implemented** |
|```for index=1,10 do                                ```|index                      |**not implemented** |
|```function foo(arg1, arg2, arg3)                   ```|arg1, arg2, arg3           |**not implemented** |
|```call(function(arg1, arg2, arg3))                 ```|arg1, arg2, arg3           |**not implemented** |

## Table keys (kind 'k')
Assignment to a sequence separated with dots is treated as a creation of a table key.
|                                 Language construction |                     Yalta |    Universal Ctags |
|:------------------------------------------------------|:--------------------------|:------------------:|
|```aaa, bbb, some.scope.key="value", ccc ...        ```|key with class:some.scope  |**not implemented** |

## Assignments (kind 'a')
Yalta generates tags for any assignment to a name that is not recognized as another known kind. Recognizing creation of global variables
requires full-fledged language parsing. Capturing any assignment is much easyer and cover almost all cases of creation table keys or globals.
|                                 Language construction |                     Yalta |    Universal Ctags |
|:------------------------------------------------------|:--------------------------|:------------------:|
|```aaa, global_variable="value", bbb                ```|global_variable            |**not implemented** |
|```local variable; variable = "value"               ```|variable                   |**not implemented** |
|```local t = {10, 20, key="value", 40}              ```|key                        |**not implemented** |
|```Call{10, 20, key="value", 40}                    ```|key                        |**not implemented** |


