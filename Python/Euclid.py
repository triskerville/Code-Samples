# Tristan Baskerville
# MAT361
# DigiPen 2023

from math import prod
from NT_functions import modexp

def multinv(a, m):
    gcd, x0, y0 = euclid(a, m)

    if gcd != 1:
        raise ArithmeticError(
            f"Multiplicative inverse does NOT exist:  gcd({a},{m}) = {gcd} != 1"
        )

    return x0 % m

def euclid(a, b):
    prev_r, r = a, b
    x, prev_x = 0, 1
    y, prev_y = 1, 0
    while r != 0:
        prev_r, (q, r) = r, divmod(prev_r, r)
        x, prev_x = prev_x - (q * x), x
        y, prev_y = prev_y - (q * y), y
    # gcd, x0, y0
    return prev_r, prev_x, prev_y

def euclid_gcd(a, b): # I'm too lazy to rewrite, I just want the gcd
    gcd, x0, y0 = euclid(a, b)
    return gcd

def chinrem(aList, mList):
    M = prod(mList)
    bList = []
    for m in mList:
        bList.append(multinv(M // m, m))
    total = 0
    for i in range(0, len(aList)):
        total += aList[i] * bList[i] * (M // mList[i])
    return total % M, M

def linear_equation_solver(a, b, c):
    gcd, x0, y0 = euclid(a, b)
    if c % gcd != 0:
        print(
            "No solutions: gcd({}, {}) = {}, which does NOT divide {}".format(
                a, b, gcd, c
            )
        )
        return

    a0 = a // gcd
    b0 = b // gcd
    mult = c // gcd

    print("x = {} + {}t".format(x0 * mult, b0))
    print("y = {} - {}t".format(y0 * mult, a0))

def congruency_solver(a, b, m):
    m0 = m
    gcd, x0, y0 = euclid(a, m)
    if m % gcd != 0:
        print("No solutions.")
        return

    a //= gcd
    b //= gcd
    m //= gcd

    inv = multinv(a, m)
    print("Multinv: {}".format(inv))
    b *= inv
    print("Initial b = {}".format(b))
    b %= m

    solutions = [b]
    while b + m < m0:
        b += m
        solutions.append(b)
    return solutions

def criterion(a, p):
    return modexp(a, (p - 1) // 2, p)