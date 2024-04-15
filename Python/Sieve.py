# Tristan Baskerville
# MAT361
# DigiPen 2023

import math
from PrimeHelpers import isPrime

# returns all primes within [2, n]
def get_primes(n):
    primes = []  # build our list of primes
    for i in range(2, n + 1):
        if isPrime(i):
            primes.append(i)

    return primes

def sieve(a, b):
    if a <= 1:
        a = 2  # we're getting rid of 1 anyway
    output = []
    max_prime = math.isqrt(b)

    primes = get_primes(max_prime)

    curr = a
    end = b + 1
    while curr < end:
        # chunking to avoid memory exceptions
        r = min((end - curr), 5000)
        output.extend(sieve_aux(curr, curr + r, primes))
        curr += r

    return output

def sieve_aux(a, b, primes):
    def first_multiple(p, list):
        for i in range(len(list)):
            if list[i] % p == 0:
                return i

    def every_nth(start, n, list, value):
        for i in range(start, len(list), n):
            list[i] = value

    # walk the output list backwards and remove all -1 encountered
    def shrink_list(list):
        return [e for e in list if e != -1]

    output = list(range(a, b))

    for p in primes:
        if p > b:
            break

        i = first_multiple(p, output)
        if i == None:
            continue  # no multiple of p

        if output[i] == p:  # we don't want to remove the prime itself
            i += p
        # start traversing every pth element and remove
        every_nth(i, p, output, -1)

    return shrink_list(output)
