# Kitanai

Tiny and dirty esoteric language I made to learn stuff about programming language creation, I plan to make a way better and cleaner language later !

## Example

### Hello, world!

```bash
print "Hello, world!"
```

### Fibonacci

```bash
$0[int(input "Fib : ")]
$1[1]$2[1]
#
    $3[$1@]
    $1[$2@]
    $2[add @ ($3@)]
    $0[sub @ 1]
?(neq ($0@) 1) (&1)
print($1@)
```

### "More or less" game

```bash
print "Welcome on the more or less game"
$"secretNumber" [int(mul random 100)]
$"try" [6]
$"numberFound" [0]

#
    $"userInput" [int(input(add(add "Enter a number (" ($"try"@)) " tries) : "$"userInput"))]
    &2
    ?(eq ($"try"@) 0) (
        &3
    )
    ?(eq ($"numberFound"@) 1) (
        &4
    )
    &1
#
    ?(eq ($"secretNumber"@) ($"userInput"@)) (
        $"numberFound" [1]
        &~
    ) (
        $"try" [sub @ 1]
        ?(gt ($"secretNumber"@) ($"userInput"@)) (
            print "It's more !"
        ) (
            print "It's less !"
        )
        &~
    )
#
    print "Looser"
    %
#
    print(add(add "Winner in " (sub 6 ($"try"@))) " tries")
%
```

## Links

- [Esolangs](https://esolangs.org/wiki/Kitanai)

## Changelog

Version 1.0 :
First version of the language

Version 1.1 :
Added possibility to create functions, support recursivity and cross functions calls
