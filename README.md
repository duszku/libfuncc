# Libfuncc

Libfuncc is a C library that ports behaviours from Haskell to C. New stuff is
added whenever I need it, so don't expect it to grow large quickly unless I
get unusually inspired for this particular project. Right now it only has lists
module and a skeleton of tuples module.

## Contents

- Haskell-like **lists** with some subset of `Data.List` functions ported
(folds, map, find, filter, take, drop and few others)
- A very basic **tuples** module

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
