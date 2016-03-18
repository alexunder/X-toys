---
layout: post
title:  SICP Exercise 
category: notes
---

# Chapter 1 Building Abstractions with Procedures #

## Exercise 1.1 ##
Below is a sequence of expressions. What is the result printed by the interpreter in response to each expression? Assume that the sequence is to be evaluated in the order in which it is presented.

```scheme
10
->10

(+ 5 3 4)
->12

(- 9 1)
->8

(/ 6 2)
->3

(+ (* 2 4) (- 4 6))
->6

(define a 3)
(define b (+ a 1))

(+ a b (* a b))
->19

(= a b)
->#f

(if (and (> b a) (< b (* a b)))
    b
    a)
->4

(cond ((= a 4) 6)
      ((= b 4) (+ 6 7 a))
    (else 25))
->16

(+ 2 (if (> b a) b a))
->6

(* (cond ((> a b) a)
         ((< a b) b)
         (else -1))
   (+ a 1))
->16
```

## Exercise 1.2 ##

Translate the following expression into prefix form:

$$
\frac{5 + 4 + (2 − (3 − (6 + \frac{4}{5} )))}{3(6 − 2)(2 − 7)}
$$

```scheme
(/ (+ 5 4 (- 2 (- 3 (+ 6 (/ 4 5)))))
   (* 3 (- 6 2) (- 2 7)) )
```

## Exercise 1.3 ##

Define a procedure that takes three numbers as arguments and returns the sum of the squares of the two larger numbers.

```scheme
(define (two-larger-of-square a b c)
    (cond ((and (> a b) (> b c)) (sum-of-squares a b))
          ((and (> a b) (< b c)) (sum-of-squares a c))
          ((and (< a b) (> a c)) (sum-of-squares a b))
          ((and (< a b) (< a c)) (sum-of-squares b c))
          (else (sum-of-squares a b))))
       
(define (sum-of-squares a b)
    (+ (square a) (square b)))  
       
(define (square x)
    (* x x))
```

## Exercise 1.4 ##

Observe that our model of evaluation allows for combinations whose operators are compound expressions. Use this observation to describe the behavior of the following procedure:

```scheme
(define (a-plus-abs-b a b)
    ((if (> b 0) + -) a b))
```

If b is larger than 0, we should add a and b, otherwise the operator '-' will be used.

## Exercise 1.5 ##

Ben Bitdiddle has invented a test to determine whether the interpreter he is faced with is using applicative-order evaluation or normal-order evaluation. He defines the following two procedures:

```scheme
(define (p) (p))

(define (test x y)
    (if (= x 0) 0 y))
```

Then he evaluates the expression

```scheme
(test 0 (p))
```

What behavior will he observe with an interpreter that uses normal-order evaluation? Explain your answer. (Assume that the evaluation rule for the special form if is the same whether the interpreter is using normal or applicative order: The predicate expression is evaluated first, and the result determines whether to evaluate the consequent or the alternative expression.)