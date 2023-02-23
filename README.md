# Libfuncc

Libfuncc is a C library that ports behaviours from Haskell to C. New stuff is
added whenever I need it, so don't expect it to grow large quickly unless I
get unusually inspired for this particular project. Right now it only has lists
module and a skeleton of tuples module.

## Contents

- Haskell-like **lists** with some subset of `Data.List` functions ported
(folds, map, find, filter, take and few others)
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
requires doxygen and pdflatex and creates _(not gonna lie, not a very
well-written)_ documentation.

## How to use the lists module

By _a list_ we mean a pointer to `struct flist`. Once you create it and set it
NULL, a call to either `flist_append()` or `flist_prepend()` will create a new
list for you and return a pointer to it. Thus every call to the aforementioned
should take form of:

```c
struct  flist *list = NULL;

list = flist_append(list, /* YOUR DATA */, /* FLAGS */);
list = flist_append(list, /* MORE DATA */, /* FLAGS */);
list = flist_append(list, /* MOAR DATA */, /* FLAGS */);
```

To mimic generics, `flist`s operate on void pointers. Thus data should be passed
to the `flist_(ap|pre)pend()` subroutines in form of pointers. These can point
to both stack- and heap-allocated data. You **need** however to let the function
know which one is it as once your list will be freed, behaviour of the cleaning
function will depend on it. You have three flags at your disposal:

- `FLIST_DONTCLEAN` - primary intended use is for stack-allocated data. No
cleaning function will ever, under any circumstances try to free this resource.
- `FLIST_CLEANABLE` - primary intended use is for heap-allocated data that you
will not need anywhere outside the list and upon cleanup it can be freed.
- `FLIST_CLEANPROT` - by itself it has no meaning however if you bitwise-or it
with `FLIST_CLEANABLE`, then the resource will not be freed by default however
there will be a way to force this behaviour if necessary.

To free resources taken by list use `flist_free()` subroutine. It traverses the
list and cleans every node. This function takes an integer parameter `force`. If
it is nonzero, all `FLIST_CLEANPROT` flags are ignored and all cleanable
resources are freed anyway. If it is zero, data declared as protected is left
untouched.

The way `flist_free()` deals with data stored in the nodes can be modified
anytime before `flist_free()` call by calling `flist_set_cleanup()`. If your
data is not a simple int or a char and needs more sophisticated _destructor_,
you may give a pointer to said destructor as a new cleanup handler by this
subroutine. This may be useful for data such as structs with dynamically
allocated fields that will become unreachable after the struct itself is freed.

Most of the remaining functions **modify the input list**. Thus if you need to
preserve its previous state, use `flist_copy()`. As name suggests it creates a
copy of its input list. Such a copy can be either shallow (simply put all
pointers to the new list as they were) or deep. A deep copy will be performed if
second parameter of `flist_copy()` is not NULL and points to a function which
is capable of creating dynamically allocated copies of data stored within the
list.

Deep-copied lists have all their elements with `FLIST_CLEANABLE` flag. On the
other hand, shallow-copied lists have `FLIST_CLEANPROT` set additionally for all
elements that had the cleanable flag. This protects from potential
double-freeing of resources.

This was just to give you a quick start. Remaining functions behave somewhat
similarly to their Haskell counterparts and the doxygen-generated documentation
should be enough for you to figure out how everything works. I may create a
proper set of manpages for it sometime, but today is not the day.

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
