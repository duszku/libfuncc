# libfuncc

Libfuncc is a simple library that aims to port some of features typically seen
in functional programming languages to C in a reasonable way.

It is still in an early stage of development.

## Modules

- [x] Linked lists and set of functions imitating Haskell's `Data.List`
- [ ] Tuples, currying and uncurrying, zipping of lists
- [ ] Monads as wrappers around types with a common interface
- [ ] Functions, set of macros/subroutines that allow for things such as
function composition or partial application

## Getting started

In order to use _libfuncc_ you first need to have a C compiler installed.
Makefile uses `gcc` for sanitizers in the DEBUG target, however with minor
changes you may freely use any other compiler such as `tcc` as the code itself
does not depend on any compiler extensions and sticks to C90.

First, clone the repository:

```shell
git clone https://github.com/duszku/libfunc.git
```

Next build the library:

```shell
make
```

There also exists an `install` target which puts all the necessary files into
appropriate subdirectories of `/usr/local` as well as a `pdf` target which
requires doxygen and pdflatex and creates _(somewhat chaotic and still under
development)_ documentation.

As of now default target is set to DEBUG, as project is still far from finished.
That said for everyday usage it might be wise to switch it to RELEASE. This can
be done by editing `TARGET` variable in the makefile.

## What comes next?

The idea of normal and _"inplace"_ functions was somewhat missed and i think ill
remove it and just add a separate function for copying. I also plan to add a
separate field in the list structure for data destructor, as now it may only
store simple types, releasable by `free()`.

Once I do that, this and some of possible bug fixes and optimizations will
become the libfuncc release 0.2 and ill switch to tuples module as i need it for
list zips.

## Lain for no reason

<pre>
                             ;*+@#####@@######@@@@@@@$*^  ..,     .             
                        ;*$@#####^          ;+@&&&@&@@@#@$*        ,..          
                     ,+##@@@@@&#&            *@&&&&&&&&&&&@#=,  .......,.       
           ...,.   *##@@@@&&&@&@#         ^&@@&&&&&@&&&&&&@@@@@, ......,.,.     
         .  ^    +##@&&&@@&&&&@&##;  ,**&@@@@&&&&&&&&&&&&&@&&@@#*  ........,    
        , ^$   *##@@@&@@&&&@@&@&@@@@@##@@@@&&&&&&&&&&&&&&@&&&&&@#@  .........,  
       ,, .   @#@@@@@&&&@&@@@@@@######@@@@@&&&&&&&@&&&&&&@&&@&&&@#&  ........,,.
       . .. ;###@@@@@@###@++*^^^;;;;^^^**++@###@&&&&&&&&&&&&&&@&@@#, ...........
        ,. *###@@##&++^                        **+@#@@@&&&&&&&&&&@## ...........
       ,,  ####@+;                                  ;+@##@@&&&&&&@@#  ..........
      ,.. &@+,                                          ;+@#@&&&&&@#  ..........
     ,,,.                                                   *@#@@@@#  ..........
    ,. .                                                      .+###@ .........,.
   . ^+                    ,  &@   #@                            +#. ...........
   ,$*                    &  ###  &##;   =       .                  ............
  *&*                    #^ @###  ####   #       ,#                  .,.........
 ;&=                    ## ,###& &####   #@       ##^                  .........
 =&               =    ##= ####  #####;  ##       ###;  *               ........
 ++              #+  +^**  +=## @#####+ @##=##@+ *####  @&               .......
 ,.             *& *#####$+$*=#=###### #######$+*,.,^** *#                .,....
              & ##+#&++++#@*;#################+&######@++^ *+              ,....
 .            ###+,  *.    ;#################### ;*+########@*              ....
 ^   =       ,##* #^ .     +=####################*       ^+####,            ,...
 +  $$;,     =##$*#@;     $######################+#  =;  . ,.@#@ ^           ...
 =^^===$^    #####&=*++^*@*#######################=        #; =##            ...
 ===+==$.    #####################################+**   ;+##**+## ;;.         ..
 $==+==&     ######################################**@$+*^$+####    ^;;.,     ..
.$======     ####################+@#############################    ,*^        .
=$.====+     ###################.@#..##########################    .            
$;;$==$,,$   @##################^##*+#########################+          ==     
+;$==+=+=$,   ################################################         &&@*     
;====+==+=$   =##############################################;        $#+    ^^ 
$===+==+++&+;@,*#############################################         *,      $=
=+====+=*$@@@&@*.#################@==#######################.                 =+
==++===*=@&&&&&@= *########################################^           ;   ;=+=+
==++==++@&&&&&&&@@^ *####################################&           ,===^;===++
==+==+*&@&&&&&&&&&@@* =################################&=         *;^=+++==+++++
+++=+*$@&&&&&&&&&&&&&@^;=###########################&$$;        ;==$&&*+++=+++++
+=++++@&&&&&&&&&&&&&&@@@&*=@#####################@=*=^;       ^&@&&&&@=*++++++++
=+++*&@&&&@&&@@#@@@=*+*^.    *=#############@*^.,;;=+       *@@&&&&&&&@*++=+++++
</pre>

this ascii-art was made by the [asciify](https://github.com/duszku/asciify) gang
