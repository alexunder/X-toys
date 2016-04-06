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

### Answer ###

According to the textbooks, applicative-order evaluation is that the parameter should be calculated before it is passed into the procedure. However, normal-order evaluation is kind of policy that the final value of parameter will be calculated when it is needed inside the procedure.

Therefore, let's talk about it by two cases:

**1. Applicative-order evaluation**

Before step into the test procedure, its two parameters must be confirmed. Then (define (p) (p)) will run, it's always Substitute itself with p, finally, it stuck on the dead loop.

**2. Normal-order evaluation evaluation**

Under this case, we will directly step into the test procedure, because x equals 0, the final result is 0 according the procedure's logical rutine. The value of y is not needed to calculate.

## Exercise 1.6 ##

Alyssa P. Hacker doesn’t see why if needs to be provided as a special form. “Why can’t I just define it as an ordinary procedure in terms of cond ?” she asks. Alyssa’s friend Eva Lu Ator claims this can indeed be done, and she
defines a new version of if :

```scheme
(define (new-if predicate then-clause else-clause)
   (cond (predicate then-clause)
   (else else-clause)))
```

Eva demonstrates the program for Alyssa:

```scheme
(new-if (= 2 3) 0 5)
5
(new-if (= 1 1) 0 5)
0
```

Delighted, Alyssa uses new-if to rewrite the square-root program:

```scheme
(define (sqrt-iter guess x)
   (new-if (good-enough? guess x)
   guess
   (sqrt-iter (improve guess x) x)))
```

What happens when Alyssa atempts to use this to compute
square roots? Explain.

### Anwser ###

After calling the new procedure, something bad will happen:

```scheme
1 ]=> (sqrt-new 9)

;Aborting!: maximum recursion depth exceeded
```

According to the applicative-order evaluation which is used by scheme, before going into the sqrt-iter procedure, its parameters must be evaluated, then sqrt-iter will be called recursively forever until the calling depth exceeds the maximum of scheme.

## Exercise 1.7 ##

The good-enough? test used in computing square roots will not be very effective for finding the square roots of very small numbers. Also, in real computers, arithmetic operations are almost always performed with limited precision. This makes our test inadequate for very large numbers. Explain these statements, with examples showing how the test fails for small and large numbers. An alternative strategy for implementing good-enough? is to watch how guess changes from one iteration to the next and to stop when the change is a very small fraction of the guess. Design a square-root procedure that uses this kind of end test. Does this work better for small and large numbers?